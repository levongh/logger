#pragma once

#include <ctime>
#include <common.h>

namespace logger {
namespace os {

LOG_API std::chrono::system_clock::time_point now() noexcept;
LOG_API std::tm localtime(const std::time_t &time) noexcept;
LOG_API std::tm localtime() noexcept;
LOG_API std::tm gmtime(const std::time_t &time) noexcept;
LOG_API std::tm gmtime() noexcept;

//!@brief end of line
#ifdef _WIN32
#define LOG_EOL "\r\n"
#else
#define LOG_EOL "\n"
#endif

constexpr static const char* default_eol = LOG_EOL;
#ifdef _WIN32
static const char folder_sep = '\\';
#else
constexpr static const char folder_sep = '/';
#endif

}
}
