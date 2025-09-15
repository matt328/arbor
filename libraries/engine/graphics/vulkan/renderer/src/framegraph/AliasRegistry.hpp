#pragma once

#include <array>

#include "bk/NonCopyMove.hpp"
#include "ImageAlias.hpp"
#include "ResourceAliases.hpp"
#include "resources/BufferHandle.hpp"
#include "resources/ImageHandle.hpp"

namespace arb {

/// Contains mappings of resource Aliases used in the FrameGraph to their (Logical)Handles.
class AliasRegistry : public NonCopyableMovable {
public:
  AliasRegistry() = default;
  ~AliasRegistry() = default;

  [[nodiscard]] auto getHandle(ImageAlias alias) const -> LogicalImageHandle;
  [[nodiscard]] auto getHandle(BufferAlias alias) const -> LogicalBufferHandle;
  [[nodiscard]] auto getHandle(GlobalBufferAlias alias) const -> BufferHandle;

  auto setHandle(ImageAlias alias, LogicalImageHandle handle) -> void;
  auto setHandle(BufferAlias alias, LogicalBufferHandle handle) -> void;
  auto setHandle(GlobalBufferAlias alias, BufferHandle handle) -> void;
  auto reset() -> void;

private:
  std::array<LogicalImageHandle, static_cast<size_t>(ImageAlias::Count)> handles;
  std::array<LogicalBufferHandle, static_cast<size_t>(BufferAlias::Count)> bufferHandles;
  std::array<BufferHandle, static_cast<size_t>(GlobalBufferAlias::Count)> globalBufferHandles;
};

}
