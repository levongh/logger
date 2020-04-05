#pragma once

/**
 * @author Levon Ghukasyan
 * @file common.h
 * @brief this file contains common interface
 */

#include <memory>
#include <string>
#include <initializer_list>
#include <functional>
#include <type_traits>
#include <exception>
#include <chrono>

#if defined(_WIN32) && defined(SHARED_LIB)
    #ifdef LOG_EXPORT
        #define LOG_API __declspec(dllexport)
    #else
        #define LOG_API __declspec(dllimport)
    #endif
#else
    #define LOG_API
#endif

namespace details {

/**
 *
 * _______________________________
 * |   Meaning of __cplusplus    |
 * -------------------------------
 * | __cplusplus |	C++ Standard |
 * -------------------------------
 * |    199711L  |  	C++98    |
 * |    201103L  |  	C++11    |
 * |    201402L  |  	C++14    |
 * |    201703L  |  	C++17    |
 * -------------------------------
 *
 * detailed information you can find by follogin link: https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique
 **/

#if __cplusplus >= 201402L
    using std::make_unique;
#else
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        static_assert(!std::is_array<T>::value, "array is not supported");
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#endif

} //!namespace details
