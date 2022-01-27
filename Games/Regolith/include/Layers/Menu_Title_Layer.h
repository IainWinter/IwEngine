#include "Layers/Menu_Layer.h"
#include "Layers/Menu_Highscores_Layer.h"

#include "gl/glew.h"
#include "gl/wglew.h"

#include <functional>

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
		: Value(0.f)
		, Working_Value(0.f)
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
	VSyncSetting() : GameSetting(false) {}
	void ApplySetting()                override { wglSwapIntervalEXT((int)IsEnabled); }
	void DrawSetting(const char* name) override { ImGui::Checkbox(name, &Working_IsEnabled); }
};

struct AudioSetting : GameSetting
{
	iw::ref<iw::IAudioSpace> Audio;
	int AudioHandle;

	AudioSetting(
		iw::ref<iw::IAudioSpace> audio,
		int audioHandle
	)
		: GameSetting (0.f)
		, Audio       (audio)
		, AudioHandle (audioHandle)
	{}

	void ApplySetting()                override { Audio->SetVolume(AudioHandle, Value / 100.f); }
	void DrawSetting(const char* name) override { ImGui::SliderFloat(name, &Working_Value, 0.f, 100.f, "%.0f"); }
};

struct GameSettings
{
	std::unordered_map<std::string, GameSetting*> Settings;
	bool HasChanged;

	GameSettings()
	{
		HasChanged = false;
		Settings.emplace("VSync", new VSyncSetting());
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
};

enum class MenuTarget
{
	DEFAULT,
	HIGHSCORES,
	SETTINGS,
	GAME
};

struct Menu_Title_Layer : Menu_Layer2
{
	iw::Entity ball;
	iw::Entity axis;
	iw::Entity title;
	iw::Entity title_hs;
	iw::Entity title_st;
	iw::Entity stars;
	iw::Entity smoke;
	iw::ref<iw::RenderTarget> bg;
	iw::PerspectiveCamera persp;
	iw::OrthographicCamera ortho;
	float t;
	float t1;

	glm::vec3 target_pos;
	glm::quat target_rot;
	glm::vec3 last_pos;
	glm::quat last_rot;

	int target_menu;
	int last_menu;

	Highscores_MenuParts highscoreParts;

	GameSettings GameSettings;
	MenuTarget BackButtonTarget = MenuTarget::DEFAULT; // config for buttons
	std::function<void()> BackButtonFunc;

	Menu_Title_Layer()
		: Menu_Layer2 ("Menu Title")
		, t  (1)
		, t1 (1)
	{
		SetViewDefault();
		last_menu = target_menu;
		last_pos = target_pos;
		last_rot = target_rot;
	}

	void SetViewDefault()
	{
		target_menu = 0;
		target_pos = glm::vec3(0, 0, 10);
		target_rot = glm::quat(1, 0, 0, 0);
	}

	void SetViewHighscores()
	{
		target_menu = 1;
		target_pos = glm::vec3(10, 0, 0);
		target_rot = glm::quat(sqrt(2) / 2, 0, sqrt(2) / 2, 0);
	}

	void SetViewSettings()
	{
		target_menu = 2;
		target_pos = glm::vec3(7.4, -8, 5);
		target_rot = glm::quat(.98, 0, .2, 0);
	}

	void SetViewGame()
	{
		target_menu = -1; // no menu
		target_pos = glm::vec3(10, 10, 10);
		target_rot = glm::quat(1, 0, 0, 0);
	}

	void GoBack()
	{
		switch (BackButtonTarget)
		{
			case MenuTarget::DEFAULT:     SetViewDefault();    break;
			case	MenuTarget::HIGHSCORES:  SetViewHighscores(); break;
			case	MenuTarget::SETTINGS:    SetViewSettings();   break;
			case	MenuTarget::GAME:        SetViewGame();       break;
		}

		if (BackButtonFunc)
		{
			BackButtonFunc();
		}
	}

	int Initialize() override;
	void UI() override;
};
//https://github.com/blender/blender/blob/master/source/blender/gpu/shaders/material/gpu_shader_material_tex_noise.glsl
