#include "Log.hpp"

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/std/Array.h"

#include <array>

namespace Log {

void init() {
  quill::Backend::start();

  auto options =
      quill::PatternFormatterOptions{"%(time) [%(thread_name:<11)] %(short_source_location:<28) "
                                     "%(logger:<12) %(log_level:<9) %(message)",
                                     "%I:%M:%S",
                                     quill::Timezone::LocalTime};
  options.add_metadata_to_multi_line_logs = false;

  auto sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");

  Core = quill::Frontend::create_or_get_logger("core", sink, options);
  Application = quill::Frontend::create_or_get_logger("application", sink, options);
  Assets = quill::Frontend::create_or_get_logger("assets", sink, options);
  Renderer = quill::Frontend::create_or_get_logger("renderer", sink, options);
  Resources = quill::Frontend::create_or_get_logger("resources", sink, options);
  Gameplay = quill::Frontend::create_or_get_logger("gameplay", sink, options);
}

} // namespace Log
