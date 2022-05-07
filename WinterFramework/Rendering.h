#pragma once

#include "Event.h"
#include <string>
#include <stdint.h>
#include <cmath>

using u32 = uint32_t;
using u8 = uint8_t;

struct Color
{
	union {
		u32 u32;
		struct { u8 r, g, b, a; };
	};

	Color(
		u8 r, u8 g, u8 b, u8 a = 255
	)
		: r (r)
		, g (g)
		, b (b)
		, a (a)
	{}
};

/*

	Textures and Sprites

*/

struct ISprite
{
	virtual ~ISprite() = default;

	virtual int Width() const = 0;
	virtual int Height() const = 0;

	virtual bool OnHost() const = 0;
	virtual bool OnDevice() const = 0;
	virtual void FreeHost() = 0;
};

struct Sprite
{
private: // only above public for init order
	ISprite* m_sprite;

public:
	int Width; // should be const but move doesnt allow that...
	int Height;

	Sprite()
		: m_sprite (nullptr)
		, Width    (0)
		, Height   (0)
	{}

	Sprite(
		const std::string& path
	)
		: m_sprite (Sprite::Create(path))
		, Width    (m_sprite->Width())
		, Height   (m_sprite->Height())
	{}

	~Sprite()
	{
		delete m_sprite;
	}

	ISprite* _ptr() { return m_sprite; }

	// yes moves
	//  no copys
	Sprite(Sprite&& move) : m_sprite(move.m_sprite), Width(move.Width), Height(move.Height) { move.m_sprite = nullptr; }
	Sprite& operator=(Sprite&& move) { Width = move.Width; Height = move.Height; m_sprite = move.m_sprite; move.m_sprite = nullptr; return *this; }
	Sprite(const Sprite& move) = delete;
	Sprite& operator=(const Sprite& move) = delete;

	bool OnHost() { return m_sprite->OnHost(); }
	bool OnDevice() { return m_sprite->OnDevice(); };
	void FreeHost() { m_sprite->FreeHost(); }

private:
	static ISprite* Create(const std::string& path);
};

// x and y get floored
struct SpriteTransform2D
{
	float x, y, r;
};

/*

	Window and Renderer

*/

struct WindowConfig
{
	std::string Title = "Welcome to Winter Framework";
	int Width = 640;
	int Height = 480;
};

struct IWindow
{
	virtual ~IWindow() = default;

	virtual void PollEvents() = 0;

	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual void DrawSprite(const SpriteTransform2D& transform, Sprite& sprite) = 0;
	virtual void SendSpriteToDevice(Sprite& sprite) = 0;
};

struct Window
{
private:
	IWindow* m_window;

public:
	Window()
		: m_window (nullptr)
	{}

	Window(
		const WindowConfig& config,
		event_manager* events = nullptr
	)
		: m_window (Window::Create(config, events))
	{}

	~Window()
	{ 
		delete m_window;
	}

	IWindow* _ptr() { return m_window; }

	// yes moves
	//  no copys
	Window(Window&& move) : m_window(move.m_window) { move.m_window = nullptr; }
	Window& operator=(Window&& move) { m_window = move.m_window; move.m_window = nullptr; return *this; }
	Window(const Window& move) = delete;
	Window& operator=(const Window& move) = delete;
	
	void PollEvents() { m_window->PollEvents(); }

	void BeginRender() { m_window->BeginRender(); }
	void EndRender() { m_window->EndRender(); }
	void DrawSprite(const SpriteTransform2D& transform, Sprite& sprite) { m_window->DrawSprite(transform, sprite); }
	void SendSpriteToDevice(Sprite& sprite) { m_window->SendSpriteToDevice(sprite); }

	// call once at start of program before using Window
	static void InitRendering();

private:
	// returns a new window, you own the pointer
	static IWindow* Create(const WindowConfig& config, event_manager* events);
};

/*

	Window and Renderer events

*/

struct event_Shutdown
{

};

struct event_MouseMove
{
	int x, y;
};