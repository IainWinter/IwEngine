#pragma once

// if sdl is a wrapper around os calls, then hiding it is stupid
// keep it simple!

#include "Event.h"
#include "Common.h"
#include <string>
#include <cmath>
#include <tuple>
#include <array>

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

#define STB_IMAGE_IMPLEMENTATION // not great, I guess this should be in a cpp file
#include "stb/stb_image.h"

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

	while (hasError && finite < 3)
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

			else if (finite > 0)
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

// mesh and texture, which gets used as a sprite, are very similar
// should think about combining. I want ease of use to be the focus

// struct IGraphicsObject
// {
// 	GLuint m_device; // handle to the object in the graphics library
// 	bool m_static; // if after submitting the data to the device, should the host be freeed

// 	bool OnDevice() const { return m_device != 0; };

// 	virtual bool OnHost() const = 0;
// 	virtual void SendToDevice() = 0;
// 	virtual void FreeHost() = 0;
// };

struct Texture
{
	SDL_Surface* m_host;   // this is on the cpu
	GLuint m_device;       // this is on the gpu
	bool m_static; // does sending to the device free the host

	Texture()
		: m_host   (nullptr)
		, m_device (0)
		, m_static (true)
	{}

	Texture(
		const std::string& path,
		bool is_static = true
	)
		: m_device (0)
		, m_static (is_static)
	{
		// pitch is the size in bytes of each row of image data
		
		auto [pixels, width, height, channels] = load_image_using_stb(path);
		init_texture_host_memory(pixels, width, height, channels);
	}

	Texture(
		int width, int height, int channels,
		bool is_static = true
	)
		: m_device (0)
		, m_static (is_static)
	{
		void* pixels = malloc(width * height * channels);
		init_texture_host_memory(pixels, width, height, channels);
	}

	~Texture()
	{
		if (m_host) 
		{
			void* pixels = m_host->pixels;
			SDL_FreeSurface(m_host);
			free(pixels); // free_image_using_stb(pixels); // stb calls free, doesnt HAVE to though so this is a lil jank
			m_host = nullptr;
		}

		if (m_device) 
		{
			gl(glDeleteTextures(1, &m_device));
		}
	}

private:
	void init_texture_host_memory(void* pixels, int w, int h, int channels)
	{
		std::array<int, 4> masks = {0, 0, 0, 0};
		switch (channels)
		{
			case 1:                                               masks[0] = 0x000000ff; break;
			case 2:                        masks[0] = 0x0000ff00; masks[1] = 0x000000ff; break;
			case 3: masks[0] = 0x00ff0000; masks[1] = 0x0000ff00; masks[2] = 0x000000ff; break;
			case 4: masks[0] = 0x00ff0000; masks[1] = 0x0000ff00; masks[2] = 0x000000ff; masks[3] = 0xff000000; break;
			default: assert(false && "no channels?"); break;
		}

		m_host = SDL_CreateRGBSurfaceFrom(pixels, w, h, channels * 8, w * channels, masks[0], masks[1], masks[2], masks[3]);
		SDL_assert(m_host && "Sprite failed to load");
	}
public:

	Color& Get(int x, int y)
	{
		int index = (x + y * m_host->w) * m_host->format->BytesPerPixel;
		return *(Color*)((u8*)m_host->pixels + index);
	}

	void ClearHost()
	{
		memset(m_host->pixels, 0, m_host->pitch * m_host->h);
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
			gl(glTextureSubImage2D(m_device, 0, 0, 0, Width(), Height(), mode, GL_UNSIGNED_BYTE, m_host->pixels));
		}

