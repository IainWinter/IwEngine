#pragma once

// if sdl is a wrapper around os calls, then hiding it is stupid
// keep it simple!

#include "Event.h"
#include "Common.h"
#include <string>
#include <cmath>

// renderer ties this to opengl
// should split this into an ext and seperate the imgui and window
// but you cannot mix and match so its a replacement of the renderer...
// this is why hiding the impl is nice, but whatever...

// all this could be hidden from the public interface
// but for now Ill leave it

// imgui is going to be the UI library for everything in the game
// so hard commit to tieing it up with the renderer

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "sdl/SDL.h"
#include "glad/glad.h"

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

	float rf() { return r / (float)255.f; }
	float gf() { return g / (float)255.f; }
	float bf() { return b / (float)255.f; }
	float af() { return a / (float)255.f; }
};

struct Texture
{
	SDL_Surface* m_host;   // this is on the cpu
	GLuint m_device;       // this is on the gpu

	Texture()
		: m_host   (nullptr)
		, m_device (0)
	{}

	Texture(
		const std::string& path
	)
		: m_device (0)
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
		if (m_device) glDeleteTextures(1, &m_device);
	}

	int Width() const { return m_host->w; }
	int Height() const {return m_host->h; }
	bool OnHost() { return m_host; }
	bool OnDevice() { return m_device > 0; }

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
		move.m_device = 0;
	}
	Texture& operator=(Texture&& move)
	{
		m_host = move.m_host;
		m_device = move.m_device;
		move.m_host = nullptr;
		move.m_device = 0;
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
	SDL_GLContext m_opengl;
	event_manager* m_events;
private:
	inline static bool s_first = true;

public:
	Window()
		: m_window (nullptr)
		, m_opengl (nullptr)
		, m_events (nullptr)
	{}

	Window(
		const WindowConfig& config,
		event_manager* events = nullptr
	)
		: m_events (events)
	{
		if (s_first) s_first = false;
		else assert(false && "Only a single window has been tested");

		const char* first_glsl_version = Window::Init_Video();

		m_window = SDL_CreateWindow(config.Title.c_str(), 0, 0, config.Width, config.Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL); // opengl only for imgui :<... prob good in long run but now i need a sprite rendering lib..........
		m_opengl = SDL_GL_CreateContext(m_window);
		
		SDL_GL_MakeCurrent(m_window, m_opengl);
		SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

		gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		glViewport(0, 0, config.Width, config.Height);

		Init_Imgui(first_glsl_version);
	}

	~Window()
	{
		Dnit_Imgui();
		SDL_GL_DeleteContext(m_opengl);
		SDL_DestroyWindow(m_window);
	}

	void PumpEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (!m_events)
			{
				if (event.type == SDL_QUIT)
				{
					SDL_Quit(); // allow for quit
				}
				
				continue; // pump but do nothing, maybe log
			} 

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

	// A dead simple sprite renderer
	// draws to the currently bound framebuffer
	// should queue and draw an instanced version sorted by texture

	void BeginRender()
	{
		Color cc = Color(22, 22, 22, 22);
		glClearColor(cc.rf(), cc.gf(), cc.bf(), cc.af());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndRender() 
	{
		SDL_GL_SwapWindow(m_window);
	}

	// draw a 1by1 square onto the current framebuffer with a simple texture shader

	void DrawSprite(const Transform2D& transform, Texture* sprite)
	{
		if (!sprite->OnDevice())
		{
			SendTextureToDevice(sprite);
		}

		// SDL_Rect dest;
		// dest.x = -sprite->m_host->w / 2 + floor(transform.x);
		// dest.y = -sprite->m_host->h / 2 + floor(transform.y);
		// dest.w = sprite->m_host->w;
		// dest.h = sprite->m_host->h;

		//SDL_RenderCopyEx(m_render, sprite->m_device, nullptr, &dest, transform.r, nullptr, SDL_FLIP_NONE);
	}

	void SendTextureToDevice(Texture* tex)
	{
		GLint mode = 0;
		switch (tex->m_host->format->BytesPerPixel)			
		{
			case 1: mode = GL_R;    break;
			case 2: mode = GL_RG;   break;
			case 3: mode = GL_RGB;  break;
			case 4: mode = GL_RGBA; break;
			default: assert(false); break;
		}

		if (tex->OnDevice())
		{
			glBindTexture(GL_TEXTURE_2D, tex->m_device);
			glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex->Width(), tex->Height(), mode, GL_UNSIGNED_BYTE, tex->m_host->pixels);
		}

		else
		{
			glGenTextures(1, &tex->m_device);
			glBindTexture(GL_TEXTURE_2D, tex->m_device);
			glTexImage2D(GL_TEXTURE_2D, 0, mode, tex->Width(), tex->Height(), 0, mode, GL_UNSIGNED_BYTE, tex->m_host->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		// state change on GL_TEXTUER_2D
	}

	// imgui renderer

	void BeginImgui()
	{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window);
        ImGui::NewFrame();
	}

	void EndImgui()
	{
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	// yes moves
	//  no copys
	Window(Window&& move)
		: m_window (move.m_window)
		, m_opengl (move.m_opengl)
		, m_events (move.m_events)
	{
		move.m_window = nullptr;
		move.m_opengl = nullptr;
		move.m_events = nullptr;
	}
	Window& operator=(Window&& move)
	{
		m_window = move.m_window;
		m_opengl = move.m_opengl;
		m_events = move.m_events;
		move.m_window = nullptr;
		move.m_opengl = nullptr;
		move.m_events = nullptr;
		return *this;
	}
	Window(const Window& move) = delete;
	Window& operator=(const Window& move) = delete;

// init funcs

private:
	static const char* Init_Video()
	{
		SDL_Init(SDL_INIT_VIDEO);

		// set OpenGL attributes
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE
		);

		// is this needed?

#ifdef __APPLE__
		// GL 3.2 Core + GLSL 150
		SDL_GL_SetAttribute( // required on Mac OS
			SDL_GL_CONTEXT_FLAGS,
			SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
		);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		return "#version 150";
#elif __linux__
		// GL 3.2 Core + GLSL 150
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		return "#version 150";
#elif _WIN32
		// GL 3.0 + GLSL 130
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		return "#version 130";
#endif
	}

	void Init_Imgui(const char* glsl_version)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForOpenGL(m_window, m_opengl);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	void Dnit_Imgui()
	{
		ImGui_ImplOpenGL3_Shutdown();
    	ImGui_ImplSDL2_Shutdown();
 		ImGui::DestroyContext();
	}
};