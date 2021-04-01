/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
*  Author: MrBaoquan
*  CreateTime: 2018-7-11 14:07
*  Email: mrma617@gmail.com
*/

// 若遇到 winsock 相关重定义 取消注释下面宏定义
#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <queue>
#include <mutex>

#pragma comment(lib,"WS2_32.lib")

enum TIType
{
    TCP = 10,
    UDP = 11,
};

struct MessageInfo
{
public:
    bool IsValidData()
    {
        return Content != "Invalid";
    }

public:
    std::string RemoteIP;
    unsigned short RemotePort;
    std::string Content;
private:
    MessageInfo() :RemoteIP("localhost")
        , RemotePort(0)
        , Content("Invalid") {}
    MessageInfo(std::string InRemoteIP, unsigned short InRemotePort, std::string InContent) :
        RemoteIP(InRemoteIP)
        , RemotePort(InRemotePort)
        , Content(InContent) {}

    friend class TIMessage;
    friend class TISocket;
};

class TIMessage
{
public:
    MessageInfo PopMessage()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (recvMessages.size() > 0)
        {
            MessageInfo msg = recvMessages.front();
            recvMessages.pop();
            return msg;
        }
        return MessageInfo();
    }

private:
    void AddMessage(MessageInfo info)
    {
        std::lock_guard<std::mutex> lk(mut);
        recvMessages.push(info);
    }

private:
    std::mutex mut;
    std::queue<MessageInfo> recvMessages;
    friend class TISocket;
};

class TISocket
{
public:
    TISocket() {}
    ~TISocket() {}

    /** 开始监听数据 */
    void StartReceived()
    {
        this->bExit = false;
        
        if (this->tcpipType == TCP)
        {
            listen(socketInstance, 10);
            queryConnectThread = std::move(std::thread(&TISocket::UpdateConnect, this));
        }
        queryMsgThread = std::move(std::thread(&TISocket::UpdateMessage, this));
    }

    /** 退出监听 */
    void Exit()
    {
        this->bExit = true;
        shutdown(socketInstance, SD_BOTH);
        closesocket(socketInstance);
        if (queryMsgThread.joinable())
        {
            queryMsgThread.join();
        }
        if (queryConnectThread.joinable())
        {
            queryConnectThread.join();
        }
    }

    /**
    * 初始化一个UDP服务器,并绑定一个远程连接
    */
    int Initialize(TIType InType, unsigned short localPort)
    {
        tcpipType = InType;
        WORD w = MAKEWORD(2, 0);
        WSAStartup(w, &data);
        if (tcpipType == UDP)
        {
            socketInstance = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        }else
        {
            socketInstance = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        }
        
        localAddr.sin_family = AF_INET;
        localAddr.sin_port = htons(localPort);
        localAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        return bind(socketInstance, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr));
    }

    /** 设置远程连接 */
    void SetRemoteAddr(std::string remoteIP, unsigned short remotePort)
    {
        remoteAddr.sin_family = AF_INET;
        remoteAddr.sin_port = htons(remotePort);
        std::wstring w_remoteIP(remoteIP.begin(), remoteIP.end());
        InetPton(AF_INET, w_remoteIP.c_str(), &remoteAddr.sin_addr.S_un.S_addr);
    }

    /** 向远程发送数据 */
    int Send(const char* sendBuf, int length = -1)
    {
        if (length == -1)
        {
            length = strlen(sendBuf);
        }
        int len = sizeof(SOCKADDR);
        SOCKET netSocket;
        if (this->tcpipType == UDP)
        {
            netSocket = socketInstance;
        }else
        {
            if (connections.size() > 0)
            {
                netSocket = connections[0].connectionSocket;
                SetRemoteAddr(connections[0].ip, connections[0].port);
            }else
            {
                return -1;
            }
        }
        return sendto(netSocket, sendBuf, length, 0, reinterpret_cast<sockaddr*>(&remoteAddr), len);
    }
    
    /** 监视客户端连接 */
    void UpdateConnect()
    {
        int len = sizeof(SOCKADDR);
        {
            sockaddr_in tempAddr;
            SOCKET connectSocket = accept(socketInstance, reinterpret_cast<sockaddr*>(&tempAddr), &len);
            if (connectSocket)
            {
                char ip[512];
                InetNtopA(tempAddr.sin_family, &tempAddr.sin_addr, ip, sizeof(ip));
                unsigned short port = ntohs(tempAddr.sin_port);
                TCPConnection connection;
                connection.connectionSocket = connectSocket;
                connection.ip = ip;
                connection.port = port;
                connections.push_back(connection);

            }
        }
    }

    /** 监视数据 */
    void UpdateMessage()
    {
        while (!this->bExit)
        {
            sockaddr_in addr2;
            int len = sizeof(SOCKADDR);
            switch (this->tcpipType)
            {
            case TCP:
                break;
            case UDP:
            {
                Sleep(35);
                ZeroMemory(buffer, sizeof(buffer));
                int a = recvfrom(socketInstance, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&addr2), &len);
                if (a>0)
                {
                    char ip[512];
                    InetNtopA(addr2.sin_family, &addr2.sin_addr, ip, sizeof(ip));
                    unsigned short port = ntohs(addr2.sin_port);
                    tiMessage.AddMessage(MessageInfo(ip, port, buffer));
                }else
                {
                    int val = WSAGetLastError();
                    OutputDebugStringA(std::to_string(val).data());
                    OutputDebugStringA(" SocketError \n");
                }
                break;
            }
            default:
                break;
            }
        }
    }

    /** 获取接收到的数据 */
    TIMessage& GetMessage()
    {
        return tiMessage;
    }

private:
    struct TCPConnection
    {
    public:
        SOCKET connectionSocket;
        std::string ip;
        unsigned short port;

    };
    TIType tcpipType = UDP;

    TIMessage tiMessage;

    WSADATA data;
    sockaddr_in remoteAddr, localAddr;
    SOCKET socketInstance;

    std::thread queryMsgThread;
    std::thread queryConnectThread;
    std::vector<TCPConnection> connections;

    char buffer[512];

    bool bExit = false;
};