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
		float size,
		float ratio) const
	{
		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
		description.DescribeBuffer(iw::bName::UV, iw::MakeLayout<float>(2));

		return GenerateMesh(description, string, size, ratio);
	}

	Mesh Font::GenerateMesh(
		const MeshDescription& description,
		const std::string& string,
		float size,
		float ratio) const
	{
		MeshData* data = new MeshData(description);
		Mesh mesh = data->MakeInstance();

		UpdateMesh(mesh, string, size, ratio);

		return mesh;
	}

	void Font::UpdateMesh(
		Mesh& mesh,
		const std::string& string,
		float size,
		float ratio) const
	{
		if (   !mesh.Data()->Description().HasBuffer(bName::POSITION)
			|| !mesh.Data()->Description().HasBuffer(bName::UV))
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

		int lineHeightPixels = m_lightHeight - padHeight;

		iw::vector2 scale = iw::vector2(
			size / lineHeightPixels * m_size * ratio,
			size / lineHeightPixels * m_size
		);

		unsigned indexCount = count * 6;
		unsigned vertCount  = count * 4;

		unsigned*    indices = new unsigned   [indexCount];
		iw::vector3* verts   = new iw::vector3[vertCount];
		iw::vector2* uvs     = new iw::vector2[vertCount];

		iw::vector2 cursor;
		unsigned vert = 0;
		unsigned index = 0;
		for (std::string line : lines) {
			for (size_t i = 1; i <= line.length(); i++) {
				unsigned char1 = line[i - 1];
				unsigned char2 = line[i];

				const Character& c = GetCharacter(char1);
				int kerning = GetKerning(char1, char2);

				iw::vector2 dim(c.Width - padWidth, c.Height - padHeight);
				iw::vector2 tex(GetTexture(c.Page)->Width(), GetTexture(c.Page)->Height());

				float u = (c.X + m_padding.Left) / tex.x;
				float v = (c.Y + m_padding.Top) / tex.y;
				float maxU = u + dim.x / tex.x;
				float maxV = v + dim.y / tex.y;

				float x = cursor.x + (c.Xoffset + m_padding.Left) * scale.x;
				float y = cursor.y - (c.Yoffset + m_padding.Top) * scale.y;
				float maxX = x + dim.x * scale.x;
				float minY = y - dim.y * scale.y;

				verts[vert + 0] = iw::vector3(   x,    y, 0);
				verts[vert + 1] = iw::vector3(   x, minY, 0);
				verts[vert + 2] = iw::vector3(maxX,    y, 0);
				verts[vert + 3] = iw::vector3(maxX, minY, 0);

				uvs[vert + 0] = iw::vector2(   u,    v);
				uvs[vert + 1] = iw::vector2(   u, maxV);
				uvs[vert + 2] = iw::vector2(maxU,    v);
				uvs[vert + 3] = iw::vector2(maxU, maxV);

				indices[index + 0] = vert;
				indices[index + 1] = vert + 1;
				indices[index + 2] = vert + 2;
				indices[index + 3] = vert + 1;
				indices[index + 4] = vert + 3;
				indices[index + 5] = vert + 2;

				vert += 4;
				index += 6;

				cursor.x += (c.Xadvance + kerning) * scale.x;
			}

			cursor.x = 0;
			cursor.y -= size * m_size;
		}
	
		mesh.Data()->SetIndexData(indexCount, indices);
		mesh.Data()->SetBufferData(bName::POSITION, vertCount, verts);
		mesh.Data()->SetBufferData(bName::UV,       vertCount, uvs);

		delete[] verts;
		delete[] uvs;
		delete[] indices;
	}

	iw::ref<Texture>& Font::GetTexture(
		unsigned id)
	{
		return m_textures.at(id);
	}

	const iw::ref<Texture>& Font::GetTexture(
		unsigned id) const
	{
		return m_textures.at(id);
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
		unsigned id, 
		iw::ref<Texture> texture)
	{
		m_textures[id] = texture;
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
		unsigned id, 
		iw::ref<Texture> texture)
	{
		m_font.SetTexture(id, texture);
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
