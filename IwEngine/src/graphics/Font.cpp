#include "iw/graphics/Font.h"
#include <istream>

namespace iw {
namespace Graphics {
	Font::Font(
		std::string name, 
		int size, 
		bool bold, 
		bool italic,
		std::string charSet,
		bool unicode, 
		unsigned stretchY, 
		bool smooth, 
		bool antialiadsed, 
		Padding padding, 
		Spacing spacing, 
		unsigned lightHeight, 
		unsigned base, 
		unsigned scaleX, 
		unsigned scaleY)
		: m_name(name)
		, m_size(size)
		, m_bold(bold)
		, m_italic(italic)
		, m_charSet(charSet)
		, m_unicode(unicode)
		, m_stretchY(stretchY)
		, m_smooth(smooth)
		, m_antiAliased(antialiadsed)
		, m_padding(padding)
		, m_spacing(spacing)
		, m_lightHeight(lightHeight)
		, m_base(base)
		, m_scaleX(scaleX)
		, m_scaleY(scaleY)
	{}

	void Font::Initialize(
		const iw::ref<IDevice>& device)
	{
		for (auto pair : m_textures) {
			pair.second->Initialize(device);
		}
	}

	Mesh Font::GenerateMesh(
		const std::string& string,
		const FontMeshConfig& config) const
	{
		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
		description.DescribeBuffer(iw::bName::UV, iw::MakeLayout<float>(2));

		return GenerateMesh(description, string, config);
	}

	Mesh Font::GenerateMesh(
		const MeshDescription& description,
		const std::string& string,
		const FontMeshConfig& config) const
	{
		MeshData* data = new MeshData(description);
		Mesh mesh = data->MakeInstance();
		mesh.Material = m_material;

		UpdateMesh(mesh, string, config);

		return mesh;
	}

	void Font::UpdateMesh(
		Mesh& mesh,
		const std::string& string,
		const FontMeshConfig& config) const
	{
		if (   !mesh.Data->Description().HasBuffer(bName::POSITION)
			|| !mesh.Data->Description().HasBuffer(bName::UV))
		{
			LOG_WARNING << "Cannot update a mesh data with description that does not contain at least a POSITION and UV buffer!";
			return;
		}

		std::vector<std::string> lines;
		size_t count = 0;

		std::istringstream stream(string);
		std::string line;
		while (std::getline(stream, line)) {
			lines.push_back(line);
			count += line.length();
		}

		int padWidth  = m_padding.Left + m_padding.Right;
		int padHeight = m_padding.Top + m_padding.Bottom;


		// line height and size are something else
		// I think the scale is actually the same for x and y, but when incrementing the cursor
		// at the bottom of the loop, we need to use the lineheight not the scale!~!

		float scale = config.Size / 2834.6456692913f; // https://www.convertunits.com/from/pt/to/meter
		float lineHeightScale = (m_lightHeight - padHeight) / 2834.6456692913f;

		unsigned indexCount = count * 6;
		unsigned vertCount  = count * 4;

		unsigned*  indices = new unsigned [indexCount];
		glm::vec3* verts   = new glm::vec3[vertCount];
		glm::vec2* uvs     = new glm::vec2[vertCount];

		// Center should center the actual text too 

		glm::vec2 cursor = glm::vec2(0.f);
		unsigned vert = 0;
		unsigned index = 0;
		for (std::string line : lines) 
		{
			for (size_t i = 1; i <= line.length(); i++) 
			{
				unsigned char1 = line[i - 1];
				unsigned char2 = line[i];

				const Character& c = GetCharacter(char1);
				int kerning = GetKerning(char1, char2);

				glm::vec2 dim(c.Width - padWidth, c.Height - padHeight);
				glm::vec2 tex(GetTexture(c.Page)->Width(), GetTexture(c.Page)->Height());

				float u = (c.X + m_padding.Left) / tex.x;
				float v = (c.Y + m_padding.Top)  / tex.y;
				float maxU = u + dim.x / tex.x;
				float maxV = v + dim.y / tex.y;

				float x = cursor.x + (c.Xoffset + m_padding.Left) * scale;
				float y = cursor.y - (c.Yoffset + m_padding.Top)  * scale;
				float maxX = x + dim.x * scale; // makes font size determined on file size
				float minY = y - dim.y * scale;

				verts[vert + 0] = glm::vec3(   x,    y, 0);
				verts[vert + 1] = glm::vec3(   x, minY, 0);
				verts[vert + 2] = glm::vec3(maxX,    y, 0);
				verts[vert + 3] = glm::vec3(maxX, minY, 0);

				uvs[vert + 0] = glm::vec2(   u,    v);
				uvs[vert + 1] = glm::vec2(   u, maxV);
				uvs[vert + 2] = glm::vec2(maxU,    v);
				uvs[vert + 3] = glm::vec2(maxU, maxV);

				indices[index + 0] = vert;
				indices[index + 1] = vert + 1;
				indices[index + 2] = vert + 2;
				indices[index + 3] = vert + 1;
				indices[index + 4] = vert + 3;
				indices[index + 5] = vert + 2;

				vert += 4;
				index += 6;

				cursor.x += (c.Xadvance + kerning) * scale; // makes font size determined on file size
			}

			cursor.x = 0;
			cursor.y -= config.Size * lineHeightScale;
		}
	
		mesh.Data->SetIndexData(indexCount, indices);
		mesh.Data->SetBufferDataPtr(bName::POSITION, vertCount, verts);
		mesh.Data->SetBufferDataPtr(bName::UV,       vertCount, uvs);

		// Font by default is anchored in top left
		// Font not behind monospaced makes this not work so well
		
		auto [min, max] = mesh.GetBounds<iw::d2>();

		float offsetX = 0;
		float offsetY = 0;

		switch (config.Anchor)
		{
			case FontAnchor::CENTER:
			{
				offsetX = (max.x - min.x) / 2;
				offsetY = (max.y - min.y) / 2;
				break;
			}
			case FontAnchor::TOP_CENTER:
			{
				offsetX = (max.x - min.x) / 2;
				break;
			}
			case FontAnchor::TOP_RIGHT:
			{
				offsetX = max.x - min.x;
				break;
			}
			case FontAnchor::TOP_LEFT: // default do nothing
			default:
				break;
		}

		for (unsigned i = 0; i < vertCount; i++)
		{
			verts[i].x = verts[i].x - offsetX;
			verts[i].y = verts[i].y + offsetY; // maybe this needs to take into account the padding around the text?
		}

		// only need to delete indices because the mesh gets the pointers

		delete[] indices;
	}

