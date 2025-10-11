#pragma once

#include "common/Handles.hpp"
#include "engine/common/HandleMapper.hpp"
#include "engine/common/data/Tags.hpp"
#include "engine/common/data/gpu/GeometryRegion.hpp"
#include "Texture.hpp"

namespace arb {

using GeometryHandleMapper = HandleMapper<Geometry, GeometryRegionTag>;
using TextureHandleMapper = HandleMapper<TextureTag, Texture>;

}
