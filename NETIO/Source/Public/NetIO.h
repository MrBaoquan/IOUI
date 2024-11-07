#ifndef NETIO_H
#define NETIO_H

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include "json/single_include/nlohmann/json.hpp" // JSON 头文件
#include <iostream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <set> // 用于存储唯一的 IP 地址
#include <chrono> // 用于计时

// 定义事件对象结构
struct Event {
    std::string evt; // 事件类型名称
    std::string msg; // 提示消息，默认为空
    std::map<std::string, std::string> data; // 存储参数的映射

    // 默认构造函数
    Event() : evt(""), msg(""), data() {}

    // 从 JSON 对象构造事件对象
    static Event fromJson(const nlohmann::json& json) {
        Event event;
        try {
            if (json.contains("evt")) {
                event.evt = json["evt"].get<std::string>();
            }
            if (json.contains("msg")) {
                event.msg = json["msg"].get<std::string>();
            }
            if (json.contains("data") && json["data"].is_object()) {
                for (const auto& [key, value] : json["data"].items()) {
                    // 使用 value.get<std::string>() 直接获取字符串
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
        json["evt"] = evt; // 事件类型名称
        json["msg"] = msg; // 提示消息
        json["data"] = nlohmann::json::object();
        for (const auto& [key, value] : data) {
            json["data"][key] = value; // 添加到 JSON 对象
        }
        return json;
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
    bool popEvent(Event& event);
    void sendEvent(const Event& event, const std::string& target_host, const std::string& target_port);
    void sendBroadcast(const Event& event, const std::string& port);
    void broadcastSelfInfo(const std::string& custom_msg); // 新增方法：广播自身 IP 和端口信息
    void checkAndBroadcast(const std::string& custom_msg, int interval_seconds); // 新增接口：检查并广播

private:
    bool createUDPService(const std::string& host, const std::string& port);
    void receiveMessages();
    void handleMessage(const std::vector<BYTE>& message, const boost::asio::ip::udp::endpoint& sender_endpoint);
    std::set<std::string> getLocalIPAddresses(); // 获取本机所有 IP 地址

    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_;
    std::queue<Event> event_queue_;
    std::mutex queue_mutex_;
    std::thread receive_thread_;
    std::atomic<bool> running_;
    std::atomic<bool> stop_requested_;
    std::condition_variable cv_;
    std::mutex cv_mutex_;
    bool service_created_; // 服务是否成功创建
    std::string port_; // 保存传入的端口号
    std::chrono::steady_clock::time_point last_broadcast_time_; // 上次广播的时间

    static const std::string BROADCAST_PORT; // 静态常量广播端口
};

#endif // NETIO_H
