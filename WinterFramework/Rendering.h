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

#include "util/error_check.h" // gives gl
#include "sdl/SDL_surface.h"

#define STB_IMAGE_IMPLEMENTATION // not great, I guess this should be in a cpp file
#include "stb/stb_image.h"

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
	free(pixels); // stb calls free, doesnt HAVE to though so this is a lil jank
}

// I want to create the simplest graphics api that hides as much as possible away
// I only need simple Texture/Mesh/Shader, I dont even need materials
// user should be able to understand where the memory is without needing to know the specifics of each type
//   I like how cuda does it with host and device
// Each thing could be interfaces with a graphics objeect type, might make things more? or less confusing
// This should just be a wrapper, I dont want to cover everything, so expose underlying library

// I was using RAII, but that causes headaches with all the move constructors
// So here is how this works

// Constructor  ( create host memory / load from files )
// SendToDevice ( create device memory and copy over) [free host if static]
// Cleanup      ( destroy host and device memory if they exists)

// this allows these classes to be passed to functions without ref
// bc they are quite small, only store handles / pointers

// doesnt support reading from device, but just follow the pattern to do that. Need SendToHost, _InitOnHost, _UpdateOnHost
// or just require non static and have only SendToHost and UpdateOnHost
// now it does :)

// you cannot however, SendToDevice(), FreeHost(), SendToHost()
// Sending to host requires it is never freeed, and therefore not static

struct IDeviceObject
{
	IDeviceObject(
		bool is_static
	)
		: m_static (is_static)
	{} 

	void FreeHost()
	{
		assert_on_host();
		_FreeHost();
	}

	void FreeDevice()
	{
		assert_on_device();
		_FreeDevice();
	}

	void SendToDevice()
	{
		assert_on_host(); // always require at least something on host to be copied to device, no device only init

		if (OnDevice())
		{
			assert_not_static();
			_UpdateOnDevice();
		}

		else
		{
			_InitOnDevice();
			if (m_static) FreeHost();
		}
	}

	void SendToHost()
	{
		assert_on_host(); // need host memory
		assert_not_static();
		_UpdateFromDevice();
	}

	void Cleanup()
	{
		if (OnDevice()) _FreeDevice();
		if (OnHost())   _FreeHost();
	}

	bool IsStatic() const
	{
		return m_static;
	}

	// interface

public:
	virtual bool OnHost() const = 0; 
	virtual bool OnDevice() const = 0;
	virtual int  DeviceHandle() const = 0;      // underlying opengl handle, useful for custom calls

protected:
	virtual void _FreeHost() = 0;               // delete host memory
	virtual void _FreeDevice() = 0;             // delete device memory
	virtual void _InitOnDevice() = 0;           // create device memory
	virtual void _UpdateOnDevice() = 0;         // update device memory
	virtual void _UpdateFromDevice() = 0;       // update host memory
private:
	bool m_static;

	// some helpers

// these could be public
protected:
	void assert_on_host()    const { assert(OnHost()   && "Device object has no data on host"); }
	void assert_on_device()  const { assert(OnDevice() && "Device object has no data on device"); }
	void assert_is_static()  const { assert( m_static  && "Device object is static"); }
	void assert_not_static() const { assert(!m_static  && "Device object is not static"); }
};

// textures are very simple
// just two arrays, one on the cpu, one on the device
// can pass by value without much worry
struct Texture : IDeviceObject
{
private:
	SDL_Surface* m_host     = nullptr; // deafult construction
	GLuint       m_device   = 0u;

	int          m_width    = 0;       // cache so we don't need to read from host
	int          m_height   = 0; 
	int          m_channels = 0; 

// public texture specific functions

public:
	int Width()      const { return m_width; } 
	int Height()     const { return m_height; } 
	int Channels()   const { return m_channels; }
	u8* Pixels()     const { return (u8*)m_host->pixels; }
	int BufferSize() const { return Width() * Height() * Channels(); }

