#include "NetIO.h"
#include <windows.h> // 用于获取进程名
#include <psapi.h>   // 用于 EnumProcessModules 函数

// 静态成员初始化
std::atomic<int> Event::id_counter(0); // 初始化事件 ID 计数器为 0
std::string NetIO::process_name_ = NetIO::getProcessName(); // 初始化静态成员变量

// 定义固定的广播端口和活动请求端口
const std::string NetIO::BROADCAST_PORT = "21000"; // 广播端口固定为 21000
const std::string NetIO::ACTIVE_REQUEST_PORT = "22000"; // 监听的活动请求端口

// 构造函数，接受 host 和 port
NetIO::NetIO(const std::string& host, const std::string& port)
	: socket_(io_context_), active_socket_(io_context_), running_(false), stop_requested_(false), service_created_(false), port_(port) {
	service_created_ = createUDPService(host, port);
	last_broadcast_time_ = std::chrono::steady_clock::now(); // 初始化上次广播时间
}

// 构造函数，接受 port 号（默认 host 为 0.0.0.0）
NetIO::NetIO(const std::string& port)
	: socket_(io_context_), active_socket_(io_context_), running_(false), stop_requested_(false), service_created_(false), port_(port) {
	service_created_ = createUDPService("0.0.0.0", port); // 默认绑定到所有接口
	last_broadcast_time_ = std::chrono::steady_clock::now(); // 初始化上次广播时间
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
		// std::cerr << "Failed to create UDP service: " << e.what() << std::endl;
		return false;
	}
}

// 启动接收线程
void NetIO::start() {
	if (running_) return;
	running_ = true;
	receive_thread_ = std::thread(&NetIO::receiveMessages, this);
	listenForActiveRequests(); // 启动监听 IsActive 请求
}

// 停止服务
void NetIO::stop() {
	if (!running_) return;
	stop_requested_ = true;
	socket_.close();
	active_socket_.close(); // 关闭独立的活动请求 socket
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
				else {
					// std::cerr << "Receive error on active socket: " << error.message() << std::endl;
				}
			}
			});
		std::cout << "Listening for IsActive requests on port " << ACTIVE_REQUEST_PORT << std::endl;
	}
	catch (const std::exception& e) {
		// std::cerr << "Failed to listen on port " << ACTIVE_REQUEST_PORT << ": " << e.what() << std::endl;
	}
}

// 检查并广播
void NetIO::checkAndBroadcastHeartbeat(const std::string& custom_msg, int interval_seconds) {
	auto now = std::chrono::steady_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - last_broadcast_time_).count();

	if (elapsed_time > interval_seconds) {
		broadcastHeartbeatEvent(custom_msg); // 调用广播
		last_broadcast_time_ = now; // 更新上次广播时间
	}
}

// 返回服务是否成功创建
bool NetIO::isServiceCreated() const {
	return service_created_;
}

// 弹出任意事件
bool NetIO::popEvent(Event& event) {
	std::lock_guard<std::mutex> queue_lock(queue_mutex_);

	// 如果事件队列不为空，弹出队列中的事件
	if (!event_queue_.empty()) {
		event = event_queue_.front(); // 获取队列中的第一个事件
		event_queue_.pop_front(); // 从队列中移除

		// 从 event_map_ 中同步移除对应的事件
		std::lock_guard<std::mutex> map_lock(map_mutex_);
		for (auto& pair : event_map_) {
			auto& events = pair.second;
			auto it = std::remove_if(events.begin(), events.end(), [&](const Event& e) { return e == event; });
			if (it != events.end()) {
				events.erase(it, events.end()); // 从列表中移除
				break; // 找到并移除后跳出循环
			}
		}

		return true; // 返回事件
	}

	return false; // 如果没有找到匹配的事件，返回 false
}

// 按事件类型弹出事件
bool NetIO::popEvent(Event& event, const std::string& evtName) {
	std::lock_guard<std::mutex> map_lock(map_mutex_);

	// 从特定事件类型中弹出事件
	auto it = event_map_.find(evtName);
	if (it != event_map_.end() && !it->second.empty()) {
		event = it->second.front(); // 获取第一个事件
		it->second.pop_front(); // 从列表中移除

		// 同步移除事件队列中的事件
		std::lock_guard<std::mutex> queue_lock(queue_mutex_);
		event_queue_.erase(std::remove_if(event_queue_.begin(), event_queue_.end(), [&](const Event& e) { return e == event; }), event_queue_.end()); // 从队列中移除

		return true; // 找到事件
	}

	return false; // 如果没有找到匹配的事件，返回 false
}