	iw::ref<Texture>& Font::GetTexture(
		unsigned page)
	{
		return m_textures.at(page);
	}

	const iw::ref<Texture>& Font::GetTexture(
		unsigned page) const
	{
		return m_textures.at(page);
	}

	Character& Font::GetCharacter(
		unsigned character)
	{
		return m_characters.at(character);
	}

	const Character& Font::GetCharacter(
		unsigned character) const 
	{
		return m_characters.at(character);
	}

	int Font::GetKerning(
		unsigned characterA,
		unsigned characterB) const
	{
		auto itr = m_kernings.find(std::make_pair(characterA, characterB));
		if (itr == m_kernings.end()) {
			return 0;
		}

		return itr->second;
	}

	void Font::SetCharacter(
		unsigned id,
		Character character)
	{
		m_characters[id] = character;
	}

	void Font::AddKerning(
		Kerning kerning)
	{
		m_kernings[std::make_pair(kerning.First, kerning.Second)] = kerning.Amount;
	}

	void Font::SetTexture(
		unsigned page, 
		iw::ref<Texture> texture)
	{
		m_textures[page] = texture;
	}

	FontBuilder::FontBuilder()
		: m_font("", 0, false, false, "", false, 0, false, 
			false, Padding(), Spacing(), 0, 0, 0, 0)
	{}

	void FontBuilder::SetName(
		const std::string & name)
	{
		m_font.m_name = name;
	}

	void FontBuilder::SetSize(
		int size)
	{
		m_font.m_size = size;
	}

	void FontBuilder::SetBold(
		bool bold)
	{
		m_font.m_bold = bold;
	}

	void FontBuilder::SetItalic(
		bool italic)
	{
		m_font.m_italic = italic;
	}

	void FontBuilder::SetCharSet(
		const std::string& charSet)
	{
		m_font.m_charSet = charSet;
	}

	void FontBuilder::SetUnicode(
		bool unicode)
	{
		m_font.m_unicode = unicode;
	}

	void FontBuilder::SetStretchY(
		unsigned stretchY)
	{
		m_font.m_stretchY = stretchY;
	}

	void FontBuilder::SetSmooth(
		bool smooth)
	{
		m_font.m_smooth = smooth;
	}

	void FontBuilder::SetAnitiAliased(
		bool antiAliased)
	{
		m_font.m_antiAliased = antiAliased;
	}

	void FontBuilder::SetPadding(
		Padding padding)
	{
		m_font.m_padding = padding;
	}

	void FontBuilder::SetSpacing(
		Spacing spacing)
	{
		m_font.m_spacing = spacing;
	}

	void FontBuilder::SetLightHeight(
		unsigned lightHeight)
	{
		m_font.m_lightHeight = lightHeight;
	}

	void FontBuilder::SetBase(
		unsigned base)
	{
		m_font.m_base = base;
	}

	void FontBuilder::SetScaleX(
		unsigned scaleX)
	{
		m_font.m_scaleX = scaleX;
	}

	void FontBuilder::SetScaleY(
		unsigned scaleY)
	{
		m_font.m_scaleY = scaleY;
	}

	void FontBuilder::SetTextureCount(
		unsigned count)
	{
		m_font.m_textures.reserve(count);
	}

	void FontBuilder::SetCharacterCount(
		unsigned count)
	{
		m_font.m_characters.reserve(count);
	}

	void FontBuilder::SetKerningCount(
		unsigned count)
	{
		m_font.m_kernings.reserve(count);
	}

	void FontBuilder::SetTexture(
		unsigned page,
		iw::ref<Texture> texture)
	{
		m_font.SetTexture(page, texture);
	}

	void FontBuilder::SetCharacter(
		unsigned id, 
		Character character)
	{
		m_font.SetCharacter(id, character);
	}

	void FontBuilder::AddKerning(
		Kerning kerning)
	{
		m_font.AddKerning(kerning);
	}
	
	Font* FontBuilder::Build() {
		return new Font(m_font);
	}
}
}