	// reads from the host
	// only rgba up to Channels() belong to the pixel at (x, y)
	// only r -> Channels() = 1
	// only rg -> Channels() = 2
	// only rgb -> Channels() = 3
	// full rgba -> Channels() = 4
	      Color& Get(int x, int y)       { assert_on_host(); return *(Color*)(Pixels() + x + y * m_width); }
	const Color& Get(int x, int y) const { assert_on_host(); return *(Color*)(Pixels() + x + y * m_width); }

	void Clear()
	{
		assert_on_host();
		memset(m_host->pixels, 0, m_width * m_height * m_channels);
	}

// interface

public:	
	bool OnHost()       const override { return m_host   != nullptr; }
	bool OnDevice()     const override { return m_device != 0u;      }
	int  DeviceHandle() const override { return m_device; } 
protected:
	void _FreeHost()
	{
		void* pixels = m_host->pixels;
		SDL_FreeSurface(m_host);
		free_image_using_stb(pixels);
		m_host = nullptr;
	}

	void _FreeDevice() override
	{
		gl(glDeleteTextures(1, &m_device));
		m_device = 0;
	}

	void _InitOnDevice() override
	{
		gl(glGenTextures(1, &m_device));
		gl(glBindTexture(GL_TEXTURE_2D, m_device));
		gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		gl(glTexImage2D(GL_TEXTURE_2D, 0, gl_format(), Width(), Height(), 0, gl_format(), GL_UNSIGNED_BYTE, Pixels()));
	}

	void _UpdateOnDevice() override
	{
		gl(glTextureSubImage2D(m_device, 0, 0, 0, Width(), Height(), gl_format(), GL_UNSIGNED_BYTE, Pixels()));
	}

	void _UpdateFromDevice() override
	{
		gl(glGetTextureImage(m_device, 0, gl_format(), GL_UNSIGNED_BYTE, BufferSize(), Pixels()));
	}

// construction

public:
	Texture()
		: IDeviceObject (true)
	{}

	Texture(
		const std::string& path,
		bool isStatic = true
	)
		: IDeviceObject (isStatic)
	{
		auto [pixels, width, height, channels] = load_image_using_stb(path);
		init_texture_host_memory(pixels, width, height, channels);
	}

	Texture(
		int width, int height, int channels,
		bool isStatic = true
	)
		: IDeviceObject (isStatic)
	{
		void* pixels = malloc(width * height * channels);
		init_texture_host_memory(pixels, width, height, channels);
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

		// pitch is the size in bytes of each row of image data

		m_host = SDL_CreateRGBSurfaceFrom(pixels, w, h, channels * 8, w * channels, masks[0], masks[1], masks[2], masks[3]);
		assert(m_host && "Sprite failed to load");

		m_width = w; // cache values
		m_height = h;
		m_channels = channels;
	}

	GLenum gl_format() const // doesnt need to be here
	{
		switch (m_host->format->BytesPerPixel)			
		{
			case 1: return GL_R;
			case 2: return GL_RG;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
		}

		assert(false);
		return -1;
	}
};

// meshes are annoying
// can have many many differnt setups
// I am going to make the desision to spit each buffer into a seperate array so they can be appended
// this is useful for a GenerateNormals function, which would add some buffers

// so goal is to be able to add a buffer ONLY by typename
// if vert attribs are a map, not an array, then I will store the device buffers as such

// I dont like this thought because functions cannot know what other functions will change
// so two could try and add the same type of buffer without knowing
// and there are no names that work for everything, so aAttrib1, 2, 3, 4, 5 are needed
// maybe should just use strings?
// or have an order

// hmm maybe create a the buffer obj that the mesh seems to be a collection
// and then the mesh is just a map of these
// MeshSource is a map of attribs to shared_ptrs of buffers
// then a Mesh is just an instance of this, can swap out attribs as it likes

// add std::enable_shared_from_this

