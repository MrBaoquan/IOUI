#pragma once
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include "ProtocolFactory.h"

namespace IOHub {

// URI协议解析器
class ProtocolUri {
public:
    // 解析URI字符串
    static bool parse(const std::string& uri, std::unique_ptr<ProtocolConfig>& outConfig);
    
    // 获取协议类型字符串
    static std::string getScheme(const std::string& uri);
    
private:
    // 解析查询参数 (?key=value&key2=value2)
    static std::map<std::string, std::string> parseQueryParams(const std::string& query);
    
    // URL解码
    static std::string urlDecode(const std::string& str);
    
    // 解析主机和端口 (host:port)
    static bool parseHostPort(const std::string& hostPort, std::string& host, uint16_t& port);
    
    // 解析各种协议
    static bool parseSerial(const std::string& uri, std::unique_ptr<SerialConfig>& config);
    static bool parseUdp(const std::string& uri, std::unique_ptr<UdpConfig>& config);
    static bool parseTcp(const std::string& uri, std::unique_ptr<TcpConfig>& config);
    static bool parseTcpServer(const std::string& uri, std::unique_ptr<TcpServerConfig>& config);
};

} // namespace IOHub