# Vulkan Backend Design

```mermaid

flowchart TB

  subgraph Core
    Device["Device
    PhysicalDevice
    Swapchain
    Queues"]
  end

  subgraph Resources
    BufferSystem["BufferSystem
    GPU buffer mgmt"]
    ImageSystem["ImageSystem
    GPU images/views"]
  end

  subgraph Assets
    UploadSystem["UploadSystem
    schedules data uploads"]
    TransferSystem["TransferSystem
    staging + copies"]
    Mappers["HandleMappers
    Geometry/Texture"]
  end

  subgraph Renderer
    FrameManager["FrameManager
    per-frame state"]
    CommandBufferMgr["CommandBufferManager
    alloc/record"]
  end

  %% Dependencies
  Renderer --> Resources
  Renderer --> Core
  Renderer --> Common

  Assets --> Resources
  Assets --> Core

  Assets --> Renderer

  Resources --> Common

  %% Styling for clarity
  style Core fill:#1b1f3b,stroke:#333,stroke-width:2px,color:#fff
  style Resources fill:#283655,stroke:#333,stroke-width:2px,color:#fff
  style Assets fill:#4d648d,stroke:#333,stroke-width:2px,color:#fff
  style Renderer fill:#d0e1f9,stroke:#333,stroke-width:2px,color:#000
