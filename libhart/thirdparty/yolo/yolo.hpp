#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <vector>
#include <utility>

namespace yolo {

namespace detail {
    static std::vector<std::pair<std::string, std::string>> modules;

    template<typename... Args>
    std::string format(const std::string& format, Args... args)
    {
        std::ostringstream oss;
        size_t start = 0; size_t pos;
        ((oss << format.substr(start, (pos = format.find("{}", start)) - start) << args, start = pos + 2), ...);
        oss << format.substr(start);
        return oss.str();
    }

    std::string formatTime() 
    {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm local_time = *std::localtime(&time);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        std::ostringstream oss;
        oss << std::put_time(&local_time, "%H:%M:%S.") << std::setfill('0') << std::setw(3) << ms;
        return oss.str();
    }
}

template <typename... Args>
inline void __log(const std::string& level, const std::string& col, const std::string& message, Args... args)
{
    std::cout 
        << '[' << detail::formatTime() << "] "
        << '[' << col << level << "\033[0m" << "] " 
        << detail::format(message, args...) << std::endl;
}

template <typename... Args>
inline void __log(uint8_t module, const std::string& level, const std::string& col, const std::string& message, Args... args)
{
    auto mod = detail::modules[module];
    std::cout 
        << '[' << detail::formatTime() << "] "
        << '[' << col << level << "\033[0m" << "] "
        << '[' << mod.second << mod.first << "\033[0m" << "] "
        << detail::format(message, args...) << std::endl;
}


template <typename... Args>
inline void info(const std::string& format, Args&&... args)
{       
    __log("INFO", "\u001b[32;1m", format, args...);
}

template <typename... Args>
inline void warn(const std::string& format, Args&&... args)
{
    __log("WARN", "\u001b[33;1m", format, args...);
}

template <typename... Args>
inline void error(const std::string& format, Args&&... args)
{
    __log("ERROR", "\u001b[31;1m", format, args...);
}

template <typename... Args>
inline void debug(const std::string& format, Args&&... args)
{
    __log("DEBUG", "\u001b[34;1m", format, args...);
}


template <typename... Args>
inline void info(uint8_t module, const std::string& format, Args&&... args)
{       
    __log(module, "INFO", "\u001b[32;1m", format, args...);
}

template <typename... Args>
inline void warn(uint8_t module, const std::string& format, Args&&... args)
{
    __log(module, "WARN", "\u001b[33;1m", format, args...);
}

template <typename... Args>
inline void error(uint8_t module, const std::string& format, Args&&... args)
{
    __log(module, "ERROR", "\u001b[31;1m", format, args...);
}

template <typename... Args>
inline void debug(uint8_t module, const std::string& format, Args&&... args)
{
    __log(module, "DEBUG", "\u001b[34;1m", format, args...);
}


// Registers a module with a name and an optional ANSI colour code..
// this will return a reference that you can pass for future logs
// which will format the logger correctly to your name and colour
inline uint8_t registerModule(std::string name, std::string ANSI)
{
    detail::modules.push_back(std::make_pair(name, ANSI));
    return detail::modules.size() - 1; 
}

}

