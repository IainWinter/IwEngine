#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "iw/util/algorithm/pair_hash.h"
#include <unordered_map>

namespace iw {
namespace Graphics {
	struct Padding {
		int Top;
		int Right;
		int Bottom;
		int Left;
	};

	struct Spacing {
		int X;
		int Y;
	};

	struct Character {
		int X;
		int Y;
		int Width;
		int Height;
		int Xoffset;
		int Yoffset;
		int Xadvance;
		unsigned Page;
		unsigned Chnl;
	};

	struct Kerning {
		unsigned First;
		unsigned Second;
		int Amount;
	};

	class FontBuilder;

	struct Font {
	private:
		std::string m_name;
		int         m_size;
		bool        m_bold;
		bool        m_italic;
		std::string m_charSet;
		bool        m_unicode;
		int         m_stretchY;
		bool        m_smooth;
		bool        m_antiAliased;
		Padding     m_padding;
		Spacing     m_spacing;
		unsigned    m_lightHeight;
		unsigned    m_base;
		unsigned    m_scaleX;
		unsigned    m_scaleY;
		//int m_pages;
		//bool m_packed;

		friend class FontBuilder;

	public:
		ref<Material> m_material;

	private:
		std::unordered_map<unsigned, ref<Texture>> m_textures;
		std::unordered_map<unsigned, Character> m_characters;
		std::unordered_map<std::pair<unsigned, unsigned>, int, pair_hash> m_kernings;

	public:
		IWGRAPHICS_API
		Font(
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
			unsigned scaleY);

		IWGRAPHICS_API
		void Initialize(
			const ref<IDevice>& device);

		IWGRAPHICS_API
		Mesh GenerateMesh(
			const std::string& string,
			float size) const;

		IWGRAPHICS_API
		Mesh GenerateMesh(
			const MeshDescription& description,
			const std::string& string,
			float size) const;

		IWGRAPHICS_API
		void UpdateMesh(
			Mesh& meshData,
			const std::string& string,
			float size) const;

		IWGRAPHICS_API
		ref<Texture>& GetTexture(
			unsigned id);

		IWGRAPHICS_API
		const ref<Texture>& GetTexture(
			unsigned id) const;

		IWGRAPHICS_API
		Character& GetCharacter(
			unsigned character);

		IWGRAPHICS_API
		const Character& GetCharacter(
			unsigned character) const;

		IWGRAPHICS_API
		int GetKerning(
			unsigned characterA,
			unsigned characterB)  const;

		IWGRAPHICS_API
		void SetTexture(
			unsigned id,
			ref<Texture> texture);

		IWGRAPHICS_API
		void SetCharacter(
			unsigned id,
			Character character);

		IWGRAPHICS_API
		void AddKerning(
			Kerning kerning);
	};

	class FontBuilder {
	private:
		Font m_font;

	public:
		IWGRAPHICS_API
		FontBuilder();

		IWGRAPHICS_API
		void SetName(
			const std::string& name);

		IWGRAPHICS_API
		void SetSize(
			int size);

		IWGRAPHICS_API
		void SetBold(
			bool bold);

		IWGRAPHICS_API
		void SetItalic(
			bool italic);

		IWGRAPHICS_API
		void SetCharSet(
			const std::string& charSet);

		IWGRAPHICS_API
		void SetUnicode(
			bool unicode);

		IWGRAPHICS_API
		void SetStretchY(
			unsigned stretchY);

		IWGRAPHICS_API
		void SetSmooth(
			bool smooth);

		IWGRAPHICS_API
		void SetAnitiAliased(
			bool antiAliased);

		IWGRAPHICS_API
		void SetPadding(
			Padding padding);

		IWGRAPHICS_API
		void SetSpacing(
			Spacing spacing);

		IWGRAPHICS_API
		void SetLightHeight(
			unsigned lightHeight);

		IWGRAPHICS_API
		void SetBase(
			unsigned base);

		IWGRAPHICS_API
		void SetScaleX(
			unsigned scaleX);

		IWGRAPHICS_API
		void SetScaleY(
			unsigned scaleY);

		IWGRAPHICS_API
		void SetTextureCount(
			unsigned count);

		IWGRAPHICS_API
		void SetCharacterCount(
			unsigned count);

		IWGRAPHICS_API
		void SetKerningCount(
			unsigned count);

		IWGRAPHICS_API
		void SetTexture(
			unsigned id,
			ref<Texture> texture);

		IWGRAPHICS_API
		void SetCharacter(
			unsigned id,
			Character character);

		IWGRAPHICS_API
		void AddKerning(
			Kerning kerning);

		IWGRAPHICS_API
		Font* Build();
	};
}

	using namespace Graphics;
}
