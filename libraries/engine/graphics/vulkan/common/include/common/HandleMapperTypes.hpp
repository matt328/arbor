#pragma once

#include "engine/common/HandleMapper.hpp"
#include "engine/common/data/Tags.hpp"
#include "engine/common/data/gpu/GeometryRegion.hpp"
#include "Texture.hpp"

namespace arb {

using GeometryHandleMapper = HandleMapper<Geometry, gpu::GeometryRegion>;
using TextureHandleMapper = HandleMapper<TextureTag, Texture>;

}
