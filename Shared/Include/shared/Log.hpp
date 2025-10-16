#pragma once

#include <quill/Logger.h>
#include <quill/LogMacros.h>

namespace Log {

inline quill::Logger* Application;
inline quill::Logger* Core;
inline quill::Logger* Assets;
inline quill::Logger* Renderer;
inline quill::Logger* Resources;
inline quill::Logger* Gameplay;

void init();

}
