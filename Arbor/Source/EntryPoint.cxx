#include "pch.h"

#include "EntryPoint.hpp"

#include <mutex>

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/std/Array.h"

#include "EngineContext.hpp"

namespace Arbor {

using EngineLogCallback = void (*)(int, const wchar_t*);

EngineLogCallback gCallback = nullptr;
std::mutex gMutex;

static std::unique_ptr<EngineContext> gEngineContext;
static quill::Logger* gLogger = nullptr;

class ManagedSink : public quill::Sink {
public:
  void write_log(quill::MacroMetadata const* log_metadata,
                 uint64_t /** log_timestamp **/,
                 std::string_view /** thread_id **/,
                 std::string_view /** thread_name **/,
                 std::string const& /** process_id **/,
                 std::string_view /** logger_name **/,
                 quill::LogLevel /** log_level **/,
                 std::string_view /** log_level_description **/,
                 std::string_view /** log_level_short_code **/,
                 std::vector<std::pair<std::string, std::string>> const*,
                 std::string_view,
                 std::string_view log_statement) override {

    std::wstring msg(reinterpret_cast<const wchar_t*>(log_statement.data()),
                     log_statement.size() / sizeof(wchar_t) // <-- use explicit length
    );
    std::lock_guard<std::mutex> lock(gMutex);
    if (gCallback) {
      gCallback(static_cast<int>(log_metadata->log_level()), msg.c_str());
    }
  }

  void flush_sink() noexcept override {
  }
};

ManagedSink* managedSink = nullptr;

void initialize() {
  if (gEngineContext == nullptr) {
    OutputDebugStringA("Initializing Engine\n");
    gEngineContext = std::make_unique<EngineContext>(EngineOptions{});
  }
}

void initializeLogging() {
  quill::BackendOptions backend_options;
  quill::Backend::start(backend_options);

  auto file_sink = quill::Frontend::create_or_get_sink<ManagedSink>("sink_id_1");
  gLogger = quill::Frontend::create_or_get_logger("root", std::move(file_sink));

  QUILL_LOG_INFO(gLogger, "Logging Initialized");
}

void registerLogCallback(void (*callback)(int, const wchar_t*)) {
  OutputDebugStringA("registering callback");
  std::lock_guard<std::mutex> lock(gMutex);
  gCallback = callback;
}

void shutdown() {
  if (gEngineContext != nullptr) {
    OutputDebugStringA("Shutting down Engine\n");
    gEngineContext.reset();
    gEngineContext = nullptr;
  }
}

}
