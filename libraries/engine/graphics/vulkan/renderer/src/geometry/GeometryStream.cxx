#include "GeometryStream.hpp"

#include <glm/glm.hpp>

#include "bk/Logger.hpp"
#include "resources/ResourceFacade.hpp"

namespace arb {

GeometryStream::GeometryStream(ResourceFacade& newResourceFacade)
    : resourceFacade{newResourceFacade},
      indexBuffer{resourceFacade.createBuffer(
          BufferCreateInfo{.allocationStrategy = AllocationStrategy::Arena,
                           .bufferLifetime = BufferLifetime::Persistent,
                           .initialSize = IndexBufferInitialSize,
                           .itemStride = sizeof(uint32_t),
                           .debugName = "Buffer-GeometryIndex"})},
      positionBuffer{resourceFacade.createBuffer(
          BufferCreateInfo{.allocationStrategy = AllocationStrategy::Arena,
                           .bufferLifetime = BufferLifetime::Persistent,
                           .initialSize = PositionBufferInitialSize,
                           .itemStride = sizeof(glm::vec3),
                           .debugName = "Buffer-GeometryPosition"})},
      colorBuffer{resourceFacade.createBuffer(
          BufferCreateInfo{.allocationStrategy = AllocationStrategy::Arena,
                           .bufferLifetime = BufferLifetime::Persistent,
                           .initialSize = ColorBufferInitialSize,
                           .itemStride = sizeof(glm::vec4),
                           .debugName = "Buffer-GeometryColors"})},
      texCoordBuffer{resourceFacade.createBuffer(
          BufferCreateInfo{.allocationStrategy = AllocationStrategy::Arena,
                           .bufferLifetime = BufferLifetime::Persistent,
                           .initialSize = TexCoordBufferInitialSize,
                           .itemStride = sizeof(glm::vec2),
                           .debugName = "Buffer-GeometryTexCoords"})},
      normalBuffer{resourceFacade.createBuffer(
          BufferCreateInfo{.allocationStrategy = AllocationStrategy::Arena,
                           .bufferLifetime = BufferLifetime::Persistent,
                           .initialSize = NormalBufferInitialSize,
                           .itemStride = sizeof(glm::vec3),
                           .debugName = "Buffer-GeometryNormal"})},
      animationBuffer{resourceFacade.createBuffer(
          BufferCreateInfo{.allocationStrategy = AllocationStrategy::Arena,
                           .bufferLifetime = BufferLifetime::Persistent,
                           .initialSize = AnimationBufferInitialSize,
                           .itemStride = sizeof(glm::vec4),
                           .debugName = "Buffer-AnimationData"})} {
  Log::trace("Constructing Geometry Stream");
}

GeometryStream::~GeometryStream() {
  Log::trace("Destroying Geometry Stream");
}

auto GeometryStream::checkSizes(const GeometryData& data) -> std::vector<ResizeRequest> {
  auto resizes = std::vector<ResizeRequest>{};

  auto check = [&](BufferHandle handle, size_t size) {
    if (size == 0) {
      return;
    }
    if (auto maybe = resourceFacade.checkBufferSize(handle, size)) {
      resizes.push_back(*maybe);
    }
  };

  check(indexBuffer, data.indexData->size());
  check(positionBuffer, data.positionData->size());
  check(colorBuffer, data.colorData ? data.colorData->size() : 0L);
  check(texCoordBuffer, data.texCoordData ? data.texCoordData->size() : 0L);
  check(normalBuffer, data.normalData ? data.normalData->size() : 0L);
  check(animationBuffer, data.animationData ? data.animationData->size() : 0L);

  return resizes;
}

auto GeometryStream::allocate(BufferHandle handle, const BufferRequest& bufferRequest)
    -> BufferRegion {
  return resourceFacade.allocateInBuffer(handle, bufferRequest.size);
};

/// Allocates space in the existing index buffer
auto GeometryStream::allocateIndexBuffer(const BufferRequest& bufferRequest) -> BufferRegion {
  return resourceFacade.allocateInBuffer(indexBuffer, bufferRequest.size);
}

auto GeometryStream::allocatePositionBuffer(const BufferRequest& bufferRequest) -> BufferRegion {
  return resourceFacade.allocateInBuffer(positionBuffer, bufferRequest.size);
}

auto GeometryStream::allocateColorBuffer(const BufferRequest& bufferRequest) -> BufferRegion {
  return resourceFacade.allocateInBuffer(colorBuffer, bufferRequest.size);
}

auto GeometryStream::allocateTexCoordBuffer(const BufferRequest& bufferRequest) -> BufferRegion {
  return resourceFacade.allocateInBuffer(texCoordBuffer, bufferRequest.size);
}

auto GeometryStream::allocateNormalBuffer(const BufferRequest& bufferRequest) -> BufferRegion {
  return resourceFacade.allocateInBuffer(normalBuffer, bufferRequest.size);
}

auto GeometryStream::allocateAnimationBuffer(const BufferRequest& bufferRequest) -> BufferRegion {
  return resourceFacade.allocateInBuffer(animationBuffer, bufferRequest.size);
}

}
