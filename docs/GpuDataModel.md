# GPU Data Layout Overview

This document describes how **scene and geometry data** are organized in GPU buffers within the rendering system.  
It provides an overview of the **ObjectData**, **GeometryData**, and related buffer layouts used by the engine’s renderer.

## 🧩 Overview

The renderer separates GPU data into **two major families**:

| Category | Description | Lifetime |
|-----------|--------------|-----------|
| **Persistent Geometry Buffers** | Contain de-interleaved vertex and index data for all static meshes. | Long-lived (created once, reused) |
| **Transient Object Data Buffers** | Contain per-frame scene data (object transforms, materials, region lookups, etc.). | Rebuilt each frame |

This separation enables efficient **bindless rendering**, **streaming**, and **GPU-driven culling**.

## 📦 Object Data Buffers (`arb::gpu::ObjectData`)

These buffers describe **what to draw**, **how to transform it**, and **which geometry and materials** to use.  
They are transient and updated every frame by the visibility/scene system.

### `struct Object`

| Field | Type | Description |
|-------|------|-------------|
| `transformIndex` | `uint32_t` | Index into the `Transform` buffer. |
| `rotationIndex`  | `uint32_t` | Index into the `Rotation` buffer. |
| `scaleIndex`     | `uint32_t` | Index into the `Scale` buffer. |
| `geometryRegionId` | `uint32_t` | ID into the `GeometryRegion` buffer. |
| `materialId`     | `uint32_t` | Index into the `Material` buffer. |
| `animationId`    | `uint32_t` | Index into the `Animation` buffer (if animated). |

Each visible entity corresponds to one `Object` entry.  
During rendering, shaders can fetch associated data by indexing into the other buffers.

### `struct Transform`

Stores the world-space position of an object.

```cpp
struct alignas(4) Transform {
    glm::vec3 position;
};
```

### `struct Rotation`

Stores the orientation of an object as a quaternion.

```cpp
struct Rotation {
    glm::quat rotation;
};
```

### `struct Scale`

Stores the non-uniform scale of an object.

```cpp
struct alignas(4) Scale {
    glm::vec3 scale;
};
```

### `struct Animation`

Stores skeletal animation data (joint matrices) for skinned objects.

```cpp
struct Animation {
    glm::mat4 jointMatrices;
};
```

### `struct Material`

Defines surface appearance parameters.

| Field             | Type        | Description                               |
| ----------------- | ----------- | ----------------------------------------- |
| `baseColor`       | `glm::vec4` | Base albedo tint.                         |
| `albedoTextureId` | `uint32_t`  | Texture ID in the bindless texture array. |

## 🧱 Geometry Data Buffers (`arb::gpu::GeometryData`)

These buffers contain **persistent mesh geometry** stored in de-interleaved format.
Each mesh (or submesh) is represented by a `GeometryRegion` struct that provides offsets into the shared vertex buffers.

### `constexpr uint32_t INVALID_OFFSET`

```cpp
constexpr uint32_t INVALID_OFFSET = std::numeric_limits<uint32_t>::max(); // 0xFFFFFFFF
```

Used as a sentinel to indicate unused or invalid offsets.

### `struct GeometryRegion`

Defines where a particular mesh’s vertex attributes live within the persistent vertex buffers.

| Field            | Type       | Description                                       |
| ---------------- | ---------- | ------------------------------------------------- |
| `indexCount`     | `uint32_t` | Number of indices in the mesh.                    |
| `indexOffset`    | `uint32_t` | Offset into the global index buffer.              |
| `positionOffset` | `uint32_t` | Offset into the global vertex position buffer.    |
| `colorOffset`    | `uint32_t` | Offset into the global vertex color buffer.       |
| `texCoordOffset` | `uint32_t` | Offset into the global texture coordinate buffer. |
| `normalOffset`   | `uint32_t` | Offset into the global vertex normal buffer.      |

Each `GeometryRegion` represents a logical “mesh” within the global GPU geometry pools.

> 💡 The `GeometryRegion` buffer is **transient** (rebuilt per-frame or per-scene)
> even though it **indexes into persistent geometry buffers**.
> This allows for dynamic remapping of visible or streamed-in meshes.

### De-Interleaved Vertex Buffers

Each vertex attribute type is stored in its own tightly-packed buffer for GPU efficiency and bindless access.

| Struct               | Field(s)              | Description                    |
| -------------------- | --------------------- | ------------------------------ |
| `VertexPositionData` | `glm::vec3 position`  | Object-space vertex positions. |
| `VertexColorData`    | `glm::vec4 color`     | Per-vertex color (if any).     |
| `VertexTexCoordData` | `glm::vec2 texCoords` | Texture coordinates.           |
| `VertexNormalData`   | `glm::vec3 normal`    | Per-vertex normals.            |

The index buffer (`IndexData`) references these buffers via offsets defined in `GeometryRegion`.

### Example usage

```cpp
vec3 pos = positionBuffer[region.positionOffset + indexData.index];
vec3 nrm = normalBuffer[region.normalOffset + indexData.index];
vec2 uv  = texCoordBuffer[region.texCoordOffset + indexData.index];
```

## 🔁 Data Flow Summary

| System                              | Produces                                                       | Updates     | Lifetime   |
| ----------------------------------- | -------------------------------------------------------------- | ----------- | ---------- |
| **ResourceSystem / GeometryStream** | Vertex, index, and persistent geometry buffers                 | Rarely      | Persistent |
| **ObjectDataStream**                | Object, Transform, Material, Animation, GeometryRegion buffers | Every frame | Transient  |
| **FrameGraph / Renderer**           | Consumes all above for draw and dispatch                       | Every frame | N/A        |

## 🧭 Summary Diagram

```plaintext
                     ┌───────────────────────────────┐
                     │       GeometryStream          │
                     │  (persistent geometry data)   │
                     │                               │
                     │  VertexPositionData[]         │
                     │  VertexNormalData[]           │
                     │  VertexColorData[]            │
                     │  VertexTexCoordData[]         │
                     │  IndexData[]                  │
                     └──────────────┬────────────────┘
                                    │ references
                     ┌──────────────▼────────────────┐
                     │       ObjectDataStream        │
                     │   (transient scene data)      │
                     │                               │
                     │  Object[]                     │
                     │  Transform[]                  │
                     │  Rotation[]                   │
                     │  Scale[]                      │
                     │  Material[]                   │
                     │  Animation[]                  │
                     │  GeometryRegion[]             │
                     └──────────────┬────────────────┘
                                    │ consumed by
                     ┌──────────────▼────────────────┐
                     │         Renderer              │
                     │ (bindless + indirect draws)   │
                     └───────────────────────────────┘
```

## 🧠 Key Takeaways

* Geometry and scene data are **de-interleaved** into separate GPU buffers for cache efficiency.
* **Persistent geometry buffers** hold all static mesh data.
* **Transient object buffers** hold frame-specific scene data and mappings.
* `GeometryRegion` acts as the bridge between the two worlds.
* This layout supports **GPU-driven rendering**, **streaming**, and **LOD transitions** with minimal CPU-GPU synchronization.
