# Buffer Mess Untangling

Systems are facades/apis into a group of related components.
Systems also orchestrate and wire together component.
Components under systems should stick to single responsibility principle.

## Data Flows

### BufferSystem

Creating a Peristent Buffer

- request a handle
- create a BufferEntry
- create a buffer with the Allocator and place it in versions.front
- emplace BufferEntry in map
- emplace an IBufferAllocator in the allocatorMap

Creating a PerFrame Buffer

- create and map a logicalHandle to physical handles
- call createPersistentBuffer once per frame, recording logical->physical mapping

Allocating a BufferRegion

- lookup and defer to IBufferAllocator from map

Resizing a Transient Buffer

- not needed yet

Resizing a Persistent buffer

- call checksize on all affected buffers -> ResizeRequest
- collect and submit a batch of ResizeRequests
- for each ResizeRequest
  - copy over BufferCreateInfo
  - update size
  - bump version number in name
  - create new buffer
  - submit copy data to transfer queue synchronously
  - track each ResizeJob
- for each ResizeJob
  - set new buffer's validFromFrame to current
  - set old buffer's validToFrame to current
  - add new buffer to entry's versions list
  - update entry's currentsize
  - update Buffer's bufferMeta::size
  - if there is an allocator, notify it that buffer has resized
- periodically call pruneOldVersions:
  - if a buffer version's validTo is older than 3 frames, remove it from the entry.versions

Getting the 'current' buffer

- Transient buffers just return the versions.front()
- iterates the versions in reverse, return the first one whose validFrom is before the current frame, and it's validTo is after the current frame

### UploadSystem

Persistent buffer upload

- UploadBatchSystem recieves endBatch event
- loads models and analyzes staging buffers and size needs
- partitions into subbatches based on staging buffer size
- creates an uploadsubbatch
  - checks sizes and optionally resizes destination buffers if needed
  - allocates geometryAllocation in the geometry buffer system
  - creates destination images
  - allocates image space in the staging buffer
  - adds ImageUpload to the subbatch
- submits uploadsubbatches to transfersystem
- emits Responses to caller, keyed by requestId and batchId

## Buffer

- light wrapper around VkBuffer
- immutable

## BufferSystem Details

- registers buffers, exchanges `BufferCreateInfo` for `[Logical]BufferHandle`
- inserts data into PerFrame host visible buffers synchronously
- checkSize()/processResizes() asynchronously
- virtually allocate space in a buffer
- getBuffer(handle) -> Buffer&
- getVkBuffer(handle) -> VkBuffer
- clients never cache these, assume they'll be invalidated after the current frame
- Dependencies:
  - AllocatorService
- Components:
  - IBufferAllocator - stateful allocator, mapped from BufferHandle

## UploadBatchSystem Details

- system itself listens to resource batch events, buffers them until an EndBatch event then processes
- partitions uploads into SubBatches sized to match the staging buffer
- augments SubBatches into UploadSubBatches by performing virtual allocations into all buffers and storing
allocation information in UploadSubBatch
- passes UploadSubBatches to TransferSystem to be synchronously uploaded via a dedicated transfer queue

### Components

- VirtualBufferAllocator
  - depends on GeometryStream (name tbd)
  - depends on transfer context's staging allocator
  - allocate(const GeometryData& data) -> GeometryAllocation
- GeometryAllocation
  - struct containing everything needed to upload a coherent mesh into the Geometry Data Buffer
