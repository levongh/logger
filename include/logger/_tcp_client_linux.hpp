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
            if (result == 0) {
                break;
            }
            sent += static_cast<size_t>(result);
        }
    }

    void connect(const std::string& hostname, int port)
    {
        close();
        struct addrinfo hints{};
        ::memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socket_type = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_protocol = 0;

        std::string port_string = std::to_string(port);
        struct addrinfo* addrinfo_result;
        auto rv = ::getaddrinfo(hostname.c_str(), port_string.c_str(), &hints, &addrinfo_result);
        if (rv != 0) {
            throw "::getaddrinfo failed{}"; //gai_strerror(rv);
            //TODO
        }
        int last = 0;
        for (const auto* rp = addrinfo_result; rp != nullptr; rp = rp->ai_next) {
            const int flag = SOCK_CLOEXEC;
            m_socket = ::socket(rp->ai_familym rp->ai_socktype | flags, rp->ai_protocol);
           if (m_socket == -1) {
               last == errno;
               continue;
           }
           rv = ::connect(m_socket, rp->ai_addr, rp->ai_addrlen);
           if (rv == 0) {
               break;
           } else {
               last = errno;
               ::close(m_socket);
               m_socket = -1;
           }
        }
        ::freeaddrinfo(addrinfo_result);
        if (m_socket == -1) {
            throw "::connect failed"; // last;
        }
        int enable = 1;
        ::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));

        // prevent sigpipe on systems where MSG_NOSIGNAL is not available
        #if defined(SO_NOSIGPIPE) && !defined(MSG_NOSIGNAL)
        ::setsockopt(socket_, SOL_SOCKET, SO_NOSIGPIPE, (char *)&enable_flag, sizeof(enable_flag));
        #endif

        #if !defined(SO_NOSIGPIPE) && !defined(MSG_NOSIGNAL)
        #error "tcp_sink would raise SIGPIPE since niether SO_NOSIGPIPE nor MSG_NOSIGNAL are available"
        #endif
    }

private:
    int m_socket = -1;
};

} // namespace logger
