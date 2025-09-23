#pragma once

#include "bk/NonCopyMove.hpp"
#include "bk/Logger.hpp"
#include "core/images/ImageView.hpp"
#include "images/Sampler.hpp"

#include <memory>

namespace arb {

class Texture : public NonCopyableMovable {
public:
  Texture(std::shared_ptr<ImageView> newImageView,
          std::shared_ptr<Sampler> newSampler,
          const std::optional<std::string>& name = std::nullopt)
      : imageView{std::move(newImageView)},
        sampler{std::move(newSampler)},
        debugName{name.value_or("Unnamed Texture")} {
    Log::trace("Creating Texture {}", debugName);
  }
  ~Texture() {
    Log::trace("Destroying Texture {}", debugName);
  }

  [[nodiscard]] auto vkView() const {
    return imageView;
  }

  [[nodiscard]] auto vkSampler() const {
    return sampler;
  };

private:
  std::shared_ptr<ImageView> imageView;
  std::shared_ptr<Sampler> sampler;
  std::string debugName;
};

}
