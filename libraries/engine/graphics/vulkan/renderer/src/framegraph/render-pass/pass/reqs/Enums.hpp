#pragma once

#include <cstdint>

namespace arb {

enum class AliasAccess : uint8_t {
  Read = 0,
  Write,
  ReadWrite
};

enum class AliasUseType : uint8_t {
  Attachment = 0,
  Sampled,
  Storage
};

}
