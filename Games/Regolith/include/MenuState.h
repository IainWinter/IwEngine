#pragma once

#include "imgui/imgui.h"
#include "iw/engine/AppVars.h"
#include "Events.h"
#include <unordered_map>

// trying global app vars...

void SetMenuStateAppVars(iw::AppVars& app);

bool Button(const std::string& name);
bool ImageButton(const ImTextureID& id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, float frame, const ImVec4& bg, const ImVec4& tint);
bool Checkbox(const std::string& name, bool& active);
bool SliderFloat(const std::string& name, float& value, float min, float max, const char* fmt);

bool ConfirmButton(const std::string& name);

void PlayHoverOrClickSound(const std::string& name);
void PlayHoverSound(const std::string& name);
void PlayClickSound(const std::string& name);

void RegisterImage(const std::string& str);
void RegisterImage(const std::string& str, void* img);
void* Image(const std::string& str);