struct Buffer : IDeviceObject
{
public:
	enum ElementType
	{
		_none, _u8, _u32, _f32
	};
private:
	void*        m_host     = nullptr; // deafult construction
	GLuint       m_device   = 0u;

	int          m_length   = 0;
	int          m_repeat   = 0;
	ElementType  m_type     = _none;

// public mesh specific functions

public:
	      void* Data()                   { return m_host; }
	const void* Data()             const { return m_host; }
	int         Length()           const { return m_length; }
	int         Repeat()           const { return m_repeat; }
	ElementType Type()             const { return m_type; }
	int         BytesPerElement()  const { return Repeat() * element_type_size(Type()); }
	int         Bytes()            const { return Length() * BytesPerElement(); }

	template<typename _t>       _t& Get(int i)       { assert_on_host(); return *(      _t*)m_host + i * BytesPerElement(); }
	template<typename _t> const _t& Get(int i) const { assert_on_host(); return *(const _t*)m_host + i * BytesPerElement(); }

	// length is number of elements
	void Set(int length, const void* data)
	{
		assert_on_host();
		int size = length * BytesPerElement();
		m_length = length;
		m_host = realloc(m_host, size);
		memcpy(m_host, data, size);
	}

	// length is number of elements
	// offset is the number of elements from the start
	// offset must be positive and adjacent or inside of the buffer
	// if the length + offset is larger than the buffer, it is realloced and appended to
	// data is left uninitalized if there are gaps between offsets
	void Append(int length, const void* data, int offset)
	{
		assert_on_host();
		assert(offset >= 0 && offset <= Length() && "offset must be positive and adjacent or inside of the buffer");
		int size = length * BytesPerElement();
		int osize = size + offset * BytesPerElement();
		if (osize > Bytes())
		{
			m_host = realloc(m_host, osize);
			m_length = length + offset;
		}

		memcpy((char*)m_host + offset, data, size);
	}

// interface

public:
	bool OnHost()       const override { return m_host   != nullptr; }
	bool OnDevice()     const override { return m_device != 0u;      }
	int  DeviceHandle() const override { return m_device; }
protected:
	void _FreeHost() override
	{
		free(m_host);
		m_host = nullptr;
	}

	void _FreeDevice() override
	{
		glDeleteBuffers(1, &m_device);
		m_device = 0;
	}

	void _InitOnDevice() override
	{
		gl(glGenBuffers(1, &m_device));
		gl(glBindBuffer(GL_ARRAY_BUFFER, m_device)); // user can bind to what they want after using ::DeviceHandle
		gl(glNamedBufferData(m_device, Bytes(), Data(), gl_static()));
	}

	void _UpdateOnDevice() override
	{
		// on realloc, does old buffer need to be destroied?

		int size = 0;
		glGetNamedBufferParameteriv(m_device, GL_BUFFER_SIZE, &size);
		if (Bytes() != size) { gl(glNamedBufferData(m_device, Bytes(), Data(), gl_static())); }
		else                 { gl(glNamedBufferSubData(m_device, 0, Bytes(), Data())); }
	}

	void _UpdateFromDevice() override
	{
		gl(glGetNamedBufferSubData(m_device, 0, Bytes(), Data()));
	}

// construction

public:
	Buffer()
		: IDeviceObject (true)
	{}

	Buffer(
		int length, int repeat, ElementType type,
		bool isStatic = true
	)
		: IDeviceObject (isStatic)
		, m_length      (length)
		, m_repeat      (repeat)
		, m_type        (type)
	{
		m_host = malloc(length * repeat * element_type_size(type));
	}
private:
	int element_type_size(ElementType type) const // doesnt need to be here
	{
		switch (type)
		{
			case _u8:  return sizeof(u8);
			case _u32: return sizeof(u32);
			case _f32: return sizeof(f32);
		}

		assert(false && "invalid buffer element type");
		return 0;
	}

	GLenum gl_static() const
	{
		return IsStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
	}
};

