#include "iw/graphics/Material.h"
#include "iw/log/logger.h"

namespace iw {
namespace Graphics {
	Material::Material()
		: m_alloc(128)
		, m_transparency(Transparency::NONE)
		, m_castShadows(true)
		, m_wireframe(false)
		, m_initialized(false)
		, m_order(0)
	{}
	
	Material::Material(
		iw::ref<iw::Shader>& shader)
		: Shader(shader)
		, m_alloc(128)
		, m_transparency(Transparency::NONE)
		, m_castShadows(true)
		, m_wireframe(false)
		, m_initialized(false)
		, m_order(0)
	{}

	// get me outta here

	Material::Material(
		const Material& other)
		: Shader        (other.Shader)
		, m_alloc       (other.m_alloc.capacity())
		, m_properties  (other.m_properties)
		, m_textures    (other.m_textures)
		, Triangles       (other.Triangles)
		, m_transparency(other.m_transparency)
		, m_castShadows (other.m_castShadows)
		, m_wireframe   (other.m_wireframe)
		, m_initialized (other.m_initialized)
		, m_order(0)
	{
		for (auto& prop : m_properties) {
			unsigned size = prop.TypeSize * prop.Stride * prop.Count;
			void* ptr = m_alloc.alloc(size);
			memcpy(ptr, prop.Data, size);
			prop.Data = ptr;
		}
	}

	Material::Material(
		Material&& other) noexcept
		: Shader        (std::move(other.Shader))
		, m_alloc       (std::move(other.m_alloc))
		, m_properties  (std::move(other.m_properties))
		, m_textures    (std::move(other.m_textures))
		, Triangles       (std::move(other.Triangles))
		, m_transparency(other.m_transparency)
		, m_castShadows (other.m_castShadows)
		, m_wireframe   (other.m_wireframe)
		, m_initialized (other.m_initialized)
		, m_order       (other.m_order)
	{}

	Material::~Material() {}

	Material& Material::operator=(
		const Material& other)
	{
		Shader         = other.Shader;
		m_alloc        = iw::linear_allocator(other.m_alloc.capacity());
		m_properties   = other.m_properties;
		m_textures     = other.m_textures;
		Triangles        = other.Triangles;
		m_transparency = other.m_transparency;
		m_castShadows  = other.m_castShadows;
		m_wireframe    = other.m_wireframe;
		m_initialized  = other.m_initialized;
		m_order        = 0;

		for (auto& prop : m_properties) {
			unsigned size = prop.TypeSize * prop.Stride * prop.Count;
			void* ptr = m_alloc.alloc(size);
			memcpy(ptr, prop.Data, size);
			prop.Data = ptr;
		}

		return *this;
	}

	Material& Material::operator=(
		Material&& other) noexcept
	{
		Shader         = std::move(other.Shader);
		m_alloc        = std::move(other.m_alloc);
		m_properties   = std::move(other.m_properties);
		m_textures     = std::move(other.m_textures);
		Triangles        = std::move(other.Triangles);
		m_transparency = other.m_transparency;
		m_castShadows  = other.m_castShadows;
		m_wireframe    = other.m_wireframe;
		m_initialized  = other.m_initialized;
		m_order        = 0;

		return *this;
	}

	void Material::Initialize(
		const iw::ref<IDevice>& device)
	{
		static int s_counter = 0;

		if(!m_initialized) {
			m_initialized = true;
			m_order = ++s_counter;

			if (!Shader) {
				LOG_WARNING << "Tried to initialize material without a shader!";
				return;
			}
		
			if (!Shader->Handle()) {
				Shader->Initialize(device);
			}

			for (TextureProperty& prop : m_textures) {
				if (    prop.Texture
					&& !prop.Texture->Handle())
				{
					prop.Texture->Initialize(device);
				}
			}

			int count = Shader->Handle()->UniformCount();
			for (int i = 0; i < count; i++) {
				IPipelineParam* uniform = Shader->Handle()->GetParam(i); // this will prob fuck up if there are custom layouts
				if (!uniform) {
					continue; // Check for null
				}

				if(uniform->Name().substr(0, 4) != "mat_") {
					continue; // Check for 'mat_' prefix
				}
				
				std::string name = uniform->Name().substr(4);
				if (!Has(name)) {
					if (uniform->Type() == UniformType::SAMPLER) {
						SetTexture(name, nullptr);
					}

					else {
						SetProperty(name, nullptr,
							uniform->Type(),
							uniform->TypeSize(),
							uniform->Stride(),
							uniform->Count());
					}
				}
			}
		}
	}

