#pragma once

#include <string>
#include <thread>
#include <format>

namespace spdlog {
class logger;
}

namespace bk {

const std::string LOG_PATTERN = "%I:%M:%S [%-10n] %^%-8l%$ %v";

class LoggerManager {
public:
  // Initialize a logger for the current thread (or module)
  static void initLogger(const std::string& name);

  static auto logStringTrace(std::string message) -> void;
  static auto logStringDebug(std::string message) -> void;
  static auto logStringInfo(std::string message) -> void;
  static auto logStringWarn(std::string message) -> void;
  static auto logStringError(std::string message) -> void;

  template <typename... Args>
  static auto trace(const std::string& fmt, Args&&... args) noexcept -> void {
    const std::string message = std::vformat(fmt, std::make_format_args(args...));
    logStringTrace(message);
  }

  template <typename... Args>
  static auto debug(const std::string& fmt, Args&&... args) -> void {
    const std::string message = std::vformat(fmt, std::make_format_args(args...));
    logStringDebug(message);
  }

  template <typename... Args>
  static auto info(const std::string& fmt, Args&&... args) -> void {
    const std::string message = std::vformat(fmt, std::make_format_args(args...));
    logStringInfo(message);
  }

  template <typename... Args>
  static auto warn(const std::string& fmt, Args&&... args) -> void {
    const std::string message = std::vformat(fmt, std::make_format_args(args...));
    logStringWarn(message);
  }
  template <typename... Args>
  static auto error(const std::string& fmt, Args&&... args) -> void {
    const std::string message = std::vformat(fmt, std::make_format_args(args...));
    logStringError(message);
  }

  // Access the current logger conveniently
  static auto getLogger() -> std::shared_ptr<spdlog::logger>& {
    return current_logger();
  }

private:
  static auto current_logger() -> std::shared_ptr<spdlog::logger>& {
    static std::shared_ptr<spdlog::logger> logger;
    const auto id = std::this_thread::get_id();
    return logger;
  }
};

// Macros to preserve old usage
#define InitLogger(name) bk::LoggerManager::initLogger(name)
#define Log bk::LoggerManager
} // namespace bk