		else
		{
			gl(glGenTextures(1, &m_device));
			gl(glBindTexture(GL_TEXTURE_2D, m_device));
			gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			gl(glTexImage2D(GL_TEXTURE_2D, 0, mode, Width(), Height(), 0, mode, GL_UNSIGNED_BYTE, m_host->pixels));
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
		, m_static (move.m_static)
	{
		move.m_host = nullptr;
		move.m_device = 0;
	}
	Texture& operator=(Texture&& move)
	{
		m_host = move.m_host;
		m_device = move.m_device;
		m_static = move.m_static;
		move.m_host = nullptr;
		move.m_device = 0;
		return *this;
	}
	Texture(const Texture& copy) = delete;
	Texture& operator=(const Texture& copy) = delete;

private:
	std::tuple<u8*, int, int, int> load_image_using_stb(const std::string& filepath)
	{
		int width, height, channels, format;
		stbi_info(filepath.c_str(), &width, &height, &channels);

		switch (channels)
		{
			case 1: format = STBI_grey;       break;
			case 2: format = STBI_grey_alpha; break;
			case 3: format = STBI_rgb;        break;
			case 4: format = STBI_rgb_alpha;  break;
		}

		u8* pixels = stbi_load(filepath.c_str(), &width, &height, &channels, format);

		if (!pixels || stbi_failure_reason())
		{
			printf("failed to load image '%s' reason: %s\n", filepath.c_str(), stbi_failure_reason());
		}

		return std::make_tuple(pixels, width, height, channels);
	}

	void free_image_using_stb(void* pixels)
	{
		stbi_image_free(pixels);
	}
};

struct Mesh
{
	// should add index

	std::vector<glm::vec2> m_host;   // this is on the cpu
	GLuint m_device;       // this is on the gpu
	bool m_static; // does sending to the device free the host

	int m_vertex_count;

	Mesh()
		: m_host         ()
		, m_device       (0)
		, m_static       (true)
		, m_vertex_count (0)
	{}

	Mesh(
		std::vector<glm::vec2> host,
		bool is_static = true
	)
		: m_host         (host)
		, m_device       (0)
		, m_static       (is_static)
		, m_vertex_count (host.size())
	{}

	~Mesh()
	{
		if (m_device) gl(glDeleteVertexArrays(1, &m_device));
	}

	int TriangleCount() const { return m_vertex_count / 3; }
	int VertexCount() const {return m_vertex_count; }
	bool OnHost() { return m_host.size() > 0; }
	bool OnDevice() { return m_device > 0; }

	void FreeHost()
	{
		SDL_assert(OnHost() && "Nothing to free on the host");
		m_host.clear();
	}

	void SendToDevice()
	{
		m_vertex_count = m_host.size();

		if (OnDevice())
		{
			// gl(glBindTexture(GL_TEXTURE_2D, m_device));
			// gl(glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width(), Height(), mode, GL_UNSIGNED_BYTE, m_host->pixels));
		}

		else
		{
			GLuint vbo;
			gl(glGenBuffers(1, &vbo));
			gl(glGenVertexArrays(1, &m_device));

			gl(glBindBuffer(GL_ARRAY_BUFFER, vbo));
			gl(glBufferData(GL_ARRAY_BUFFER, m_vertex_count * 2 * sizeof(float), m_host.data(), GL_STATIC_DRAW));

			gl(glBindVertexArray(m_device));
			gl(glEnableVertexAttribArray(0));
			gl(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr));
		}

		if (m_static)
		{
			FreeHost();
		}

		// state change on GL_ARRAY_BUFFER
	}

	// yes moves
	//  no copys
	Mesh(Mesh&& move)
		: m_host   (std::move(move.m_host))
		, m_device (move.m_device)
		, m_static (move.m_static)
	{
		move.m_device = 0;
	}
	Mesh& operator=(Mesh&& move)
	{
		m_host = std::move(move.m_host);
		m_device = move.m_device;
		m_static = move.m_static;
		move.m_device = 0;
		return *this;
	}
	Mesh(const Mesh& copy) = delete;
	Mesh& operator=(const Mesh& copy) = delete;

private:
	std::tuple<u8*, int, int, int> load_image_using_stb(const std::string& filepath)
	{
		int width, height, channels, format;
		stbi_info(filepath.c_str(), &width, &height, &channels);

		switch (channels)
		{
			case 1: format = STBI_grey;       break;
			case 2: format = STBI_grey_alpha; break;
			case 3: format = STBI_rgb;        break;
			case 4: format = STBI_rgb_alpha;  break;
		}

		u8* pixels = stbi_load(filepath.c_str(), &width, &height, &channels, format);

		if (!pixels || stbi_failure_reason())
		{
			printf("failed to load image '%s' reason: %s\n", filepath.c_str(), stbi_failure_reason());
		}

		return std::make_tuple(pixels, width, height, channels);
	}

