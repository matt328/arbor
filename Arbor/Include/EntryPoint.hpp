#pragma once

#include <cstdint>
#include <memory>

#ifdef ARBOR_EXPORTS
#define ARBOR_API extern "C" __declspec(dllexport)
#else
#define ARBOR_API extern "C" __declspec(dllimport)
#endif

namespace Arbor {

class EngineContext;

ARBOR_API void initialize();

ARBOR_API void shutdown();

}
