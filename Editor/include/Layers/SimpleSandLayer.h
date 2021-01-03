#pragma once 

#include "iw/engine/Layer.h"

#include "Sand/SandWorld.h"
#include "Sand/Tile.h"

namespace iw {
	class SimpleSandLayer
		: public Layer
	{
	private:
		iw::Mesh m_sandMesh;
		iw::ref<iw::Texture> m_sandTexture;

		SandWorld m_world;
		std::vector<Tile> m_tiles;
		iw::Entity shipEntity;

	public:
		SimpleSandLayer();

		int Initialize() override;
		void PostUpdate() override;
	private:
		bool MoveDown    (size_t x, size_t y, const Cell& cell);
		bool MoveDownSide(size_t x, size_t y, const Cell& cell);
		bool MoveSide    (size_t x, size_t y, const Cell& cell);
	};
}

inline void ShuffleIfTrue(bool& a, bool& b) {
	if (a && b) {
		bool rand = iw::randf() > 0;
		a = rand ? true : false;
		b = rand ? false : true;
	}
}
