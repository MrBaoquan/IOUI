#include "NetIO.h"
#include <windows.h> // 用于获取进程名及OutputDebugStringA
#include <psapi.h>   // 用于 EnumProcessModules 函数
#include <iostream>  // 用于 std::cout（可根据需要移除）
#include <mutex>     // C++17 std::scoped_lock

// 静态成员初始化
std::atomic<int> Event::id_counter(0);
std::string NetIO::process_name_ = NetIO::getProcessName();

// 定义固定的广播端口和活动请求端口
const std::string NetIO::BROADCAST_PORT = "21000";		// 广播端口
const std::string NetIO::ACTIVE_REQUEST_PORT = "22000";	// 用于netio-admin 主动发出的探测心跳的端口

// 构造函数，接受 host 和 port
NetIO::NetIO(const std::string& host, const std::string& port)
	: socket_(io_context_), active_socket_(io_context_), running_(false),
	stop_requested_(false), service_created_(false), port_(port) {
	service_created_ = createUDPService(host, port);
	last_broadcast_time_ = std::chrono::steady_clock::now();
}

// 构造函数，接受 port 号（默认 host 为 0.0.0.0）
NetIO::NetIO(const std::string& port)
	: socket_(io_context_), active_socket_(io_context_), running_(false),
	stop_requested_(false), service_created_(false), port_(port) {
	service_created_ = createUDPService("0.0.0.0", port);
	last_broadcast_time_ = std::chrono::steady_clock::now();
}

// 析构函数
NetIO::~NetIO() {
	stop();
}

// 创建 UDP 服务
bool NetIO::createUDPService(const std::string& host, const std::string& port) {
	try {
		boost::asio::ip::udp::resolver resolver(io_context_);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, port);
		boost::asio::ip::udp::endpoint endpoint = *resolver.resolve(query).begin();
		socket_.open(endpoint.protocol());

		// 启用广播选项
		boost::asio::socket_base::broadcast option(true);
		socket_.set_option(option);
		socket_.bind(endpoint);

		return true;
	}
	catch (std::exception&) {
		return false;
	}
}

// 启动接收线程
void NetIO::start() {
	if (running_) return;
	running_ = true;

	receive_thread_ = std::thread(&NetIO::receiveMessages, this);
	listenForActiveRequests();
}

// 停止服务
void NetIO::stop() {
	if (!running_) return;

	stop_requested_ = true;

	boost::system::error_code ec1, ec2;
	socket_.close(ec1);
	active_socket_.close(ec2);

	if (receive_thread_.joinable()) {
		receive_thread_.join();
	}

	if (active_receive_thread_.joinable()) {
		active_receive_thread_.join();
	}

	running_ = false;
}

// 创建用于监听 IsActive 请求的 socket
void NetIO::listenForActiveRequests() {
	try {
		boost::asio::ip::udp::endpoint active_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), std::stoi(ACTIVE_REQUEST_PORT));
		active_socket_.open(active_endpoint.protocol());
		active_socket_.bind(active_endpoint);

		active_receive_thread_ = std::thread([this]() {
			while (!stop_requested_) {
				std::vector<BYTE> buffer(2048);
				boost::asio::ip::udp::endpoint sender_endpoint;
				boost::system::error_code error;

				size_t len = active_socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint, 0, error);

				if (!error) {
					handleMessage(std::vector<BYTE>(buffer.begin(), buffer.begin() + len), sender_endpoint);
				}
			}
			});
	}
	catch (const std::exception&) {
	}
}

// 检查并广播
void NetIO::checkAndBroadcastHeartbeat(const std::string& custom_msg, int interval_seconds) {
	auto now = std::chrono::steady_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - last_broadcast_time_).count();

	if (elapsed_time > interval_seconds) {
		broadcastHeartbeatEvent(custom_msg);
		last_broadcast_time_ = now;
	}
}

// 返回服务是否成功创建
bool NetIO::isServiceCreated() const {
	return service_created_;
}

