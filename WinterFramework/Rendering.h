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

inline constexpr char const* GetErrorString(
	GLenum err) noexcept
{
	switch (err) {
		case GL_NO_ERROR:                      return "GL_NO_ERROR";
		case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
		case GL_TABLE_TOO_LARGE:               return "GL_TABLE_TOO_LARGE";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default:                               return "unknown error";
	}
}

inline bool MessageCallback(
	const char* stmt,
	const char* fname,
	int line)
{
	int finite = 0;
	bool hasError = true;
	GLenum lastErr = 0;

	while (hasError && finite < 10)
	{
		GLenum err = glGetError();
		hasError = err != GL_NO_ERROR;

		if (hasError)
		{
			const char* str = GetErrorString(err);
			
			if (err == lastErr) 
			{
				finite += 1;
				printf("\r");
			}

			else
			{
				finite = 0;
				printf("\n"); // this inf loops on a set of repeating two errors
			}

			printf("GL ERROR: %s in file %s[%d] %s", str, fname, line, stmt);
			if (err == lastErr) printf(" x%d", finite);
		}

		lastErr = err;
	}

	if (hasError)
	{
		printf("\n");
	}
	
	return hasError;
}

#ifdef IW_DEBUG
#	define gl(stmt)  stmt; if (MessageCallback(#stmt, __FILE__, __LINE__)) {}
#	define gle(stmt) stmt; if (MessageCallback(#stmt, __FILE__, __LINE__)) { err = true; }
#else
#	define gl(stmt)  stmt
#	define gle(stmt) stmt
#endif

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

struct event_MouseMove
{
	int x, y;
};

// should provide a way to map an sdl input to an enum, and read a char from it
// imgui handles all input typing for me

#include <unordered_map>

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

	Textures and Sprites

*/

struct Texture
{
	SDL_Surface* m_host;   // this is on the cpu
	GLuint m_device;       // this is on the gpu

	Color m_tint;
	bool m_static; // does sending to the device free the host

	Texture()
		: m_host   (nullptr)
		, m_device (0)
		, m_static (true)
	{}

	Texture(
		const std::string& path,
		Color tint = {},
		bool is_static = true
	)
		: m_device (0)
		, m_tint   (tint)
		, m_static (is_static)
	{
		m_host = SDL_LoadBMP(path.c_str());
		SDL_assert(m_host && "Sprite failed to load");
	}

	~Texture()
	{
		if (m_host) SDL_FreeSurface(m_host);
		if (m_device) gl(glDeleteTextures(1, &m_device));
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

	void SendToDevice()
	{
		GLint mode = 0;
		switch (m_host->format->BytesPerPixel)			
		{
			case 1: mode = GL_R;    break;
			case 2: mode = GL_RG;   break;
			case 3: mode = GL_RGB;  break;
			case 4: mode = GL_RGBA; break;
			default: assert(false); break;
		}

		if (OnDevice())
		{
			gl(glBindTexture(GL_TEXTURE_2D, m_device));
			gl(glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width(), Height(), mode, GL_UNSIGNED_BYTE, m_host->pixels));
		}

		else
		{
			gl(glGenTextures(1, &m_device));
			gl(glBindTexture(GL_TEXTURE_2D, m_device));
			gl(glTexImage2D(GL_TEXTURE_2D, 0, mode, Width(), Height(), 0, mode, GL_UNSIGNED_BYTE, m_host->pixels));
			gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}

		if (m_static)
		{
			FreeHost();
		}

		// state change on GL_TEXTUER_2D
	}

	// yes moves
	//  no copys
	Texture(Texture&& move)
		: m_host   (move.m_host)
		, m_device (move.m_device)
		, m_tint   (move.m_tint)
	{
		move.m_host = nullptr;
		move.m_device = 0;
	}
	Texture& operator=(Texture&& move)
	{
		m_host = move.m_host;
		m_device = move.m_device;
		m_tint = move.m_tint;
		move.m_host = nullptr;
		move.m_device = 0;
		return *this;
	}
	Texture(const Texture& copy) = delete;
	Texture& operator=(const Texture& copy) = delete;
};

/*

	Window and Renderer

*/

struct Camera
{
	float x, y, w, h;

	Camera()
		: x(0), y(0), w(12), h(8) 
	{}

	Camera(int x, int y, int w, int h)
		: x(x), y(y), w(w), h(h)
	{}

	glm::mat4 Projection() const
	{
		glm::mat4 camera = glm::ortho(-w, w, -h, h, -16.f, 16.f);
		camera = glm::translate(camera, glm::vec3(x, y, 0.f));

		return camera;
	}
};

struct SpriteRenderer2D
{
	GLuint m_vertices;
	GLuint m_shader;

