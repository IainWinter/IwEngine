#include "iw/graphics/Font.h"
#include <istream>

namespace IW {
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

	Mesh* Font::GenerateMesh(
		std::string string,
		float lineHeight,
		float ratio) const
	{
		std::vector<std::string> lines;
		size_t count = 0;

		std::istringstream stream(string);
		std::string line;
		while (std::getline(stream, line)) {
			lines.push_back(line);
			count += line.length();
		}

		int padWidth  = m_padding.Left + m_padding.Right;
		int padHeight = m_padding.Top  + m_padding.Bottom;

		int lineHeightPixels = m_lightHeight - padHeight;

		iw::vector2 charDim = iw::vector2(
			lineHeight / lineHeightPixels,
			lineHeight / lineHeightPixels / ratio
		);

		Mesh* mesh = new Mesh();

		mesh->VertexCount = count * 4;
		mesh->IndexCount  = count * 6;

		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Uvs      = new iw::vector2[mesh->VertexCount];
		mesh->Indices  = new unsigned   [mesh->IndexCount];

		iw::vector2 cursor;
		unsigned vert  = 0;
		unsigned index = 0;
		for (std::string line : lines) {
			for (unsigned character : line) {
				const Character& c = GetCharacter(character);
				
				float xTex = float(c.X + m_padding.Left) / GetTexture(c.Page)->Width();
				float yTex = float(c.Y + m_padding.Top)  / GetTexture(c.Page)->Height();

				int width  = c.Width  - padWidth;
				int height = c.Height - padHeight;

				float quadWidth =  width  * charDim.x;
				float quadHeight = height * charDim.y;

				float xTexSize = (float)width  / GetTexture(c.Page)->Width();
				float yTexSize = (float)height / GetTexture(c.Page)->Height();

				float xOff = (c.Xoffset + m_padding.Left) * charDim.x;
				float yOff = (c.Yoffset + m_padding.Top)  * charDim.y;

				float xAdvance = (c.Xadvance - padWidth) * charDim.x;

				float x = cursor.x + (xOff * m_size);
				float y = cursor.y + (yOff * m_size);
				float maxX = x + (quadWidth  * m_size);
				float maxY = y + (quadHeight * m_size);

				mesh->Vertices[vert + 0] = iw::vector3(x,    y,    0);
				mesh->Vertices[vert + 1] = iw::vector3(x,    maxY, 0);
				mesh->Vertices[vert + 2] = iw::vector3(maxX, y,    0);
				mesh->Vertices[vert + 3] = iw::vector3(maxX, maxY, 0);

				mesh->Uvs[vert + 0] = iw::vector2(xTex + xTexSize, yTex + yTexSize);
				mesh->Uvs[vert + 1] = iw::vector2(xTex + xTexSize, yTex);
				mesh->Uvs[vert + 2] = iw::vector2(xTex,            yTex + yTexSize);
				mesh->Uvs[vert + 3] = iw::vector2(xTex,            yTex);

				mesh->Indices[index + 0] = vert;
				mesh->Indices[index + 1] = vert + 1;
				mesh->Indices[index + 2] = vert + 2;
				mesh->Indices[index + 3] = vert + 1;
				mesh->Indices[index + 4] = vert + 3;
				mesh->Indices[index + 5] = vert + 2;

				vert  += 4;
				index += 6;

				cursor.x += xAdvance * m_size;
			}

			cursor.y += lineHeight * m_size;
		}

		return mesh;
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
		unsigned characterB)
	{
		return m_kernings.at(std::make_pair(characterA, characterB));
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
