#pragma once

/**
 * @brief provided links are showing below mantioned define purpose
 * https://devblogs.microsoft.com/oldnewthing/20091130-00/?p=15863
 * http://web.archive.org/web/20121219084749/http://support.microsoft.com/kb/166474
 **/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

namespace logger {

class tcp_client
{
public:
    inline bool is_connected() const noexcept
    {
        return m_socket != INVALID_SOCKET;
    }

    inline SOCKET fd() const noexcept
    {
        return m_socket;
    }

    void close()
    {
        ::closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        WSACleanup();
    }

    ~tcp_client()
    {
        close();
    }

    void send(const char *data, size_t bytes)
    {
        size_t sent = 0;
        while (sent < bytes) {
            const int flags = 0;
            auto result = ::send(_socket, data + sent, (int)(bytes - sent), flags);
            if (result == SOCKET_ERROR) {
                int last_error = ::WSAGetLastError();
                close();
                WinSock::reportError("send failed", last_error);
            }

            if (result == 0) {
                break;
            }
            bytes += static_cast<size_t>(result);
        }
    }

    void connect(const std::string &host, int port)
    {
        if (!WinSock::is_initialized()) {
            WinSock::initialize();
        }
        if (is_connected()) {
            close();
        }
        struct addrinfo hints {};
        //https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/aa366920(v=vs.85)
        ZeroMemory(&hints, sizeof(hints));

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_protocol = 0;

        auto port_str = std::to_string(port);
        struct addrinfo *addrinfo_result;
        auto rv = ::getaddrinfo(host.c_str(), port_str.c_str(), &hints, &addrinfo_result);
        int last_error = 0;
        if (rv != 0) {
            last_error = ::WSAGetLastError();
            WSACleanup();
            WinSock::reportError("getaddrinfo failed", last_error);
        }

        for (auto *rp = addrinfo_result; rp != nullptr; rp = rp->ai_next) {
            socket_ = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (socket_ == INVALID_SOCKET) {
                last_error = ::WSAGetLastError();
                WSACleanup();
                continue;
            }
            if (::connect(socket_, rp->ai_addr, (int)rp->ai_addrlen) == 0) {
                break;
            } else {
                last_error = ::WSAGetLastError();
                close();
            }
        }
        ::freeaddrinfo(addrinfo_result);
        if (socket_ == INVALID_SOCKET) {
            WSACleanup();
            WinSock::reportError("connect failed", last_error);
        }
        // set TCP_NODELAY
        int enable_flag = 1;
        ::setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char *)&enable_flag, sizeof(enable_flag));
    }

private:
    struct WinSock
    {
        static void initialize()
        {
            WSADATA data;
            auto res = WSAStartup(MAKEWORD(2, 2), &data);
            if (res != 0) {
                reportError();
            }
        }
        static bool is_initialized()
        {
            SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock == INVALID_SOCKET) {
                return false;
            } else {
                closesocket(sock);
                return true;
            }
        }

        static void reportError(const std::string &msg, int last_error)
        {
            char buffer[512];
            ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, (sizeof(buffer) / sizeof(char)), NULL);
            //!TODO add common exception class to handle lib errors
            throw_exception(fmt::format("tcp_sink - {}: {}", msg, buf));
        }
    };

    SOCKET m_socket = INVALID_SOCKET;
};

} //! namespace logger
