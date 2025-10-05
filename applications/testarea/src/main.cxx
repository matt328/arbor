#include "Log.hpp"
#include <quill/LogMacros.h>
#include <windows.h>

auto main() -> int {

  // Works on Windows 10 (1607) and later
  HRESULT hr = SetThreadDescription(GetCurrentThread(), L"MainThread");
  if (FAILED(hr)) {
    // Optionally log or ignore
  }

  Log::init();

  Log::Application->set_log_level(quill::LogLevel::TraceL3);

  LOG_INFO(Log::Core, "This is a log info example {} \n With multiple lines.", 123);
  LOG_TRACE_L1(Log::Application, "This is a log info example {} \n With multiple lines.", 123);
}
