#include "NetIO.h"

// 定义固定的广播端口
const std::string NetIO::BROADCAST_PORT = "21000"; // 广播端口固定为 21000

// 构造函数，接受 host 和 port
NetIO::NetIO(const std::string& host, const std::string& port)
    : socket_(io_context_), running_(false), stop_requested_(false), service_created_(false), port_(port) {
    service_created_ = createUDPService(host, port);
    last_broadcast_time_ = std::chrono::steady_clock::now(); // 初始化上次广播时间
}

// 构造函数，接受 port 号（默认 host 为 0.0.0.0）
NetIO::NetIO(const std::string& port)
    : socket_(io_context_), running_(false), stop_requested_(false), service_created_(false), port_(port) {
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
    catch (std::exception& e) {
        std::cerr << "Failed to create UDP service: " << e.what() << std::endl;
        return false;
    }
}

// 启动接收线程
void NetIO::start() {
    if (running_) return;
    running_ = true;
    receive_thread_ = std::thread(&NetIO::receiveMessages, this);
}

// 停止服务
void NetIO::stop() {
    if (!running_) return;
    stop_requested_ = true;
    socket_.close();
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
    running_ = false;
}

// 检查并广播
void NetIO::checkAndBroadcast(const std::string& custom_msg, int interval_seconds) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - last_broadcast_time_).count();

    if (elapsed_time > interval_seconds) {
        broadcastSelfInfo(custom_msg); // 调用广播
        last_broadcast_time_ = now; // 更新上次广播时间
    }
}

// 返回服务是否成功创建
bool NetIO::isServiceCreated() const {
    return service_created_;
}

// 从事件队列中弹出事件
bool NetIO::popEvent(Event& event) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (event_queue_.empty()) return false;
    event = event_queue_.front();
    event_queue_.pop();
    return true;
}

// 发送事件
void NetIO::sendEvent(const Event& event, const std::string& target_host, const std::string& target_port) {
    // 将事件转换为 JSON 字符串
    nlohmann::json json = event.toJson();
    std::string message = json.dump();

    boost::asio::ip::udp::endpoint target_endpoint(boost::asio::ip::address::from_string(target_host), std::stoi(target_port));
    socket_.send_to(boost::asio::buffer(message), target_endpoint);
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
void NetIO::broadcastSelfInfo(const std::string& custom_msg) {
    Event event;
    event.evt = "broadcast"; // 设置事件类型为 broadcast
    event.msg = custom_msg; // 将自定义消息传入

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

// 接收消息线程
void NetIO::receiveMessages() {
    while (!stop_requested_) {
        std::vector<BYTE> buffer(1024);
        boost::asio::ip::udp::endpoint sender_endpoint;

        boost::system::error_code error;
        size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint, 0, error);

        if (error) {
            std::cerr << "Receive error: " << error.message() << std::endl;
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

        // 响应成功处理的请求
        responseEvent.evt = "response"; // 设置 evt 为 response
        responseEvent.msg = "ok"; // 设置 msg 为 ok

        // 发送响应
        sendEvent(responseEvent, sender_endpoint.address().to_string(), std::to_string(sender_endpoint.port()));

        // 将事件放入队列
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            event_queue_.push(event);
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
