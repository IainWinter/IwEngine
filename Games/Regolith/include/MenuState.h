#pragma once

#include "imgui/imgui.h"
#include "iw/engine/AppVars.h"
#include "Events.h"
#include <unordered_map>

// trying global app vars...

void SetMenuStateAppVars(iw::AppVars& app);

bool Button(const std::string& name);
bool Checkbox(const std::string& name, bool& active);
bool SliderFloat(const std::string& name, float& value, float min, float max, const char* fmt);

void PlayHoverOrClickSound(const std::string& name);
void PlayHoverSound(const std::string& name);
void PlayClickSound(const std::string& name);

void RegisterImage(const std::string& str);
void RegisterImage(const std::string& str, void* img);
void* Image(const std::string& str);
