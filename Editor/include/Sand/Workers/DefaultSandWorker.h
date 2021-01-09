#include "Sand/SandWorker.h"
#include "iw/entity/Space.h"
#include "iw/common/Components/Transform.h"

class DefaultSandWorker : public SandWorker {
private:
	iw::ref<iw::Space> Space;
public:
	DefaultSandWorker(                    // not default anymore \ /
		SandWorld& world, SandChunk& chunk, iw::ref<iw::Space> space)
		: SandWorker(world, chunk)
		, Space(space)
	{}

	void UpdateCell(
		WorldCoord x,
		WorldCoord y,
		Cell& cell) override;

	void UpdateHitCell(
		WorldCoord hx, WorldCoord hy,
		WorldCoord x,  WorldCoord y,
		Cell& cell) override;

	// maybe these ones are good to have in parent?

	bool MoveDown(
		int x, int y,
		Cell& cell,
		const Cell& replace);

	bool MoveDownSide(
		int x, int y,
		Cell& cell,
		const Cell& replace);

	bool MoveSide(
		int x, int y,
		Cell& cell,
		const Cell& replace);

	bool MoveRandom(
		int x, int y,
		Cell& cell,
		const Cell& replace);

	bool MoveForward(
		int x, int y,
		Cell& cell,
		const Cell& replace,
		bool& hit,
		int& hitx, int& hity);

	bool MoveAsSharedUser(
		int x, int y,
		Cell& cell,
		const Cell& replace);

	bool DeleteWithTime(
		int x, int y,
		Cell& cell);

	/////////////////////////////////////////////
	// move to child class

	void HitLikeProj(
		int x,  int y,
		int lx, int ly,
		Cell& cell);

	void HitLikeBeam(
		int x,  int y,
		int lx, int ly,
		Cell& cell);

	void HitLikeMissile(
		int x,  int y,
		int mx, int my,
		Cell& missile);
};
