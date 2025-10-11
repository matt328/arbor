#pragma once

#include "bk/NonCopyMove.hpp"
#include "buffers/ResizeRequest.hpp"
#include "buffers/BufferHandle.hpp"
#include "buffers/BufferRequest.hpp"
#include "buffers/BufferRegion.hpp"
#include "buffers/GeometryData.hpp"

namespace arb {

class BufferSystem;

// Set these 1 byte larger than the test model so we can force a resize when loading 1 model
constexpr size_t IndexBufferInitialSize = 45937;
constexpr size_t PositionBufferInitialSize = 69649;
constexpr size_t TexCoordBufferInitialSize = 46433;
constexpr size_t ColorBufferInitialSize = 1024000;
constexpr size_t NormalBufferInitialSize = 1024000;
constexpr size_t AnimationBufferInitialSize = 1024000;

/// A Facade over all of the individual buffers that store geometry information on the GPU.
class GeometryStream : public NonCopyableMovable {
public:
  explicit GeometryStream(BufferSystem& newBufferSystem);
  ~GeometryStream();

  [[nodiscard]] auto getIndexBuffer() const -> const BufferHandle& {
    return indexBuffer;
  }

  [[nodiscard]] auto getPositionBuffer() const -> const BufferHandle& {
    return positionBuffer;
  }

  [[nodiscard]] auto getColorBuffer() const -> const BufferHandle& {
    return colorBuffer;
  }

  [[nodiscard]] auto getTexCoordBuffer() const -> const BufferHandle& {
    return texCoordBuffer;
  }

  [[nodiscard]] auto getNormalBuffer() const -> const BufferHandle& {
    return normalBuffer;
  }

  [[nodiscard]] auto getAnimationBuffer() const -> const BufferHandle& {
    return animationBuffer;
  }

  /// Checks to see if any buffers need resized to hold this data. Returns a vector of
  /// ResizeRequests containing each buffer that needs resized, and the buffer's new size.
  auto checkSizes(const GeometryData& data) -> std::vector<ResizeRequest>;

  auto allocate(BufferHandle handle, const BufferRequest& bufferRequest) -> BufferRegion;
  auto allocateIndexBuffer(const BufferRequest& bufferRequest) -> BufferRegion;
  auto allocatePositionBuffer(const BufferRequest& bufferRequest) -> BufferRegion;
  auto allocateColorBuffer(const BufferRequest& bufferRequest) -> BufferRegion;
  auto allocateTexCoordBuffer(const BufferRequest& bufferRequest) -> BufferRegion;
  auto allocateNormalBuffer(const BufferRequest& bufferRequest) -> BufferRegion;
  auto allocateAnimationBuffer(const BufferRequest& bufferRequest) -> BufferRegion;

private:
  BufferSystem& bufferSystem;

  BufferHandle indexBuffer;
  BufferHandle positionBuffer;
  BufferHandle colorBuffer;
  BufferHandle texCoordBuffer;
  BufferHandle normalBuffer;
  BufferHandle animationBuffer;
};

}
