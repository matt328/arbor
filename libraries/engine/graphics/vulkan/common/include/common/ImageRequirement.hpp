#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "ImageCreateDescription.hpp"
#include "ImageUseDescription.hpp"

namespace arb {

/// `ImageRequirement` is how Image Aliases and their backing resources are requested from the
/// `AliasRegistry`. `IRenderPass`, or `FrameRenderer` itself will register these with the
/// `AliasRegistry`. The `AliasRegistry` will deduplicate requirements and create the vulkan
/// resources that back them. At command buffer recording time, passes will request the vulkan
/// resources backing them.
struct ImageRequirement {
  std::string alias;
  std::optional<ImageCreateDescription> createDesc = std::nullopt;
  ImageUseDescription useDesc;
};

}

template <>
struct std::formatter<arb::ImageRequirement> : std::formatter<std::string> {
  auto format(const arb::ImageRequirement& req, std::format_context& ctx) const {
    std::string createDescStr = req.createDesc ? std::format("{}", *req.createDesc) : "nullopt";

    return std::formatter<std::string>::format(
        std::format("ImageRequirement(alias=\"{}\", createDesc={}, useDesc={})",
                    req.alias,
                    createDescStr,
                    req.useDesc), // calls ImageUseDescription's formatter
        ctx);
  }
};
