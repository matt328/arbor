#pragma once

#include <vulkan/vulkan_core.h>

#include "ResizeRequest.hpp"
#include "bk/NonCopyMove.hpp"
#include "engine/common/Handle.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "common/Handles.hpp"
#include "Buffer.hpp"
#include "resources/BufferCreateInfo.hpp"
#include "resources/BufferHandle.hpp"

namespace arb {

class AllocatorService;
struct Buffer;
class TransferSystem;

class BufferManager : public NonCopyableMovable {
public:
  BufferManager(AllocatorService& newAllocatorService);
  ~BufferManager();

  auto registerBuffer(const BufferCreateInfo& createInfo) -> BufferHandle;

  auto registerPerFrameBuffer(const BufferCreateInfo& createInfo) -> LogicalBufferHandle;

  auto processResizes(const std::vector<ResizeRequest>& resizeRequests,
                      TransferSystem& transferSystem) -> void;

  [[nodiscard]] auto resolve(LogicalBufferHandle logicalHandle) const -> BufferHandle;

  [[nodiscard]] auto getVkBuffer(BufferHandle handle) const -> VkBuffer;

private:
  AllocatorService& allocatorService;
  HandleGenerator<BufferTag> bufferHandleGenerator;

  std::unordered_map<Handle<BufferTag>, std::unique_ptr<Buffer>> bufferMap;
};

}
