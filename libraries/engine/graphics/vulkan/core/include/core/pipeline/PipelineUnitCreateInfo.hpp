#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace arb {

enum class PipelineType : uint8_t {
  Compute = 0u,
  Graphics
};

struct PushConstantInfo {
  VkShaderStageFlags stageFlags;
  uint32_t offset;
  uint32_t size;
};

struct PipelineLayoutInfo {
  std::vector<PushConstantInfo> pushConstantInfoList;
  std::span<VkDescriptorSetLayout> descriptorSetLayouts;
  std::optional<std::string> debugName = std::nullopt;
};

struct ShaderStageInfo {
  VkShaderStageFlagBits stage;
  std::string shaderFile;
  std::string entryPoint;
  std::optional<std::string> debugName = std::nullopt;
};

struct PipelineCreateInfo {
  PipelineType pipelineType;
  PipelineLayoutInfo pipelineLayoutInfo;
  std::vector<VkFormat> colorAttachmentFormats;
  std::optional<VkFormat> depthAttachmentFormat = std::nullopt;
  std::vector<ShaderStageInfo> shaderStageInfo;

  VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
  bool enableDepthTest = true;
  bool enableDepthWrite = true;
  std::optional<std::string> debugName;
};

}