// mesh data is a list of buffer objects bound together
// by a vertex array
// attribs in shaders need to be in the order of AttribName

struct Mesh : IDeviceObject
{
public:
	enum AttribName
	{
		aPosition,
		aTextureCoord,

		aNormal,
		aTangent,
		aBiTangent,
		aCustom1,
		aCustom2,
		aCustom3,
		aCustom4,
		aCustom5,
		aCustom6,

		aIndexBuffer // the index buffer
	};

	enum DrawType
	{
		dTriangles,
		dLoops
	};
private:
	using _buffers = std::unordered_map<AttribName, r<Buffer>>;
	
	_buffers     m_buffers;            // deafult construction
	GLuint       m_device   = 0;

// public mesh specific functions

public:
	int NumberOfBuffers() const { return m_buffers.size(); }

	      r<Buffer>& Get(AttribName name)       { return m_buffers.at(name); }
	const r<Buffer>& Get(AttribName name) const { return m_buffers.at(name); }

	// instances a buffer
	void Add(AttribName name, const r<Buffer>& buffer)
	{
		assert(m_buffers.find(name) == m_buffers.end() && "Buffer already exists in mesh");
		assert(name != aIndexBuffer || (buffer->Type() == Buffer::_u32 && buffer->Repeat() == 1) && "index buffer must be of type 'int' with a repeat of 1.");
		m_buffers.emplace(name, buffer);
	}

	// creates an empty buffer
	r<Buffer> Add(AttribName name, int length, int repeat, Buffer::ElementType type)
	{
		r<Buffer> buffer = std::make_shared<Buffer>(length, repeat, type);
		Add(name, buffer);

		return buffer;
	}

	// creates an empty buffer
	// gets repeat from _t::length() or 1
	// gets type from ::value_type or _t 
	template<typename _t>
	r<Buffer> Add(AttribName name, const std::vector<_t>& data)
	{
		Buffer::ElementType type;
		int repeat = 1; // deafult

		constexpr bool isFloat = std::is_same<_t, float>::value;
		constexpr bool isByte  = std::is_same<_t,  char>::value || std::is_same<_t, unsigned char>::value; // or bool?
		constexpr bool isInt   = std::is_same<_t,   int>::value || std::is_same<_t, unsigned  int>::value;

		if constexpr (!isFloat && !isByte && !isInt)
		{
			// assume has a value_type and length() like glm

			constexpr bool _isFloat = std::is_same<typename _t::value_type, float>::value;
			constexpr bool _isByte  = std::is_same<typename _t::value_type,  char>::value || std::is_same<typename _t::value_type, unsigned char>::value; // or bool?
			constexpr bool _isInt   = std::is_same<typename _t::value_type,   int>::value || std::is_same<typename _t::value_type, unsigned  int>::value;

			if constexpr (_isFloat) { type = Buffer::_f32; }
			if constexpr (_isByte)  { type = Buffer::_u8;  }
			if constexpr (_isInt)   { type = Buffer::_u32; }

			repeat = _t::length();
		}

		if constexpr (isFloat) { type = Buffer::_f32; }
		if constexpr (isByte)  { type = Buffer::_u8;  }
		if constexpr (isInt)   { type = Buffer::_u32; }

		r<Buffer> buffer = Add(name, data.size(), repeat, type);
		buffer->Set(data.size(), data.data());

		return buffer;
	}

	void Draw(DrawType drawType = DrawType::dTriangles)
	{
		if (!OnDevice()) SendToDevice();
		gl(glBindVertexArray(m_device));

		if (m_buffers.find(aIndexBuffer) != m_buffers.end())
		{
			r<Buffer> index = m_buffers.at(aIndexBuffer);
			gl(glDrawElements(gl_drawtype(drawType), index->Length(), GL_UNSIGNED_INT, nullptr));
		}

		else
		{
			glDrawArrays(gl_drawtype(drawType), 0, m_buffers.at(aPosition)->Length());
		}
	}

// interface

public:

