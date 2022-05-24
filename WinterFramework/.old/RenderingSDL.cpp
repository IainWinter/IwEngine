#include "../RenderingSDL.h"
#include "sdl/SDL.h"

/*

	Textures and Sprites

*/

struct SpriteSDLImpl
{
	SDL_Surface* m_host;   // this is on the cpu
	SDL_Texture* m_device; // this is on the gpu

	SpriteSDLImpl(
		const std::string& path
	)
		: m_device (nullptr)
	{
		// So this could want to do many things

		// Blank render target
		// Texture on the gpu
		// Texture on the gpu, but keep original colors on the cpu for state
		// last one might be able to be 

		m_host = SDL_LoadBMP(path.c_str());
		SDL_assert(m_host && "Sprite failed to load");
	}

	~SpriteSDLImpl()
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
};

SpriteSDL::SpriteSDL(const std::string& path)
{
	m_impl = new SpriteSDLImpl(path);
}

SpriteSDL::~SpriteSDL()
{
	delete m_impl;
}

int SpriteSDL::Width() const { return m_impl->Width(); }
int SpriteSDL::Height() const { return m_impl->Height(); }
bool SpriteSDL::OnHost() const{ return m_impl->OnHost(); }
bool SpriteSDL::OnDevice() const { return m_impl->OnDevice(); }
void SpriteSDL::FreeHost() { m_impl->FreeHost(); }

ISprite* Sprite::Create(const std::string& path)
{
	return new SpriteSDL(path);
}

/*

	Window and Renderer

*/

struct WindowSDLImpl
{
public:
	SDL_Window* m_window;
	SDL_Renderer* m_render;

private:
	WindowConfig m_config;
	event_manager* m_events;

public:
	WindowSDLImpl(
		const WindowConfig& config,
		event_manager* events
	)
		: m_config (config)
		, m_events (events)
	{
		m_window = SDL_CreateWindow(config.Title.c_str(), 0, 0, config.Width, config.Height, SDL_WINDOW_RESIZABLE);
		m_render = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED); 

		SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}

	~WindowSDLImpl()
	{
		SDL_DestroyWindow  (m_window);
		SDL_DestroyRenderer(m_render);
	}

	// events

	void PollEvents()
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

	// window stronly linked with renderer. SDL seems to do this, so I will too
	// not the best design tho

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

	void DrawSprite(const SpriteTransform2D& transform, ISprite* sprite)
	{
		if (!sprite->OnDevice())
		{
			SendSpriteToDevice(sprite);
		}

		SpriteSDLImpl* impl = upcast(sprite);

		SDL_Rect dest;
		dest.x = -impl->m_host->w / 2 + floor(transform.x);
		dest.y = -impl->m_host->h / 2 + floor(transform.y);
		dest.w = impl->m_host->w;
		dest.h = impl->m_host->h;

		SDL_RenderCopyEx(m_render, impl->m_device, nullptr, &dest, transform.r, nullptr, SDL_FLIP_NONE);
	}

	void SendSpriteToDevice(ISprite* sprite)
	{
		SpriteSDLImpl* impl = upcast(sprite);

		if (impl->m_device) 
		{
			SDL_UpdateTexture(impl->m_device, nullptr, impl->m_host->pixels, impl->m_host->pitch);
		}

		else
		{
			impl->m_device = SDL_CreateTextureFromSurface(m_render, impl->m_host);
		}
	}
};

WindowSDL::WindowSDL(const WindowConfig& config, event_manager* events)
{
	m_impl = new WindowSDLImpl(config, events);
}

WindowSDL::~WindowSDL()
{
	delete m_impl;
}

void WindowSDL::PollEvents() { m_impl->PollEvents(); }
void WindowSDL::BeginRender() { m_impl->BeginRender(); }
void WindowSDL::EndRender() { m_impl->EndRender(); }
void WindowSDL::DrawSprite(const SpriteTransform2D& transform, Sprite& sprite) { m_impl->DrawSprite(transform, sprite._ptr()); }
void WindowSDL::SendSpriteToDevice(Sprite& sprite) { m_impl->SendSpriteToDevice(sprite._ptr()); }

// static init functions

IWindow* Window::Create(const WindowConfig& config, event_manager* events)
{
	return new WindowSDL(config, events);
}

void Window::InitRendering()
{
	SDL_Init(SDL_INIT_VIDEO);	
}

/*

	Upcasts

*/

WindowSDLImpl* upcast(IWindow* sdlwindow)
{
	return ((WindowSDL*)sdlwindow)->_ptr();
}

SpriteSDLImpl* upcast(ISprite* sdlsprite)
{
	return ((SpriteSDL*)sdlsprite)->_ptr();
}