	// not really an instance more of a bad copy xd
	ref<Material> Material::MakeInstance() const {
		return REF<Material>(*this);
	}

	void Material::Use(
		const iw::ref<IDevice>& device,
		iw::ref<iw::Shader> shader)
	{
		if (!m_initialized) {
			LOG_WARNING << "Cannot use material that has not been initialized!";
			return;
		}

		if (!shader) {
			shader = Shader;
		}

		//device->SetPipeline(Shader->Handle());

		device->SetWireframe(Wireframe());

		for (MaterialProperty& prop : m_properties) {
			if (!prop.Active) {
				continue;
			}

			IPipelineParam* param = shader->Handle()->GetParam("mat_" + prop.Name);

			if (!param) {
				if (shader == Shader) {
					LOG_WARNING << "Property in material not in shader: " << prop.Name;
					prop.Active = false;
				}

				continue;
			}

			switch (prop.Type) {
				case UniformType::BOOL:   param->SetAsBools  (prop.Data, prop.Stride, prop.Count); break;
				case UniformType::INT:    param->SetAsInts   (prop.Data, prop.Stride, prop.Count); break;
				case UniformType::UINT:   param->SetAsUInts  (prop.Data, prop.Stride, prop.Count); break;
				case UniformType::FLOAT:  param->SetAsFloats (prop.Data, prop.Stride, prop.Count); break;
				case UniformType::DOUBLE: param->SetAsDoubles(prop.Data, prop.Stride, prop.Count); break;
				default: LOG_WARNING << "Invalid property type in material: " << prop.Name; break;
			}
		}

		for (TextureProperty& prop : m_textures) {
			if (!prop.Active) {
				continue;
			}

			IPipelineParam* param = shader->Handle()->GetParam("mat_" + prop.Name);

			if (!param) {
				if (shader == Shader) {
					LOG_WARNING << "Texture in material not in shader: " << prop.Name;
					prop.Active = false;
				}

				continue;
			}

			if (prop.Texture) {
				if (!prop.Texture->Handle()) {
					LOG_WARNING << "Uninitialized texture in material: " << prop.Name;
				}

				//if (!Shader->Handle()->IsTextureActive(param)) {
					param->SetAsTexture(prop.Texture->Handle());
				//}
			}

			else {
				param->SetAsTexture(nullptr);
			}
		}
	}

	void Material::SetShader(
		iw::ref<iw::Shader>& shader)
	{
		Shader = shader;
	}

#define MAT_SETS(d, ut, ts)                            \
	void Material::Set(                                \
		std::string name,                              \
		d data)                                        \
	{                                                  \
		SetProperty(name, &data, ut, ts, 1, 1); \
	}                                                  \

	MAT_SETS(bool,     UniformType::BOOL,   sizeof(int))       // bools are just ints (in glsl) so they need to be resized!!
	MAT_SETS(int,      UniformType::INT,    sizeof(int))
	MAT_SETS(unsigned, UniformType::UINT,   sizeof(unsigned))
	MAT_SETS(float,    UniformType::FLOAT,  sizeof(float))
	MAT_SETS(double,   UniformType::DOUBLE, sizeof(double))

#undef MAT_SETS

#define MAT_SET(d, ut, ts, mp)                                  \
		void Material::Set(                                     \
			std::string name,                                   \
			d data,                                             \
			unsigned stride,                                    \
			unsigned count)                                     \
		{                                                       \
			SetProperty(name, mp(data), ut, ts, stride, count); \
		}                                                       \