	// All these functions will skip buffers
	// that dont meet assertion requirements
	
	// OnHost gives no information about underlying buffers
	// only if it contains any buffers
	// this is to meet interface: SendToDevice frees static hosts
	// then onhost would return false for when the mesh would try and call free host on itself
	// little hackey
	// might want to return true always just to hammer the point home that this func isnt what its ment to be

	bool OnHost()       const override { return m_buffers.size() != 0; }
	bool OnDevice()     const override { return m_device != 0u; }
	int  DeviceHandle() const override { return m_device; } 
protected:
	void _FreeHost() override
	{
		for (auto& [_, buffer] : m_buffers) if (buffer->OnHost()) buffer->FreeHost();
	}

	void _FreeDevice() override
	{
		for (auto& [_, buffer] : m_buffers) if (buffer->OnDevice()) buffer->FreeDevice();
		glDeleteVertexArrays(1, &m_device);
		m_device = 0;
	}

	void _InitOnDevice() override
	{
		gl(glGenVertexArrays(1, &m_device));
		gl(glBindVertexArray(m_device));

		for (auto& [attrib, buffer] : m_buffers)
		{
 			if (buffer->OnHost()) 
			{
				buffer->SendToDevice();
			}
			
			if (attrib == aIndexBuffer)
			{
				gl(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->DeviceHandle())); // set for vba
				continue;
			}

			gl(glBindBuffer(GL_ARRAY_BUFFER, buffer->DeviceHandle()));
			gl(glVertexAttribPointer(attrib, buffer->Repeat(), gl_format(buffer->Type()), GL_FALSE, 0/*buffer->BytesPerElement()*/, nullptr));
			gl(glEnableVertexAttribArray(attrib));
		}
	}

	void _UpdateOnDevice() override
	{
		for (auto& [_, buffer] : m_buffers) if (buffer->OnHost()) buffer->SendToDevice();
	}

	void _UpdateFromDevice() override
	{
		for (auto& [_, buffer] : m_buffers) if (buffer->OnDevice()) buffer->SendToHost();
	}

// construction

public:
	Mesh(
		bool isStatic = true
	)
		: IDeviceObject (isStatic)
	{}

	// add instancing constructor

private:
	GLenum gl_format(Buffer::ElementType type) const // doesnt need to be here
	{
		switch (type)
		{
			case Buffer::_u8:  return GL_UNSIGNED_BYTE;
			case Buffer::_u32: return GL_INT;
			case Buffer::_f32: return GL_FLOAT;
		}

		assert(false);
		return -1;
	}
	
	GLenum gl_static() const
	{
		return IsStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
	}

	GLenum gl_drawtype(DrawType drawType) const
	{
		switch (drawType)
		{
			case DrawType::dTriangles: return GL_TRIANGLES;
			case DrawType::dLoops:     return GL_LINE_LOOP;
		}

		assert(false);
		return -1;
	}
};

// this turned out to be very similar to a mesh hmmm

struct ShaderProgram : IDeviceObject
{
public:
	enum ShaderName
	{
		sVertex,
		sFragment,
		sGeometry,
		sCompute
	};
private:
	using _buffers = std::unordered_map<ShaderName, Buffer>; // buffer for string storage (host only)
	
	_buffers     m_buffers;            // deafult construction
	GLuint       m_device   = 0;

	int          m_slot     = 0;       // number of active texture slots

// public mesh specific functions

public:
	int NumberOfShaders()       const { return m_buffers.size(); }
	int NumberOfBoundTextures() const { return m_slot; }

	// appends source onto a shader
	void Add(ShaderName name, const char* str)
	{
		bool new_text = m_buffers.find(name) == m_buffers.end();
		if (new_text) m_buffers.emplace(name, Buffer(1, 1, Buffer::_u8));
		Buffer& buffer = m_buffers.at(name);
		buffer.Append(strlen(str) + 1, str, buffer.Length() - 1); // +1 copies null and -1 removes it
	}