// 弹出任意事件
bool NetIO::popEvent(Event& event) {
	std::scoped_lock lock(map_mutex_, queue_mutex_);

	if (!event_queue_.empty()) {
		event = event_queue_.front();
		event_queue_.pop_front();

		for (auto& pair : event_map_) {
			auto& events = pair.second;
			auto it = std::remove_if(events.begin(), events.end(), [&](const Event& e) { return e == event; });
			if (it != events.end()) {
				events.erase(it, events.end());
				break;
			}
		}
		return true;
	}
	return false;
}

// 按事件类型弹出事件
bool NetIO::popEvent(Event& event, const std::string& evtName) {
	std::lock_guard<std::mutex> map_lock(map_mutex_);

	auto it = event_map_.find(evtName);
	if (it != event_map_.end() && !it->second.empty()) {
		event = it->second.front();
		it->second.pop_front();

		std::lock_guard<std::mutex> queue_lock(queue_mutex_);
		event_queue_.erase(std::remove_if(event_queue_.begin(), event_queue_.end(),
			[&](const Event& e) { return e == event; }), event_queue_.end());
		return true;
	}
	return false;
}

// 弹出指定数量的事件
bool NetIO::popEvents(std::vector<Event>& events, const std::string& evtName, size_t maxCount) {
	std::lock_guard<std::mutex> map_lock(map_mutex_);
	auto it = event_map_.find(evtName);

	if (it != event_map_.end() && !it->second.empty()) {
		std::list<Event>& eventList = it->second;
		size_t currentCount = eventList.size();
		size_t countToPop = std::min(currentCount, maxCount);

		events.clear();
		for (size_t i = 0; i < countToPop; ++i) {
			events.push_back(eventList.front());
			eventList.pop_front();
		}

		{
			std::lock_guard<std::mutex> queue_lock(queue_mutex_);
			for (const auto& ev : events) {
				event_queue_.erase(std::remove_if(event_queue_.begin(), event_queue_.end(),
					[&](const Event& e) { return e == ev; }), event_queue_.end());
			}
		}

		return true;
	}
	return false;
}

// 尝试获取下一个事件，不改变事件队列
bool NetIO::peekEvent(Event& event) {
	std::lock_guard<std::mutex> lock(queue_mutex_);

	if (!event_queue_.empty()) {
		event = event_queue_.front();
		return true;
	}
	return false;
}

// 发送事件
bool NetIO::sendEvent(const Event& event, const std::string& target_host, const std::string& target_port) {
	try {
		nlohmann::json json = event.toJson();
		std::string message = json.dump();

		boost::asio::ip::udp::endpoint target_endpoint(boost::asio::ip::address::from_string(target_host), std::stoi(target_port));
		socket_.send_to(boost::asio::buffer(message), target_endpoint);

		return true;
	}
	catch (...) {
		return false;
	}
}

// 发送广播事件
void NetIO::sendBroadcast(const Event& event, const std::string& port) {
	nlohmann::json json = event.toJson();
	std::string message = json.dump();

	boost::asio::ip::udp::endpoint broadcast_endpoint(boost::asio::ip::address_v4::broadcast(), std::stoi(port));
	socket_.send_to(boost::asio::buffer(message), broadcast_endpoint);
}

// 广播自身 IP 和端口号
void NetIO::broadcastHeartbeatEvent(const std::string& custom_msg) {
	Event event;
	event.evt = "Heartbeat";
	event.msg = custom_msg;

	std::set<std::string> ipAddresses = getLocalIPAddresses();
	int index = 0;

	for (const auto& ip : ipAddresses) {
		std::string key = "ip_" + std::to_string(index);
		std::string value = ip + "_" + port_;
		event.data[key] = value;
		index++;
	}

	event.data["process_name"] = process_name_;

	sendBroadcast(event, BROADCAST_PORT);
}

// 获取本机所有 IP 地址
std::set<std::string> NetIO::getLocalIPAddresses() {
	std::set<std::string> ipAddresses;
	boost::asio::io_service io_service;
	boost::asio::ip::udp::resolver resolver(io_service);

	std::string host_name = boost::asio::ip::host_name();

	boost::asio::ip::udp::resolver::query query(host_name, "");
	boost::asio::ip::udp::resolver::iterator it = resolver.resolve(query);
	boost::asio::ip::udp::resolver::iterator end;

	for (; it != end; ++it) {
		boost::asio::ip::address addr = it->endpoint().address();
		if (addr.is_v4()) {
			ipAddresses.insert(addr.to_string());
		}
	}

	return ipAddresses;
}