		MAT_SET(bool*,       UniformType::BOOL,   sizeof(int)) // ^ see above ^
		MAT_SET(int*,        UniformType::INT,    sizeof(int))
		MAT_SET(unsigned*,   UniformType::UINT,   sizeof(unsigned))
		MAT_SET(float*,      UniformType::FLOAT,  sizeof(float))
		MAT_SET(double*,     UniformType::DOUBLE, sizeof(double))
		MAT_SET(glm::vec2,   UniformType::FLOAT,  sizeof(float), &)
		MAT_SET(glm::vec3,   UniformType::FLOAT,  sizeof(float), &)
		MAT_SET(glm::vec4,   UniformType::FLOAT,  sizeof(float), &)
		MAT_SET(Color,       UniformType::FLOAT,  sizeof(float), &)

#undef MAT_SET

	void Material::SetTexture(
		std::string name,
		iw::ref<Texture> texture)
	{
		if (Has(name)) {
			TextureProperty& prop = m_textures.at(Triangles.at(name));
			prop.Texture = texture;
			prop.Active = true;
		}

		else {
			auto itr = Triangles.emplace(name, m_textures.size());

			TextureProperty prop {
				itr.first->first,
				texture,
				true
			};

			m_textures.push_back(prop);
		}

		std::string n(name);    // diffuseMap -> hasDiffuseMap
		n[0] = toupper(n[0]);
		n = "has" + n;

		Set(n, texture != nullptr ? 1.0f : 0.0f); // why does this need to be floats??
	}

	iw::ref<Texture> Material::GetTexture(
		std::string name)
	{
		if (!Has(name)) {
			LOG_WARNING << "Tried to get texture that doesnt exist: " << name;
			return nullptr;
		}

		return m_textures.at(Triangles.at(name)).Texture;
	}

	bool Material::Has(
		std::string name) const
	{
		return Triangles.find(name) != Triangles.end();
	}

	iw::Transparency Material::Transparency() const {
		return m_transparency;
	}

	bool Material::CastShadows() const {
		return m_castShadows;
	}

	bool Material::Wireframe() const {
		return m_wireframe;
	}

	bool Material::IsInitialized() const {
		return m_initialized;
	}

	void Material::SetTransparency(
		iw::Transparency transparency)
	{
		m_transparency = transparency;
	}

	void Material::SetCastShadows(
		bool castShadows)
	{
		m_castShadows = castShadows;
	}

	void Material::SetWireframe(
		bool wireframe)
	{
		m_wireframe = wireframe;
	}

	void Material::SetName(
		std::string& name)
	{
		m_name = name;
	}

	const std::string& Material::Name() const {
		return m_name;
	}

	void Material::SetProperty(
		std::string name,
		const void* data,
		UniformType type,
		unsigned typeSize,
		unsigned stride,
		unsigned count)

	{
		if (type == UniformType::UNKNOWN) {
			LOG_WARNING << "Attempted to set uniform with an invalid type: " << name << "!";
			return;
		}

		unsigned size = typeSize * stride * count;

		MaterialProperty* prop;
		if (Has(name)) {
			prop = &m_properties.at(Triangles.at(name));
			if (prop->Type != type) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different type: " << name << "!";
				return;
			}
			
			if (prop->TypeSize != typeSize) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different type size: " << name << "!";
				return;
			}

			// may want to allow smaller counts and strides but lets see 
			if (prop->Stride != stride) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different stride: " << name << "!";
				return;
			}

			if (prop->Count != count) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different count: " << name << "!";
				return;
			}

			prop->Active = true;
		}

		else {
			auto itr = Triangles.emplace(name, m_properties.size());

			prop = &m_properties.emplace_back(
				MaterialProperty {
					itr.first->first,
					m_alloc.alloc(size),
					type,
					typeSize,
					stride,
					count,
					true
				}
			);

			if (!prop->Data) {
				m_alloc.resize(m_alloc.capacity() + size);
				prop->Data = m_alloc.alloc(size);
			}
		}

		if (!data) {
			memset(prop->Data, 0, size);
		}

		else {
			memcpy(prop->Data, data, size);
		}
	}

	Material::MaterialProperty& Material::GetProperty(
		std::string name)
	{
		return m_properties.at(Triangles.at(name));
	}
}
}
