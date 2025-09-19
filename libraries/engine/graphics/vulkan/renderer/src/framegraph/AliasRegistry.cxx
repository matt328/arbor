#include "AliasRegistry.hpp"

#include <algorithm>

namespace arb {

[[nodiscard]] auto AliasRegistry::getHandle(ImageAlias alias) const -> LogicalImageHandle {
  return handles[static_cast<size_t>(alias)];
}

[[nodiscard]] auto AliasRegistry::getHandle(BufferAlias alias) const -> LogicalBufferHandle {
  return bufferHandles[static_cast<size_t>(alias)];
}

[[nodiscard]] auto AliasRegistry::getHandle(GlobalBufferAlias alias) const -> BufferHandle {
  return globalBufferHandles[static_cast<size_t>(alias)];
}

auto AliasRegistry::setHandle(ImageAlias alias, LogicalImageHandle handle) -> void {
  handles[static_cast<size_t>(alias)] = handle;
}

auto AliasRegistry::setHandle(BufferAlias alias, LogicalBufferHandle handle) -> void {
  bufferHandles[static_cast<size_t>(alias)] = handle;
}

auto AliasRegistry::setHandle(GlobalBufferAlias alias, BufferHandle handle) -> void {
  globalBufferHandles[static_cast<size_t>(alias)] = handle;
}

auto AliasRegistry::reset() -> void {
  std::ranges::fill(handles, LogicalImageHandle{});
}

}