std::string GbkToUtf8(const std::string& gbkStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, wstr, len);

	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);

	std::string strTemp = str;
	delete[] wstr;
	delete[] str;

	return strTemp;
}

// 获取当前进程名称（不包括路径和后缀名）
std::string NetIO::getProcessName() {
	wchar_t process_name[MAX_PATH] = { 0 };
	DWORD process_id = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);

	if (hProcess != NULL) {
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleFileNameExW(hProcess, hMod, process_name, sizeof(process_name) / sizeof(wchar_t));
		}
		CloseHandle(hProcess);
	}

	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, process_name, -1, NULL, 0, NULL, NULL);
	std::string full_process_name(bufferSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, process_name, -1, &full_process_name[0], bufferSize, NULL, NULL);

	size_t last_slash_idx = full_process_name.find_last_of("\\/");
	if (last_slash_idx != std::string::npos) {
		full_process_name = full_process_name.substr(last_slash_idx + 1);
	}

	size_t dot_idx = full_process_name.find_last_of(".");
	if (dot_idx != std::string::npos) {
		full_process_name = full_process_name.substr(0, dot_idx);
	}

	return full_process_name;
}

// 接收消息线程
void NetIO::receiveMessages() {
	while (!stop_requested_) {
		std::vector<BYTE> buffer(4096);
		boost::asio::ip::udp::endpoint sender_endpoint;
		boost::system::error_code error;

		size_t len = 0;
		try {
			len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint, 0, error);
		}
		catch (const std::exception&) {
			continue;
		}

		if (error) {
			if (error == boost::asio::error::operation_aborted || error == boost::asio::error::bad_descriptor) {
				break;
			}
			continue;
		}

		try {
			handleMessage(std::vector<BYTE>(buffer.begin(), buffer.begin() + len), sender_endpoint);
		}
		catch (const std::exception&) {
		}
	}
}

// 处理接收到的消息
void NetIO::handleMessage(const std::vector<BYTE>& message, const boost::asio::ip::udp::endpoint& sender_endpoint) {
	Event responseEvent;

	try {
		std::string jsonString(message.begin(), message.end());

		auto json = nlohmann::json::parse(jsonString);
		Event event = Event::fromJson(json);

		if (event.evt == "error") {
			sendEvent(event, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port()));
			return;
		}
		if (event.evt == "response") {
			return;
		}
		if (event.evt == "Heartbeat") {
			handleActiveRequest(event, sender_endpoint);
			return;
		}
		responseEvent.evt = "response";
		responseEvent.msg = "ok";

		{
			std::scoped_lock lock(map_mutex_, queue_mutex_);
			size_t limit = event_limits_.count(event.evt) > 0 ? event_limits_[event.evt] : SIZE_MAX;

			if (event_map_[event.evt].size() >= limit) {
				event_map_[event.evt].pop_front();
			}
			event_map_[event.evt].push_back(event);

			event_queue_.push_back(event);
		}
	}
	catch (const nlohmann::json::parse_error& e) {
		responseEvent.evt = "response";
		responseEvent.msg = "JSON parse error: " + std::string(e.what());
		sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port()));
	}
	catch (const std::exception& e) {
		responseEvent.evt = "response";
		responseEvent.msg = "Error handling message: " + std::string(e.what());
		sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port()));
	}
}

// 处理 IsActive 请求
void NetIO::handleActiveRequest(const Event& event, const boost::asio::ip::udp::endpoint& sender_endpoint) {
	Event responseEvent;
	responseEvent.evt = "HeartbeatAck";
	responseEvent.msg = "Active";

	std::set<std::string> ipAddresses = getLocalIPAddresses();
	int index = 0;

	for (const auto& ip : ipAddresses) {
		std::string key = "ip_" + std::to_string(index);
		responseEvent.data[key] = ip;
		index++;
	}

	responseEvent.data["process_name"] = process_name_;

	sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port()));
}

// 设置特定事件类型的数量限制
void NetIO::setEventLimit(const std::string& evtName, size_t limit) {
	std::lock_guard<std::mutex> lock(map_mutex_);
	event_limits_[evtName] = limit;
}