	void Add(ShaderName name, const std::string& str)
	{
		Add(name, str.c_str());
	}

	void Use()
	{
		if (!OnDevice()) SendToDevice();
		gl(glUseProgram(m_device));
	}

	// these could be const if texture didnt get sent here
	// think of another way to do this...

	void Set(const std::string& name, const   int& x) { gl(glUniform1iv       (gl_location(name), 1,            (  int*)  &x)); }
	void Set(const std::string& name, const float& x) { gl(glUniform1fv       (gl_location(name), 1,            (float*)  &x)); }
	void Set(const std::string& name, const fvec1& x) { gl(glUniform1fv       (gl_location(name), 1,            (float*)  &x)); }
	void Set(const std::string& name, const fvec2& x) { gl(glUniform2fv       (gl_location(name), 1,            (float*)  &x)); }
	void Set(const std::string& name, const fvec3& x) { gl(glUniform3fv       (gl_location(name), 1,            (float*)  &x)); }
	void Set(const std::string& name, const fvec4& x) { gl(glUniform4fv       (gl_location(name), 1,            (float*)  &x)); }
	void Set(const std::string& name, const ivec1& x) { gl(glUniform1iv       (gl_location(name), 1,            (  int*)  &x)); }
	void Set(const std::string& name, const ivec2& x) { gl(glUniform2iv       (gl_location(name), 1,            (  int*)  &x)); }
	void Set(const std::string& name, const ivec3& x) { gl(glUniform3iv       (gl_location(name), 1,            (  int*)  &x)); }
	void Set(const std::string& name, const ivec4& x) { gl(glUniform4iv       (gl_location(name), 1,            (  int*)  &x)); }
	void Set(const std::string& name, const fmat2& x) { gl(glUniformMatrix2fv (gl_location(name), 1,  GL_FALSE, (float*)  &x)); }
	void Set(const std::string& name, const fmat3& x) { gl(glUniformMatrix3fv (gl_location(name), 1,  GL_FALSE, (float*)  &x)); }
	void Set(const std::string& name, const fmat4& x) { gl(glUniformMatrix4fv (gl_location(name), 1,  GL_FALSE, (float*)  &x)); }

	void Set(const std::string& name, Texture& texture)
	{
		if (!texture.OnDevice()) texture.SendToDevice();
		gl(glBindTexture(GL_TEXTURE_2D, texture.DeviceHandle())); // need texture usage
		gl(glActiveTexture(gl_slot()));
		gl(glUniform1i(gl_location(name), m_slot));
	}

// interface

public:

	bool OnHost()       const override { return m_buffers.size() != 0; }
	bool OnDevice()     const override { return m_device != 0u; }
	int  DeviceHandle() const override { return m_device; } 
protected:
	void _FreeHost() override
	{
		for (auto& [_, buffer] : m_buffers) if (buffer.OnHost()) buffer.FreeHost();
	}

	void _FreeDevice() override
	{
		gl(glDeleteProgram(m_device));
		m_device = 0;
	}

	void _InitOnDevice() override
	{
		m_device = gl(glCreateProgram());

		for (auto& [name, buffer] : m_buffers)
		{
			const char* source = (char*)buffer.Data(); // only reason this is a var is for odd error when it's an arg

			GLuint shader = gl(glCreateShader(gl_type(name)));
			glShaderSource(shader, 1, &source, nullptr);
			gl(glCompileShader(shader));

			// error check
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
				glDeleteShader(shader); // if soft error this can be removed
				printf("Failed to compile shader: %s\n", (char*)infoLog.data());
				assert(false && "Failed to compile shader"); // maybe soft error
			}

			gl(glAttachShader(m_device, shader));
			gl(glDeleteShader(shader));
		}

