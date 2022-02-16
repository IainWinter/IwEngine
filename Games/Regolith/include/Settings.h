#pragma once

#include "MenuState.h"

#include <string>
#include <unordered_map>

struct GameSetting
{
	union {
		float Value;
		bool  IsEnabled;
	};

	union {
		float Working_Value;
		bool  Working_IsEnabled;
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
	VSyncSetting() : GameSetting(true) {}
	void ApplySetting()                override { wglSwapIntervalEXT((int)IsEnabled); }
	void DrawSetting(const char* name) override { Checkbox(name, Working_IsEnabled); }
};

struct AudioSetting : GameSetting
{
	iw::ref<iw::IAudioSpace> Audio;
	int AudioHandle;

	AudioSetting(
		iw::ref<iw::IAudioSpace> audio,
		int audioHandle
	)
		: GameSetting (50.f)
		, Audio       (audio)
		, AudioHandle (audioHandle)
	{}

	void ApplySetting()                override { Audio->SetVolume(AudioHandle, Value / 100.f); }
	void DrawSetting(const char* name) override { SliderFloat(name, Working_Value, 0.f, 100.f, "%.0f"); }
};

struct DisplaySetting : GameSetting
{
	iw::IWindow* Window;
	const char* Options[3] = {"Windowed", "Borderless", "Fullscreen"};

	DisplaySetting(
		iw::IWindow* window
	)
		: GameSetting (false)
		, Window      (window)
	{}

	void ApplySetting() override
	{
		int selected = IsEnabled ? 1 : 0;
		iw::DisplayState state;

		switch (selected)
		{
			case 0: state = iw::DisplayState::NORMAL;     break;
			case 1: state = iw::DisplayState::BORDERLESS; break;
			//case 2: state = iw::DisplayState::FULLSCREEN; break;
		}

		Window->SetState(state);
	}

	void DrawSetting(const char* name) override
	{
		Checkbox(name, Working_IsEnabled);

		//int selected = (int)Working_Value;
		//if (ImGui::BeginCombo(name, Options[selected]))
		//{
		//	for (int i = 0; i < 3; i++)
		//	{
		//		if (ImGui::Selectable(Options[i]))
		//		{
		//			Working_Value = (float)i;
		//		}
		//	}
		//	ImGui::EndCombo();
		//}
	}
};

// dosnt work, I think that this might need to be in a layer
// as it needs the last inputed buton. getting this through an event would be much more simple
// could still have a setting, maybe it reads from a shared array
struct ButtonMapSetting : GameSetting
{
	std::string Command;
	iw::ref<iw::Context> Context;
	iw::ref<iw::Context> LastContext;
	iw::ref<iw::InputManager> Input;

	bool Edit;

	ButtonMapSetting(
		iw::ref<iw::InputManager> input,
		iw::ref<iw::Context> context,
		const std::string& command,
		iw::InputName name
	)
		: GameSetting ((float)name)
		, Input       (input)
		, Context     (context)
		, LastContext (nullptr)
		, Command     (command)
		, Edit        (false)
	{}

	void ApplySetting() override
	{
		Context->MapButton((iw::InputName)Value, Command);
	}

	void DrawSetting(const char* name) override
	{
		//if (Edit && !LastContext)
		//{
		//	LastContext = Input->GetContext();
		//	Input->SetContext(Context);
		//	Context->LastInput = iw::InputName::INPUT_NONE;
		//}

		//if (Context->LastInput != iw::InputName::INPUT_NONE)
		//{
		//	//Input->SetContext(LastContext);
		//	//LastContext = nullptr;
			Working_Value = (float)Context->LastInput;
		//}

		iw::InputName inputName = (iw::InputName)Working_Value;
		char c = iw::GetCharacter(inputName);

		if (c == '\0')
		{
			Reset();
			return;
		}

		const char* bname = nullptr;

		switch (c)
		{
			case ' ': bname = "SPACE"; break;
			default:  bname = &c;      break;
		}

		ImGui::Text(name);
		ImGui::SameLine();
		if (ImGui::Button(bname))
		{
			Edit = true;
		}
	}
};

struct GameSettings
{
	std::unordered_map<std::string, GameSetting*> Settings;
	bool HasChanged;

	GameSettings()
	{
		HasChanged = false;
	}

	void Draw(int x, int y, int width)
	{
		HasChanged = false;
		ImGui::SetCursorPosY(y);
		for (auto& [name, setting] : Settings)
		{
			ImGui::SetCursorPosX(x);
			ImGui::SetNextItemWidth(width);
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
