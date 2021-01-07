#include "Sand/SandWorker.h"

class DefaultSandWorker : public SandWorker {
public:
	DefaultSandWorker(
		SandWorld& world, SandChunk& chunk)
		: SandWorker(world, chunk)
	{}

	void UpdateCell(
		WorldCoord x,
		WorldCoord y,
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