	SpriteRenderer2D()
	{
		static float verts[6 * 4] = {
			-1,  1,  0, 1,
			 1, -1,  1, 0,
			-1, -1,  0, 0, 
		
			-1,  1,  0, 1,
			 1,  1,  1, 1,
			 1, -1,  1, 0
		};
		
		GLuint vbo;
		gl(glGenBuffers(1, &vbo));
		gl(glGenVertexArrays(1, &m_vertices));

		gl(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		gl(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));

		gl(glBindVertexArray(m_vertices));
		gl(glEnableVertexAttribArray(0));
		gl(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr));

		const char* source_vert = 
								"#version 330 core\n"
								"layout (location = 0) in vec4 vertex;" // <vec2 position, vec2 texCoords>

								"out vec2 TexCoords;"

								"uniform mat4 model;"
								"uniform mat4 projection;"

								"void main()"
								"{"
									"TexCoords = vertex.zw;"
									"gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);"
								"}";

		const char* source_frag = 
								"#version 330 core\n"
								"in vec2 TexCoords;"

								"out vec4 color;"

								"uniform sampler2D sprite;"
								"uniform vec4 tint;"

								"void main()"
								"{"
									"color = tint;" //  * texture(sprite, TexCoords)
								"}";

		GLuint shader_vert = gl(glCreateShader(GL_VERTEX_SHADER));
		gl(glShaderSource(shader_vert, 1, &source_vert, nullptr));
		gl(glCompileShader(shader_vert));

		GLint isCompiled = 0;
		glGetShaderiv(shader_vert, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader_vert, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader_vert, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(shader_vert);
			printf("Failed to compile vertex shader: %s\n", (char*)infoLog.data());
			assert(false && "Failed to compile vertex shader");
		}

		GLuint shader_frag = gl(glCreateShader(GL_FRAGMENT_SHADER));
		gl(glShaderSource(shader_frag, 1, &source_frag, nullptr));
		gl(glCompileShader(shader_frag));

		glGetShaderiv(shader_frag, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader_frag, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader_frag, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(shader_frag);
			printf("Failed to compile fragment shader: %s\n", (char*)infoLog.data());
			assert(false && "Failed to compile fragment shader");
		}

		m_shader = gl(glCreateProgram());
		gl(glAttachShader(m_shader, shader_vert));
		gl(glAttachShader(m_shader, shader_frag));

		gl(glLinkProgram(m_shader));

		gl(glDeleteShader(shader_vert));
		gl(glDeleteShader(shader_frag));
	}

	~SpriteRenderer2D()
	{
		if (m_vertices == 0) return; // has been moved

		// why does this error loop forever
		// do the vbos need to be deleted aswell?

		gl(glDeleteVertexArrays(1, &m_vertices));
		gl(glDeleteProgram(m_shader));
	}

	struct
	{
		glm::mat4 camera_proj;
	} 
	m_render_state;

	void Begin(Camera& camera) // include render target
	{
		m_render_state.camera_proj = camera.Projection();

		Color cc = Color(22, 22, 22, 22);
		gl(glClearColor(cc.rf(), cc.gf(), cc.bf(), cc.af()));
		gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	// should queue and sort by least state change, could also instance
	// but Ill just go with this until its a problem...

	void DrawSprite(const Transform2D& transform, Texture& sprite)
	{
		if (!sprite.OnDevice())
		{
			sprite.SendToDevice();
		}

		gl(glUseProgram(m_shader));

		gl(glBindTexture(GL_TEXTURE_2D, sprite.m_device));
		gl(glBindVertexArray(m_vertices));

		gl(glUniformMatrix4fv (glGetUniformLocation (m_shader, "projection"), 1, false, glm::value_ptr(m_render_state.camera_proj)));
		gl(glUniformMatrix4fv (glGetUniformLocation (m_shader, "model"),      1, false, glm::value_ptr(transform.World())));
		gl(glUniform4fv       (glGetUniformLocation (m_shader, "tint"),       1,        glm::value_ptr(sprite.m_tint.as_v4())));

		gl(glUniform1i(glGetUniformLocation(m_shader, "sprite"), 0));
		gl(glActiveTexture(GL_TEXTURE0));

		gl(glDrawArrays(GL_TRIANGLES, 0, 6));
	}

	// yes moves
	//  no copys
	SpriteRenderer2D(SpriteRenderer2D&& move)
		: m_vertices (move.m_vertices)
		, m_shader   (move.m_shader)
	{
		move.m_vertices = 0;
		move.m_shader = 0;
	}
	SpriteRenderer2D& operator=(SpriteRenderer2D&& move)
	{
		m_vertices = move.m_vertices;
		m_shader = move.m_shader;	
		move.m_vertices = 0;
		move.m_shader = 0;
		return *this;
	}
	SpriteRenderer2D(const SpriteRenderer2D& copy) = delete;
	SpriteRenderer2D& operator=(const SpriteRenderer2D& copy) = delete;
};

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
	}

	~Window()
	{
		if (m_window == nullptr) return; // has been moved

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
				case SDL_MOUSEMOTION:
				{
					m_events->send(event_MouseMove { event.motion.x, event.motion.y });
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
		gl(glViewport(0, 0, m_config.Width, m_config.Height ));
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