#pragma once

#include "../RenderingHideSDL.h"

/*

	Textures and Sprites

*/

struct SpriteSDLImpl;

struct SpriteSDL : ISprite
{
private:
	SpriteSDLImpl* m_impl;

public:
	SpriteSDL(const std::string& path);
	~SpriteSDL() override;

	SpriteSDLImpl* _ptr() { return m_impl; }

	int Width() const override;
	int Height() const override;
	bool OnHost() const override;
	bool OnDevice() const override;
	void FreeHost() override;
};

/*

	Window and Renderer

*/

struct WindowSDLImpl;

struct WindowSDL : IWindow
{
private:
	WindowSDLImpl* m_impl;

public:
	WindowSDL(const WindowConfig& config, event_manager* events);
	~WindowSDL() override;

	WindowSDLImpl* _ptr() { return m_impl; }

	void PollEvents() override;

	void BeginRender() override;
	void EndRender() override;
	void DrawSprite(const SpriteTransform2D& transform, Sprite& sprite) override;
	void SendSpriteToDevice(Sprite& sprite) override;
};

/*

	Upcasts

*/

WindowSDLImpl* upcast(IWindow* sdl_window);
SpriteSDLImpl* upcast(ISprite* sdl_sprite);