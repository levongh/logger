#pragma once

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