// 弹出指定数量的事件
bool NetIO::popEvents(std::vector<Event>& events, const std::string& evtName, size_t maxCount) {
	std::lock_guard<std::mutex> map_lock(map_mutex_);
	auto it = event_map_.find(evtName);

	// 检查该事件名是否存在
	if (it != event_map_.end() && !it->second.empty()) {
		std::list<Event>& eventList = it->second; // 访问对应事件的列表
		size_t currentCount = eventList.size(); // 当前事件数量

		// 获取最新的事件
		size_t countToPop = std::min(currentCount, maxCount); // 要弹出的事件数量
		events.clear(); // 清空返回的事件集合
		for (size_t i = 0; i < countToPop; ++i) {
			events.push_back(eventList.front()); // 保存最新事件
			eventList.pop_front(); // 从列表中移除该事件
		}

		// 同步移除事件队列中的事件
		{
			std::lock_guard<std::mutex> queue_lock(queue_mutex_);
			for (const auto& event : events) {
				event_queue_.erase(std::remove_if(event_queue_.begin(), event_queue_.end(), [&](const Event& e) { return e == event; }), event_queue_.end());
			}
		}

		return true; // 返回成功
	}

	return false; // 如果没有找到匹配的事件，返回 false
}

// 尝试获取下一个事件，不改变事件队列
bool NetIO::peekEvent(Event& event) {
	std::lock_guard<std::mutex> queue_lock(queue_mutex_);

	// 如果事件队列不为空，获取队列中的第一个事件
	if (!event_queue_.empty()) {
		event = event_queue_.front(); // 获取队列中的第一个事件
		return true; // 返回成功
	}

	return false; // 如果队列为空，返回 false
}

// 发送事件
bool NetIO::sendEvent(const Event& event, const std::string& target_host, const std::string& target_port) {
	try {
		// 将事件转换为 JSON 字符串
		nlohmann::json json = event.toJson();
		std::string message = json.dump();

		boost::asio::ip::udp::endpoint target_endpoint(boost::asio::ip::address::from_string(target_host), std::stoi(target_port));
		socket_.send_to(boost::asio::buffer(message), target_endpoint);
		return true; // 发送成功
	}
	catch (const boost::system::system_error& e) {
		// std::cerr << "Socket error: " << e.what() << std::endl;
		return false; // 套接字错误
	}
	catch (const std::invalid_argument& e) {
		// std::cerr << "Invalid address or port: " << e.what() << std::endl;
		return false; // 地址或端口错误
	}
	catch (const std::exception& e) {
		// std::cerr << "Other error: " << e.what() << std::endl;
		return false; // 其他错误
	}
}

// 发送广播事件
void NetIO::sendBroadcast(const Event& event, const std::string& port) {
	// 将事件转换为 JSON 字符串
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

	// 获取本机所有 IP 地址
	std::set<std::string> ipAddresses = getLocalIPAddresses();
	int index = 0; // 用于生成 IP 数据的索引

	// 将 IP 地址和端口号添加到 event 的 data 中
	for (const auto& ip : ipAddresses) {
		std::string key = "ip_" + std::to_string(index); // 生成键名
		std::string value = ip + "_" + port_; // 格式化值为 "ip_0": "127.0.0.1_20001"
		event.data[key] = value; // 将 IP 和端口添加到 event data 中
		index++;
	}

	// 将进程名称添加到事件数据中
	event.data["process_name"] = process_name_; // 添加进程名称

	// 使用固定的广播端口进行广播
	sendBroadcast(event, BROADCAST_PORT);
}

// 获取本机所有 IP 地址
std::set<std::string> NetIO::getLocalIPAddresses() {
	std::set<std::string> ipAddresses;
	boost::asio::io_service io_service;
	boost::asio::ip::udp::resolver resolver(io_service);

	// 获取主机名
	std::string host_name = boost::asio::ip::host_name();

	// 解析主机名获取地址信息
	boost::asio::ip::udp::resolver::query query(host_name, "");
	boost::asio::ip::udp::resolver::iterator it = resolver.resolve(query);
	boost::asio::ip::udp::resolver::iterator end;

	// 遍历所有地址
	for (; it != end; ++it) {
		boost::asio::ip::address addr = it->endpoint().address();
		if (addr.is_v4()) { // 仅获取 IPv4 地址
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
	if (wstr) delete[] wstr;
	if (str) delete[] str;

	return strTemp;
}

// 获取当前进程名称（不包括路径和后缀名）
std::string NetIO::getProcessName() {
	wchar_t process_name[MAX_PATH];
	DWORD process_id = GetCurrentProcessId(); // 获取当前进程 ID
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);

	if (hProcess != NULL) {
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleFileNameExW(hProcess, hMod, process_name, sizeof(process_name) / sizeof(wchar_t));
		}
		CloseHandle(hProcess);
	}

	// 将宽字符转换为多字节字符
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, process_name, -1, NULL, 0, NULL, NULL);
	std::string full_process_name(bufferSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, process_name, -1, &full_process_name[0], bufferSize, NULL, NULL);

	// 提取进程名，不包括路径和后缀
	size_t last_slash_idx = full_process_name.find_last_of("\\/");
	if (last_slash_idx != std::string::npos) {
		full_process_name = full_process_name.substr(last_slash_idx + 1); // 去掉路径
	}

	// 去掉扩展名
	size_t dot_idx = full_process_name.find_last_of(".");
	if (dot_idx != std::string::npos) {
		full_process_name = full_process_name.substr(0, dot_idx); // 去掉扩展名
	}

	return full_process_name; // 返回进程名称
}