	void free_image_using_stb(void* pixels)
	{
		stbi_image_free(pixels);
	}
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

	// this gets run multiple times... should save static stuff like shaders
	// drop raii just use init function or something

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
									"color = tint * texture(sprite, TexCoords);"  
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
		if (m_shader == 0) return; // has been moved

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

	// I dont like clear here, it's dependent on the order of systems
	// drawings, which will make odd behaviour

	void Begin(Camera& camera, bool clear) // include render target
	{
		m_render_state.camera_proj = camera.Projection();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (clear)
		{
			Color cc = Color(22, 22, 22, 22);
			gl(glClearColor(cc.rf(), cc.gf(), cc.bf(), cc.af()));
			gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		}
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
		gl(glUniform4fv       (glGetUniformLocation (m_shader, "tint"),       1,        glm::value_ptr(Color().as_v4())));

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

struct TriangleRenderer2D
{
	GLuint m_shader;

	TriangleRenderer2D()
	{
		const char* source_vert = 
								"#version 330 core\n"
								"layout (location = 0) in vec2 vertex;" // <vec2 position>
								"uniform mat4 model;"
								"uniform mat4 projection;"
								"void main()"
								"{"
									"gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);"
								"}";

		const char* source_frag = 
								"#version 330 core\n"
								"out vec4 color;"
								"uniform vec4 tint;"
								"void main()"
								"{"
									"color = tint;"  
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

	~TriangleRenderer2D()
	{
		if (m_shader == 0) return; // has been moved
		gl(glDeleteProgram(m_shader));
	}

	struct
	{
		glm::mat4 camera_proj;
	}
	m_render_state;

	void Begin(Camera& camera, bool clear) // include render target
	{
		m_render_state.camera_proj = camera.Projection();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		if (clear)
		{
			Color cc = Color(22, 22, 22, 22);
			gl(glClearColor(cc.rf(), cc.gf(), cc.bf(), cc.af()));
			gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		}
	}

	// should queue and sort by least state change, could also instance
	// but Ill just go with this until its a problem...

	void DrawMesh(const Transform2D& transform, Mesh& mesh)
	{
		if (!mesh.OnDevice())
		{
			mesh.SendToDevice();
		}

		gl(glUseProgram(m_shader));

		gl(glBindVertexArray(mesh.m_device));

		gl(glUniformMatrix4fv (glGetUniformLocation (m_shader, "projection"), 1, false, glm::value_ptr(m_render_state.camera_proj)));
		gl(glUniformMatrix4fv (glGetUniformLocation (m_shader, "model"),      1, false, glm::value_ptr(transform.World())));
		gl(glUniform4fv       (glGetUniformLocation (m_shader, "tint"),       1,        glm::value_ptr(Color(255, 0, 0).as_v4())));

		gl(glDrawArrays(GL_LINE_LOOP, 0, mesh.VertexCount()));
	}

	// yes moves
	//  no copys
	TriangleRenderer2D(TriangleRenderer2D&& move)
		: m_shader   (move.m_shader)
	{
		move.m_shader = 0;
	}
	TriangleRenderer2D& operator=(TriangleRenderer2D&& move)
	{
		m_shader = move.m_shader;	
		move.m_shader = 0;
		return *this;
	}
	TriangleRenderer2D(const TriangleRenderer2D& copy) = delete;
	TriangleRenderer2D& operator=(const TriangleRenderer2D& copy) = delete;
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
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
					event_Mouse mouse;

					m_events->send(event_Mouse { 
						event.motion.x,                                      // position as (0, width/height)
						event.motion.y,
						(event.motion.x / (float)m_config.Width)  * 2 - 1,   // position as (-1, +1)
						(event.motion.y / (float)m_config.Height) * 2 - 1,
						float(event.motion.xrel),                            // velocity
						float(event.motion.yrel),
						bool(event.motion.state & SDL_BUTTON_LMASK),
						bool(event.motion.state & SDL_BUTTON_MMASK),
						bool(event.motion.state & SDL_BUTTON_RMASK),
						bool(event.motion.state & SDL_BUTTON_X1MASK),
						bool(event.motion.state & SDL_BUTTON_X2MASK),
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