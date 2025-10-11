#pragma once

#include <memory>

#include "BufferSystem.hpp"
#include "bk/IEventQueue.hpp"
#include "bk/NonCopyMove.hpp"

namespace arb {

struct StaticMeshRequest;
struct StaticModelRequest;
struct DynamicModelRequest;

struct UploadBatchSystemDeps {
  BufferSystem& bufferSystem;
  std::shared_ptr<bk::IEventQueue> eventQueue;
};

/// This system doesn't have a public api, rather it just subscribes to events, batches and
/// processes the payloads and produces data that is consumed by the TransferSystem to upload into
/// buffers using an async transfer queue. It produces units of work--buffer resizes, image
/// creations, and uploads, and submits them to the transfer system in that order.
class UploadBatchSystem : public NonCopyableMovable {
public:
  explicit UploadBatchSystem(const UploadBatchSystemDeps& deps);
  ~UploadBatchSystem();

private:
  BufferSystem& bufferSystem;
  std::shared_ptr<bk::IEventQueue> eventQueue;

  void beginResourceBatch(uint64_t batchId);
  void handleStaticMeshRequest(const std::shared_ptr<StaticMeshRequest>& req);
  void handleStaticModelRequest(const std::shared_ptr<StaticModelRequest>& req);
  void handleDynamicModelRequest(const std::shared_ptr<DynamicModelRequest>& req);
  void processBatchedResources(uint64_t batchId);
};

}
