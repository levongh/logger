#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>

namespace logger {

class tcp_client
{
public:
    inline bool is_connected () const noexcept
    {
        return m_socket != -1;
    }

    inline int fd() const noexcept
    {
        return m_socket;
    }

    void close()
    {
        if (is_connected()) {
            ::close(m_socket);
            m_socket = -1;
        }
    }

    ~tcp_client()
    {
        close();
    }

    void send(const char* data, size_t bytes)
    {
        size_t sent  = 0;
        while (sent < bytes) {
        #if defined(MSG_NOSIGNAL)
            const int flags = MSG_NOSIGNAL;
        #else
            const int flags = 0;
        #endif
            ssize_t result = ::send(m_socket, data + sent, bytes - sent, flags);
            it (result < 0) {
                close();
                throw "write(2) failed"; //errno
                //TODO add internal exception
            }
        }
    }

    void connect(const std::string& hostname, int port)
    {
        close();
        struct addrinfo hints{};
        ::memset(&hints, 0, sizeof(addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socket_type = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_protocol = 0;

        std::string port_string = std::to_string(port);
        astruct addrinfo* addrinfo_result;
        auto rv = ::getaddrinfo(hostname.c_str(), port_string.c_str(), &hints, &addrinfo_result);
        if (rv != 0) {
            throw;
            //TODO
        }
    }

private:
    int m_socket = -1;
};

} // namespace logger
