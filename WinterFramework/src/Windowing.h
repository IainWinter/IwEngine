#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "sdl/SDL.h"
#include "glad/glad.h"
#include <unordered_map>

#include "Event.h"

/*

	Events

*/

struct event_Shutdown
{

};

struct event_WindowResize
{
	int width, height;
};

struct event_Mouse
{
	int pixel_x, pixel_y;
	float screen_x, screen_y;
	float vel_x, vel_y;

	bool button_left;
	bool button_middle;
	bool button_right;
	bool button_x1;
	bool button_x2;

	int button_repeat;
};

enum class InputName
{
	_NONE,
	UP,
	DOWN,
	RIGHT,
	LEFT,
};

struct InputMapping
{
	std::unordered_map<SDL_Scancode, InputName> m_keyboard;

	InputName Map(SDL_Scancode code)
	{
		InputName name = InputName::_NONE;
		auto itr = m_keyboard.find(code);
		if (itr != m_keyboard.end())
		{
			name = itr->second;
		}

		return name;
	}
};

struct event_Input
{
	InputName name;
	float state;
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

	WindowConfig m_config;
	InputMapping m_input;

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
		, m_config (config)
	{
		if (s_first) s_first = false;
		else assert(false && "Only a single window has been tested");

		const char* first_glsl_version = Window::Init_Video();

		m_window = SDL_CreateWindow(config.Title.c_str(), 0, 0, config.Width, config.Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
		m_opengl = SDL_GL_CreateContext(m_window);
		
		SDL_GL_MakeCurrent(m_window, m_opengl);
		SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

		gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		Init_Imgui(first_glsl_version);

		Resize(config.Width, config.Height);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		// vsync

		SDL_GL_SetSwapInterval(0);
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
			ImGui_ImplSDL2_ProcessEvent(&event); // does this need to be between frames?

			if (!m_events)
			{
				if (event.type == SDL_QUIT)
				{
					SDL_Quit(); // allow for quit without events
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
				case SDL_MOUSEBUTTONUP: // can these be combined?
				case SDL_MOUSEBUTTONDOWN:
				{
					m_events->send(event_Mouse {
						event.button.x,                                                           // position as (0, width/height)
						event.button.y,
						(                    event.button.x  / (float)m_config.Width)  * 2 - 1,   // position as (-1, +1)
						((m_config.Height -  event.button.y) / (float)m_config.Height) * 2 - 1,
						0.f,                                                                      // velocity
						0.f,
						event.button.state == SDL_BUTTON_LEFT,
						event.button.state == SDL_BUTTON_MIDDLE,
						event.button.state == SDL_BUTTON_RIGHT,
						event.button.state == SDL_BUTTON_X1, // no x1 x2?
						event.button.state == SDL_BUTTON_X2,
						event.button.clicks
					});	
					break;
				}
				case SDL_MOUSEMOTION:
				{
					m_events->send(event_Mouse { 
						event.motion.x,                                                          // position as (0, width/height)
						event.motion.y,
						(                   event.motion.x  / (float)m_config.Width)  * 2 - 1,   // position as (-1, +1)
						((m_config.Height - event.button.y) / (float)m_config.Height) * 2 - 1,
						(event.motion.xrel / (float)m_config.Width)  * 2 - 1,                    // velocity
						(event.motion.yrel / (float)m_config.Height) * 2 - 1,
						bool(event.motion.state & SDL_BUTTON_LMASK),
						bool(event.motion.state & SDL_BUTTON_MMASK),
						bool(event.motion.state & SDL_BUTTON_RMASK),
						bool(event.motion.state & SDL_BUTTON_X1MASK),
						bool(event.motion.state & SDL_BUTTON_X2MASK),
						0
					});
					break;
				}
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_RESIZED:
						{
							Resize(event.window.data1, event.window.data2);
							m_events->send(event_WindowResize { m_config.Width, m_config.Height });
							break;
						}
					}
					break;
				}
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				{
					if (event.key.repeat == 0)
					{
						m_events->send(event_Input { 
							m_input.Map(event.key.keysym.scancode),
							event.key.state == SDL_PRESSED ? 1.f : -1.f // -1 for += e.state math, check for > 0 for is pressed
						});
					}
					break;
				}
			}
		}
	}

	void Resize(int width, int height)
	{
		m_config.Width = width;
		m_config.Height = height;
		gl(glViewport(0, 0, m_config.Width, m_config.Height)); // for now this works, but shouldnt be here
	}

	void EndFrame() 
	{
		SDL_GL_SwapWindow(m_window);
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
		, m_config (move.m_config)
		, m_input  (move.m_input)
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
		m_config = move.m_config;
		m_input  = move.m_input;
		move.m_window = nullptr;
		move.m_opengl = nullptr;
		move.m_events = nullptr;
		return *this;
	}
	Window(const Window& copy) = delete;
	Window& operator=(const Window& copy) = delete;

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
