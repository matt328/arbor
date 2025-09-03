#pragma once

namespace bk {

const std::string LOG_PATTERN = "%I:%M:%S [%-10n] %^%-8l%$ %v";

class LoggerManager {
public:
  // Initialize a logger for the current thread (or module)
  static void initLogger(const std::string &name) {
    // Only create if not already registered
    if (!spdlog::get(name)) {
      auto console_sink =
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      std::vector<spdlog::sink_ptr> sinks{console_sink};
      if (qt_sink()) {
        sinks.push_back(qt_sink());
      }

      auto logger =
          std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
      logger->set_pattern(LOG_PATTERN);
      logger->set_level(spdlog::level::trace);
      spdlog::register_logger(logger);
    }

    // Set the "current" logger pointer
    current_logger() = spdlog::get(name);
  }

  // Access the current logger conveniently
  static auto getLogger() -> std::shared_ptr<spdlog::logger> & {
    return current_logger();
  }

  // Optional: register Qt sink
  static void registerQtSink(spdlog::sink_ptr sink) {
    qt_sink() = sink;
    // Add it to all existing loggers
    spdlog::details::registry::instance().apply_all(
        [&](std::shared_ptr<spdlog::logger> logger) {
          auto &sinks = logger->sinks();
          sinks.push_back(sink); // or remove, depending on operation
        });
  }

  // Unregister Qt sink safely
  static void unregisterQtSink() {
    if (!qt_sink()) {
      return;
    }

    spdlog::details::registry::instance().apply_all(
        [&](std::shared_ptr<spdlog::logger> logger) {
          auto &sinks = logger->sinks();
          sinks.erase(std::remove(sinks.begin(), sinks.end(), qt_sink()),
                      sinks.end());
        });

    qt_sink().reset();
  }

private:
  static auto qt_sink() -> spdlog::sink_ptr & {
    static spdlog::sink_ptr s;
    return s;
  }

  static std::shared_ptr<spdlog::logger> &current_logger() {
    static thread_local std::shared_ptr<spdlog::logger> logger;
    return logger;
  }
};

// Macros to preserve old usage
#define InitLogger(name) bk::LoggerManager::initLogger(name)
#define Log bk::LoggerManager::getLogger()
} // namespace bk
