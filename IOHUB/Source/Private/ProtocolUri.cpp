#include "ProtocolUri.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace IOHub {

std::string ProtocolUri::urlDecode(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%' && i + 2 < str.size()) {
            int value = 0;
            std::istringstream iss(str.substr(i + 1, 2));
            if (iss >> std::hex >> value) {
                result += static_cast<char>(value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::string ProtocolUri::getScheme(const std::string& uri) {
    size_t colonPos = uri.find("://");
    if (colonPos != std::string::npos) {
        std::string scheme = uri.substr(0, colonPos);
        std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
        return scheme;
    }
    return "";
}

std::map<std::string, std::string> ProtocolUri::parseQueryParams(const std::string& query) {
    std::map<std::string, std::string> params;
    
    if (query.empty()) {
        return params;
    }
    
    size_t start = 0;
    while (start < query.size()) {
        size_t ampPos = query.find('&', start);
        if (ampPos == std::string::npos) {
            ampPos = query.size();
        }
        
        std::string param = query.substr(start, ampPos - start);
        size_t eqPos = param.find('=');
        
        if (eqPos != std::string::npos) {
            std::string key = urlDecode(param.substr(0, eqPos));
            std::string value = urlDecode(param.substr(eqPos + 1));
            
            // 转换为小写以便不区分大小写
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            
            params[key] = value;
        }
        
        start = ampPos + 1;
    }
    
    return params;
}

bool ProtocolUri::parseHostPort(const std::string& hostPort, std::string& host, uint16_t& port) {
    size_t colonPos = hostPort.rfind(':');
    
    if (colonPos == std::string::npos) {
        return false;
    }
    
    host = hostPort.substr(0, colonPos);
    
    try {
        int portNum = std::stoi(hostPort.substr(colonPos + 1));
        if (portNum < 0 || portNum > 65535) {
            return false;
        }
        port = static_cast<uint16_t>(portNum);
        return true;
    } catch (...) {
        return false;
    }
}

bool ProtocolUri::parseSerial(const std::string& uri, std::unique_ptr<SerialConfig>& config) {
    // 格式: serial:///COM5?baudrate=115200&databits=8&stopbits=1&parity=none
    
    size_t schemeEnd = uri.find("://");
    if (schemeEnd == std::string::npos) {
        return false;
    }
    
    std::string remaining = uri.substr(schemeEnd + 3);
    
    // 分离路径和查询参数
    size_t queryPos = remaining.find('?');
    std::string path = (queryPos != std::string::npos) ? remaining.substr(0, queryPos) : remaining;
    std::string query = (queryPos != std::string::npos) ? remaining.substr(queryPos + 1) : "";
    
    // 移除路径开头的斜杠
    while (!path.empty() && path[0] == '/') {
        path = path.substr(1);
    }
    
    if (path.empty()) {
        std::cout << "[ProtocolUri] Error: Serial port name is empty" << std::endl;
        return false;
    }
    
    config = std::make_unique<SerialConfig>();
    config->portName = path;
    
    // 解析查询参数
    auto params = parseQueryParams(query);
    
    if (params.count("baudrate")) {
        try {
            config->baudRate = std::stoi(params["baudrate"]);
        } catch (...) {
            std::cout << "[ProtocolUri] Warning: Invalid baudrate, using default" << std::endl;
        }
    }
    
    if (params.count("databits")) {
        try {
            config->dataBits = std::stoi(params["databits"]);
        } catch (...) {}
    }
    
    if (params.count("stopbits")) {
        try {
            config->stopBits = std::stoi(params["stopbits"]);
        } catch (...) {}
    }
    
    if (params.count("parity")) {
        config->parity = params["parity"];
    }
    
    std::cout << "[ProtocolUri] Parsed Serial: " << config->portName 
              << " @ " << config->baudRate << std::endl;
    
    return true;
}

bool ProtocolUri::parseUdp(const std::string& uri, std::unique_ptr<UdpConfig>& config) {
    // 格式: udp://192.168.1.100:9000?localport=5000
    
    size_t schemeEnd = uri.find("://");
    if (schemeEnd == std::string::npos) {
        return false;
    }
    
    std::string remaining = uri.substr(schemeEnd + 3);
    
    // 分离地址和查询参数
    size_t queryPos = remaining.find('?');
    std::string hostPort = (queryPos != std::string::npos) ? remaining.substr(0, queryPos) : remaining;
    std::string query = (queryPos != std::string::npos) ? remaining.substr(queryPos + 1) : "";
    
    config = std::make_unique<UdpConfig>();
    
    if (!parseHostPort(hostPort, config->remoteIp, config->remotePort)) {
        std::cout << "[ProtocolUri] Error: Invalid UDP host:port" << std::endl;
        return false;
    }
    
    // 解析查询参数
    auto params = parseQueryParams(query);
    
    if (params.count("localport")) {
        try {
            config->localPort = static_cast<uint16_t>(std::stoi(params["localport"]));
        } catch (...) {
            std::cout << "[ProtocolUri] Warning: Invalid localport, using default" << std::endl;
        }
    }
    
    std::cout << "[ProtocolUri] Parsed UDP: " << config->remoteIp << ":" << config->remotePort 
              << " (local:" << config->localPort << ")" << std::endl;
    
    return true;
}

bool ProtocolUri::parseTcp(const std::string& uri, std::unique_ptr<TcpConfig>& config) {
    // 格式: tcp://192.168.1.100:8080?reconnect=3000&keepalive=true
    
    size_t schemeEnd = uri.find("://");
    if (schemeEnd == std::string::npos) {
        return false;
    }
    
    std::string remaining = uri.substr(schemeEnd + 3);
    
    // 分离地址和查询参数
    size_t queryPos = remaining.find('?');
    std::string hostPort = (queryPos != std::string::npos) ? remaining.substr(0, queryPos) : remaining;
    std::string query = (queryPos != std::string::npos) ? remaining.substr(queryPos + 1) : "";
    
    config = std::make_unique<TcpConfig>();
    
    if (!parseHostPort(hostPort, config->remoteIp, config->remotePort)) {
        std::cout << "[ProtocolUri] Error: Invalid TCP host:port" << std::endl;
        return false;
    }
    
    // 解析查询参数
    auto params = parseQueryParams(query);
    
    if (params.count("reconnect")) {
        try {
            config->reconnectIntervalMs = std::stoi(params["reconnect"]);
        } catch (...) {}
    }
    
    if (params.count("keepalive")) {
        std::string val = params["keepalive"];
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        config->enableKeepalive = (val == "true" || val == "1" || val == "yes");
    }
    
    std::cout << "[ProtocolUri] Parsed TCP: " << config->remoteIp << ":" << config->remotePort << std::endl;
    
    return true;
}

bool ProtocolUri::parseTcpServer(const std::string& uri, std::unique_ptr<TcpServerConfig>& config) {
    // 格式: tcp-server://0.0.0.0:8080?keepalive=true
    
    size_t schemeEnd = uri.find("://");
    if (schemeEnd == std::string::npos) {
        return false;
    }
    
    std::string remaining = uri.substr(schemeEnd + 3);
    
    // 分离地址和查询参数
    size_t queryPos = remaining.find('?');
    std::string hostPort = (queryPos != std::string::npos) ? remaining.substr(0, queryPos) : remaining;
    std::string query = (queryPos != std::string::npos) ? remaining.substr(queryPos + 1) : "";
    
    config = std::make_unique<TcpServerConfig>();
    
    if (!parseHostPort(hostPort, config->listenIp, config->listenPort)) {
        std::cout << "[ProtocolUri] Error: Invalid TCP Server host:port" << std::endl;
        return false;
    }
    
    // 解析查询参数
    auto params = parseQueryParams(query);
    
    if (params.count("keepalive")) {
        std::string val = params["keepalive"];
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        config->enableKeepalive = (val == "true" || val == "1" || val == "yes");
    }
    
    std::cout << "[ProtocolUri] Parsed TCP Server: " << config->listenIp << ":" << config->listenPort << std::endl;
    
    return true;
}

bool ProtocolUri::parse(const std::string& uri, std::unique_ptr<ProtocolConfig>& outConfig) {
    std::string scheme = getScheme(uri);
    
    if (scheme.empty()) {
        std::cout << "[ProtocolUri] Error: No scheme found in URI: " << uri << std::endl;
        return false;
    }
    
    if (scheme == "serial") {
        std::unique_ptr<SerialConfig> config;
        if (parseSerial(uri, config)) {
            outConfig = std::move(config);
            return true;
        }
    }
    else if (scheme == "udp") {
        std::unique_ptr<UdpConfig> config;
        if (parseUdp(uri, config)) {
            outConfig = std::move(config);
            return true;
        }
    }
    else if (scheme == "tcp") {
        std::unique_ptr<TcpConfig> config;
        if (parseTcp(uri, config)) {
            outConfig = std::move(config);
            return true;
        }
    }
    else if (scheme == "tcp-server") {
        std::unique_ptr<TcpServerConfig> config;
        if (parseTcpServer(uri, config)) {
            outConfig = std::move(config);
            return true;
        }
    }
    else {
        std::cout << "[ProtocolUri] Error: Unknown scheme: " << scheme << std::endl;
    }
    
    return false;
}

} // namespace IOHub