// 接收消息线程
void NetIO::receiveMessages() {
	while (!stop_requested_) {
		std::vector<BYTE> buffer(4096);
		boost::asio::ip::udp::endpoint sender_endpoint;

		boost::system::error_code error;
		size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint, 0, error);

		if (error) {
			// std::cerr << "Receive error: " << error.message() << std::endl;
			continue;
		}

		// 将接收到的消息处理
		handleMessage(std::vector<BYTE>(buffer.begin(), buffer.begin() + len), sender_endpoint);
	}
}

// 处理接收到的消息
void NetIO::handleMessage(const std::vector<BYTE>& message, const boost::asio::ip::udp::endpoint& sender_endpoint) {
	Event responseEvent; // 创建响应事件

	try {
		// 将 BYTE 数组转换为字符串
		std::string jsonString(message.begin(), message.end());
		auto json = nlohmann::json::parse(jsonString);

		// 从 JSON 创建事件对象
		Event event = Event::fromJson(json);

		// 检查事件是否是 error
		if (event.evt == "error") {
			// 直接将错误事件响应给发送端
			sendEvent(event, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port()));
			return; // 直接返回，不再将事件放入队列
		}

		// 如果事件类型是 "response"，则忽略该事件
		if (event.evt == "response") {
			return; // 忽略该事件，不将其加入队列
		}

		// 处理 IsActive 请求
		if (event.evt == "Heartbeat") {
			handleActiveRequest(event, sender_endpoint);
			return; // 处理完后直接返回
		}

		// 响应成功处理的请求
		responseEvent.evt = "response"; // 设置 evt 为 response
		responseEvent.msg = "ok"; // 设置 msg 为 ok

		// 发送响应
		sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port()));

		// 将事件放入映射和队列
		{
			std::lock_guard<std::mutex> map_lock(map_mutex_);
			size_t limit = event_limits_.count(event.evt) > 0 ? event_limits_[event.evt] : SIZE_MAX; // 获取当前事件的限制

			// 将事件放入 event_map_
			if (event_map_[event.evt].size() >= limit) {
				// 如果数量超过限制，删除最旧的事件
				event_map_[event.evt].pop_front(); // 删除旧事件
			}
			event_map_[event.evt].push_back(event); // 添加事件到映射

			// 同步到 event_queue_
			{
				std::lock_guard<std::mutex> queue_lock(queue_mutex_);
				event_queue_.push_back(event); // 将事件添加到事件队列
			}
		}

	}
	catch (const nlohmann::json::parse_error& e) {
		responseEvent.evt = "response"; // 设置 evt 为 response
		responseEvent.msg = "JSON parse error: " + std::string(e.what()); // 将解析错误信息传递给响应
		sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port())); // 发送响应
	}
	catch (const std::exception& e) {
		responseEvent.evt = "response"; // 设置 evt 为 response
		responseEvent.msg = "Error handling message: " + std::string(e.what()); // 将处理错误信息传递给响应
		sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port())); // 发送响应
	}
}

// 处理 IsActive 请求
void NetIO::handleActiveRequest(const Event& event, const boost::asio::ip::udp::endpoint& sender_endpoint) {
	Event responseEvent;
	responseEvent.evt = "HeartbeatAck"; // 设置响应事件类型
	responseEvent.msg = "Active"; // 设置响应消息

	// 获取本机所有 IP 地址
	std::set<std::string> ipAddresses = getLocalIPAddresses();
	int index = 0; // 用于生成 IP 数据的索引

	// 将 IP 地址和端口号添加到 responseEvent 的 data 中
	for (const auto& ip : ipAddresses) {
		std::string key = "ip_" + std::to_string(index); // 生成键名
		std::string value = ip; // 格式化值为 IP 地址
		responseEvent.data[key] = value; // 将 IP 添加到 event data 中
		index++;
	}

	// 将进程名称添加到响应数据中
	responseEvent.data["process_name"] = process_name_; // 添加进程名称

	// 发送响应
	sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port())); // 回复发送者
}

// 设置特定事件类型的数量限制
void NetIO::setEventLimit(const std::string& evtName, size_t limit) {
	std::lock_guard<std::mutex> lock(map_mutex_);
	event_limits_[evtName] = limit; // 设置事件类型的数量限制
}