		gl(glLinkProgram(m_device));
	}

	void _UpdateOnDevice() override
	{
		glDeleteProgram(m_device);
		_InitOnDevice();
	}

	void _UpdateFromDevice() override
	{
		assert(false && "Updating from device has no impl for shaders");
		// maybe copy the source back? I dont think this will ever be useful
	}

// construction

public:
	ShaderProgram(
		bool isStatic = true
	)
		: IDeviceObject (isStatic)
	{}

private:
	GLenum gl_type(ShaderName type) const // doesnt need to be here
	{
		switch (type)			
		{
			case ShaderName::sVertex:   return GL_VERTEX_SHADER;
			case ShaderName::sFragment: return GL_FRAGMENT_SHADER;
			case ShaderName::sGeometry: return GL_GEOMETRY_SHADER;
			case ShaderName::sCompute:  return GL_COMPUTE_SHADER;
		}

		assert(false);
		return -1;
	}

	GLint gl_slot() const
	{
		return GL_TEXTURE0 + m_slot;
	}

	GLint gl_location(const std::string& name) const
	{
		return glGetUniformLocation(m_device, name.c_str());
	}
};

struct Camera
{
	float x, y, w, h;

	Camera()
		: x(0), y(0), w(12), h(8) 
	{}

	Camera(int x, int y, int w, int h)
		: x(x), y(y), w(w), h(h)
	{}

	mat4 Projection() const
	{
		mat4 camera = ortho(-w, w, -h, h, -16.f, 16.f);
		camera = translate(camera, vec3(x, y, 0.f));

		return camera;
	}
};

struct SpriteRenderer2D
{
	ShaderProgram m_shader;
	Mesh          m_quad;

	struct
	{
		mat4 camera_proj;
	}
	m_render_state;

	// this gets run multiple times... should save static stuff like shaders
	// drop raii just use init function or something

	SpriteRenderer2D()
	{
		m_quad.Add<vec2>(Mesh::aPosition,
		{
			vec2(-1, -1),
			vec2( 1, -1),
			vec2( 1,  1),
			vec2(-1,  1)
		});

		m_quad.Add<vec2>(Mesh::aTextureCoord,
		{
			vec2(0, 0),
			vec2(1, 0),
			vec2(1, 1),
			vec2(0, 1)
		});

		m_quad.Add<int>(Mesh::aIndexBuffer,
		{
			0, 1, 2,
			0, 2, 3
		});

		const char* source_vert = 
								"#version 330 core\n"
								"layout (location = 0) in vec2 pos;"
								"layout (location = 1) in vec2 uv;"

								"out vec2 TexCoords;"

								"uniform mat4 model;"
								"uniform mat4 projection;"

								"void main()"
								"{"
									"TexCoords = uv;"
									"gl_Position = projection * model * vec4(pos, 0.0, 1.0);"
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

		m_shader.Add(ShaderProgram::sVertex, source_vert);
		m_shader.Add(ShaderProgram::sFragment, source_frag);
	}

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
		m_shader.Use();
		m_shader.Set("projection", m_render_state.camera_proj);
		m_shader.Set("model",      transform.World());
		m_shader.Set("tint",       Color().as_v4());
		m_shader.Set("sprite",     sprite);

		m_quad.Draw();
	}
};

struct TriangleRenderer2D
{
	ShaderProgram m_shader;

	struct
	{
		mat4 camera_proj;
	}
	m_render_state;

	TriangleRenderer2D()
	{
		const char* source_vert = 
								"#version 330 core\n"
								"layout (location = 0) in vec2 vertex;"
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

		m_shader.Add(ShaderProgram::sVertex, source_vert);
		m_shader.Add(ShaderProgram::sFragment, source_frag);
	}

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
		m_shader.Use();
		m_shader.Set("projection", m_render_state.camera_proj);
		m_shader.Set("model",      transform.World());
		m_shader.Set("tint",       Color().as_v4());

		mesh.Draw(Mesh::dLoops);
	}
};
