#pragma once

#include <string>
#include <unordered_map>

struct GameSetting
{
	union {
		bool  IsEnabled;
		float Value;
	};

	union {
		bool  Working_IsEnabled;
		float Working_Value;
	};

	GameSetting(
		bool enabled
	)
		: Value             (0.f)
		, Working_Value     (0.f)
		, IsEnabled         (enabled)
		, Working_IsEnabled (enabled)
	{}

	GameSetting(
		float value
	)
		: Value         (value)
		, Working_Value (value)
	{}

	bool HasChanged() const{ return Value != Working_Value; }
	void Reset() { Working_Value = Value; }

	virtual void Apply() {
		if (HasChanged()) {
			Value = Working_Value;
			ApplySetting();
		}
	}

	virtual void DrawSetting(const char* name) = 0;
	virtual void ApplySetting() = 0;
};

struct VSyncSetting : GameSetting
{
	VSyncSetting()
		: GameSetting(false)
	{}

	void ApplySetting() override
	{
		wglSwapIntervalEXT((int)IsEnabled);
	}

	void DrawSetting(
		const char* name) override
	{
		ImGui::Checkbox(name, &Working_IsEnabled);
	}
};

struct GameSettings
{
	std::unordered_map<std::string, GameSetting*> Settings;
	bool HasChanged;

	GameSettings()
	{
		HasChanged = false;
		Init();
	}

	void Draw()
	{
		HasChanged = false;
		for (auto& [name, setting] : Settings)
		{
			setting->DrawSetting(name.c_str());
			HasChanged |= setting->HasChanged();
		}
	}

	void Init()  { for (auto& [_, setting] : Settings) setting->ApplySetting(); }
	void Apply() { for (auto& [_, setting] : Settings) setting->Apply(); }
	void Reset() { for (auto& [_, setting] : Settings) setting->Reset(); }

	void Add(const std::string& name, GameSetting* setting)
	{
		Settings.emplace(name, setting);
	}
};
