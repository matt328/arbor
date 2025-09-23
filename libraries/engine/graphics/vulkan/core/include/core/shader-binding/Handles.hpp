#pragma once

#include "engine/common/Handle.hpp"

namespace arb {

struct DSLayoutTag {};
struct ShaderBindingTag {};

using DSLayoutHandle = Handle<DSLayoutTag>;
using ShaderBindingHandle = Handle<ShaderBindingTag>;

}
