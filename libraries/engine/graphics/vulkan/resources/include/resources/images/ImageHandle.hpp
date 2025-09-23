#pragma once

#include "common/Handles.hpp"
#include "engine/common/Handle.hpp"

namespace arb {

using ImageHandle = Handle<ImageTag>;
using LogicalImageHandle = LogicalHandle<ImageTag>;

using ImageViewHandle = Handle<ImageViewTag>;
using LogicalImageViewHandle = LogicalHandle<ImageViewTag>;

using SamplerHandle = Handle<SamplerTag>;

}
