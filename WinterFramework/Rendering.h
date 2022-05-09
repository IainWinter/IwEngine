#pragma once

// if sdl is a wrapper around os calls, then hiding it is stupid
// keep it simple!

#include "Event.h"
#include "Common.h"
#include "sdl/SDL.h"
#include <string>
#include <cmath>

/*

	Events

*/


struct event_Shutdown
{

};

struct event_MouseMove
{
	int x, y;
};

/*

	Textures and Sprites

*/


using u32 = uint32_t;
using u8 = uint8_t;

struct Color
{
	union { u32 as_u32; struct { u8 r, g, b, a; }; };
	Color(u8 r, u8 g, u8 b, u8 a = 255) : r(r), g(g), b(b), a(a) {}
};

struct Texture
{
	SDL_Surface* m_host;   // this is on the cpu
	SDL_Texture* m_device; // this is on the gpu

	Texture()
		: m_host   (nullptr)
		, m_device (nullptr)
	{}

	Texture(
		const std::string& path
	)
		: m_device (nullptr)
	{
		// So this could want to do many things

		// Blank render target
		// Texture on the gpu
		// Texture on the gpu, but keep original colors on the cpu for state

		m_host = SDL_LoadBMP(path.c_str());
		SDL_assert(m_host && "Sprite failed to load");
	}

	~Texture()
	{
		SDL_FreeSurface(m_host);
		SDL_DestroyTexture(m_device);
	}

	int Width() const { return m_host->w; }
	int Height() const {return m_host->h; }
	bool OnHost() { return m_host; }
	bool OnDevice() { return m_device; }

	void FreeHost()
	{
		SDL_assert(m_host && "Nothing to free on the host");
		SDL_FreeSurface(m_host);
		m_host = nullptr;
	}

	// yes moves
	//  no copys
	Texture(Texture&& move)
		: m_host   (move.m_host)
		, m_device (move.m_device)
	{
		move.m_host = nullptr;
		move.m_device = nullptr;
	}
	Texture& operator=(Texture&& move)
	{
		m_host = move.m_host;
		m_device = move.m_device;
		move.m_host = nullptr;
		move.m_device = nullptr;
		return *this;
	}
	Texture(const Texture& move) = delete;
	Texture& operator=(const Texture& move) = delete;
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

struct Window
{
public:
	SDL_Window* m_window;
	SDL_Renderer* m_render;
	event_manager* m_events;
private:
	inline static bool s_first = true;

public:
	Window()
		: m_window (nullptr)
		, m_render (nullptr)
		, m_events (nullptr)
	{}

	Window(
		const WindowConfig& config,
		event_manager* events = nullptr
	)
		: m_events (events)
	{
		if (s_first)
		{
			s_first = false;
			FirstVideoInit();
		}

		m_window = SDL_CreateWindow(config.Title.c_str(), 0, 0, config.Width, config.Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL); // opengl only for imgui...
		m_render = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED); 

		SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}

	~Window()
	{
		SDL_DestroyWindow  (m_window);
		SDL_DestroyRenderer(m_render);
	}

	void PumpEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (!m_events) continue; // pump but do nothing, maybe log

			switch (event.type)
			{
				case SDL_QUIT: 
				{
					m_events->send(event_Shutdown {});
					break;
				}
				case SDL_MOUSEMOTION:
				{
					m_events->send(event_MouseMove {
						event.motion.x,
						event.motion.y
					});
					break;
				}
			}
		}
	}

	void BeginRender()
	{
		Color cc = Color(22, 22, 22, 22);
		SDL_SetRenderDrawColor(m_render, cc.r, cc.g, cc.b, cc.a);
		SDL_RenderClear(m_render);
	}

	void EndRender() 
	{
		SDL_RenderPresent(m_render);
	}

	void DrawSprite(const Transform2D& transform, Texture* sprite)
	{
		if (!sprite->OnDevice())
		{
			SendTextureToDevice(sprite);
		}

		SDL_Rect dest;
		dest.x = -sprite->m_host->w / 2 + floor(transform.x);
		dest.y = -sprite->m_host->h / 2 + floor(transform.y);
		dest.w = sprite->m_host->w;
		dest.h = sprite->m_host->h;

		SDL_RenderCopyEx(m_render, sprite->m_device, nullptr, &dest, transform.r, nullptr, SDL_FLIP_NONE);
	}

	void SendTextureToDevice(Texture* sprite)
	{
		if (sprite->m_device) 
		{
			SDL_UpdateTexture(sprite->m_device, nullptr, sprite->m_host->pixels, sprite->m_host->pitch);
		}

		else
		{
			sprite->m_device = SDL_CreateTextureFromSurface(m_render, sprite->m_host);
		}
	}

	// yes moves
	//  no copys
	Window(Window&& move)
		: m_window (move.m_window)
		, m_render (move.m_render)
		, m_events (move.m_events)
	{
		move.m_window = nullptr;
		move.m_render = nullptr;
		move.m_events = nullptr;
	}
	Window& operator=(Window&& move)
	{
		m_window = move.m_window;
		m_render = move.m_render;
		m_events = move.m_events;
		move.m_window = nullptr;
		move.m_render = nullptr;
		move.m_events = nullptr;
		return *this;
	}
	Window(const Window& move) = delete;
	Window& operator=(const Window& move) = delete;

private:
	static void FirstVideoInit()
	{
		SDL_Init(SDL_INIT_VIDEO);
	}
};