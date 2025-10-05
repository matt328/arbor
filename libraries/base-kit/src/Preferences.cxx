#include "bk/Preferences.hpp"

#include "bk/Log.hpp"

#include <fstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>

namespace bk {

Preferences::Preferences(std::filesystem::path newPath) : path{std::move(newPath)} {
  if (!exists(path.parent_path())) {
    create_directories(path.parent_path());
  }

  // Create the file if it doesn't exist
  if (!exists(path)) {
    if (std::ofstream o{path, std::ios::binary}; o.is_open()) {
      cereal::BinaryOutputArchive output(o);
      output(preferencesData);
    } else {
      LOG_WARNING(Log::Core, "Failed to create preferences file");
    }
  }

  try {
    if (std::ifstream i{path, std::ios::binary}; i.is_open()) {
      cereal::BinaryInputArchive input(i);
      input(preferencesData);
      loaded = true;
    } else {
      LOG_ERROR(Log::Core, "Error reading application preferences from: {0}", path.string());
    }
  } catch (const std::exception& ex) {
    LOG_WARNING(Log::Core,
                "Error reading application preferences from: {0}, {1}",
                path.string(),
                ex.what());
  }
  LOG_TRACE_L1(Log::Core, "Loaded preferences");
}

auto Preferences::put(const std::string& key, const std::string& value) -> void {
  auto lock = std::lock_guard(mtx);
  preferencesData.stringMap[key] = value;
  save();
}

void Preferences::save() const {
  if (std::ofstream o{path, std::ios::binary}; o.is_open()) {
    cereal::BinaryOutputArchive output{o};
    output(preferencesData);
  } else {
    LOG_INFO(Log::Core, "Error saving application preferences to file: {0}", path.string());
  }
}

}
