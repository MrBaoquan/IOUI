#ifndef NETIO_H
#define NETIO_H

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <mutex>
#include "json/single_include/nlohmann/json.hpp" // JSON 头文件
#include <iostream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <set> // 用于存储唯一的 IP 地址
#include <chrono> // 用于计时
#include <unordered_map> // 用于存储事件映射
#include <list> // 用于存储事件队列
#include <deque> // 用于存储事件队列
#include <windows.h> // 用于获取进程名
#include <psapi.h> // 用于 EnumProcessModules 函数

// 定义事件对象结构
struct Event {
	static const int MAX_ID = 1000000; // 定义一个最大 ID 值
	static std::atomic<int> id_counter; // 静态成员，用于生成唯一事件 ID
	int id; // 事件 ID
	std::string evt; // 事件类型名称
	std::string msg; // 提示消息，默认为空
	std::map<std::string, std::string> data; // 存储参数的映射

	// 默认构造函数
	Event() : id(id_counter++ % MAX_ID), evt(""), msg(""), data() {}

	// 从 JSON 对象构造事件对象
	static Event fromJson(const nlohmann::json& json) {
		Event event;
		try {
			if (json.contains("id")) {
				event.id = json["id"].get<int>();
			}
			if (json.contains("evt")) {
				event.evt = json["evt"].get<std::string>();
			}
			if (json.contains("msg")) {
				event.msg = json["msg"].get<std::string>();
			}
			if (json.contains("data") && json["data"].is_object()) {
				for (const auto& [key, value] : json["data"].items()) {
					event.data[key] = value.get<std::string>();
				}
			}
		}
		catch (const nlohmann::json::parse_error& e) {
			event.evt = "error"; // 设置事件类型为 error
			event.msg = e.what(); // 将异常消息放入 msg
		}
		catch (const std::exception& e) {
			event.evt = "error"; // 设置事件类型为 error
			event.msg = e.what(); // 将异常消息放入 msg
		}
		return event;
	}

	// 将 Event 转换为 JSON 对象
	nlohmann::json toJson() const {
		nlohmann::json json;
		json["id"] = id; // 事件 ID
		json["evt"] = evt; // 事件类型名称
		json["msg"] = msg; // 提示消息
		json["data"] = nlohmann::json::object();
		for (const auto& [key, value] : data) {
			json["data"][key] = value; // 添加到 JSON 对象
		}
		return json;
	}

	// 重载运算符以比较两个事件
	bool operator==(const Event& other) const {
		return id == other.id; // 通过 ID 比较事件
	}
};

// NetIO 类定义
class NetIO {
public:
	NetIO(const std::string& host, const std::string& port);
	NetIO(const std::string& port);
	~NetIO();

	void start();
	void stop();
	bool isServiceCreated() const; // 返回服务是否成功创建
	bool popEvent(Event& event); // 弹出任意事件
	bool popEvent(Event& event, const std::string& evtName); // 按事件类型弹出事件
	bool peekEvent(Event& event); // 尝试获取下一个事件，不改变事件队列
	bool popEvents(std::vector<Event>& events, const std::string& evtName, size_t maxCount); // 返回最新的 n 个事件集合
	bool sendEvent(const Event& event, const std::string& target_host, const std::string& target_port); // 更新 sendEvent 方法
	void sendBroadcast(const Event& event, const std::string& port);
	void broadcastHeartbeatEvent(const std::string& custom_msg); // 广播自身 IP 和端口信息
	void checkAndBroadcastHeartbeat(const std::string& custom_msg, int interval_seconds); // 检查并广播
	void setEventLimit(const std::string& evtName, size_t limit); // 设置特定事件类型的数量限制
	void listenForActiveRequests(); // 新增：监听 IsActive 请求

private:
	bool createUDPService(const std::string& host, const std::string& port);
	void receiveMessages();
	void handleMessage(const std::vector<BYTE>& message, const boost::asio::ip::udp::endpoint& sender_endpoint);
	void handleActiveRequest(const Event& event, const boost::asio::ip::udp::endpoint& sender_endpoint); // 处理 IsActive 请求
	std::set<std::string> getLocalIPAddresses(); // 获取本机所有 IP 地址
	static std::string getProcessName(); // 获取当前进程名称

	boost::asio::io_context io_context_;
	boost::asio::ip::udp::socket socket_; // 用于主服务的 socket
	boost::asio::ip::udp::socket active_socket_; // 用于监听 IsActive 请求的独立 socket
	boost::asio::ip::udp::endpoint sender_endpoint_;
	std::unordered_map<std::string, std::list<Event>> event_map_; // 每种事件类型的列表，直接存储 Event
	std::unordered_map<std::string, size_t> event_limits_; // 每种事件类型的数量限制
	std::mutex map_mutex_; // 保护事件映射的并发访问
	std::deque<Event> event_queue_; // 事件队列
	std::mutex queue_mutex_; // 保护事件队列的并发访问
	std::thread receive_thread_;
	std::thread active_receive_thread_; // 新增：用于处理 IsActive 请求的线程
	std::atomic<bool> running_;
	std::atomic<bool> stop_requested_;
	std::condition_variable cv_;
	std::mutex cv_mutex_;
	bool service_created_; // 服务是否成功创建
	std::string port_; // 保存传入的端口号
	std::chrono::steady_clock::time_point last_broadcast_time_; // 上次广播的时间

	static const std::string BROADCAST_PORT; // 静态常量广播端口
	static const std::string ACTIVE_REQUEST_PORT; // 监听的活动请求端口
	static std::string process_name_; // 静态成员变量，进程名称
};

#endif // NETIO_H
