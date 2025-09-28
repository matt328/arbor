#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

namespace arb {

struct BufferRequest2 {
  std::string name;
  size_t size;
  VkBufferUsageFlags usage;
  enum class Access : uint8_t {
    Read,
    Write,
    ReadWrite
  } access;
};

}
