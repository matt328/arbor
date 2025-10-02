#include "bk/Logger.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace bk {

auto LoggerManager::logStringTrace(std::string message) -> void {
  getLogger()->trace(message);
}

auto LoggerManager::logStringDebug(std::string message) -> void {
  const auto id = std::this_thread::get_id();
  getLogger()->debug(std::format("id={}: {}", id, message));
}

auto LoggerManager::logStringInfo(std::string message) -> void {
  getLogger()->info(message);
}

auto LoggerManager::logStringWarn(std::string message) -> void {
  getLogger()->warn(message);
}

auto LoggerManager::logStringError(std::string message) -> void {
  getLogger()->error(message);
}

void LoggerManager::initLogger(const std::string& name) {
  // Only create if not already registered
  if (!spdlog::get(name)) {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::vector<spdlog::sink_ptr> sinks{console_sink};
    // if (qt_sink()) {
    //   sinks.push_back(qt_sink());
    // }

    auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    logger->set_pattern(LOG_PATTERN);
    logger->set_level(spdlog::level::trace);
    spdlog::register_logger(logger);
  }

  // Set the "current" logger pointer
  current_logger() = spdlog::get(name);
}

}
