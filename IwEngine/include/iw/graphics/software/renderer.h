#pragma once

#include <vector>
#include <array>
#include <tuple>

// small software renderer, could take out and make its own file, https://www.youtube.com/watch?v=PahbNFypubE
namespace software_renderer { 

	using namespace std;

	template<
		typename _v,
		typename _f1, typename _f2, typename _f3>
	void RasterTriangle(
		const _v* v0, const _v* v1, const _v* v2,
		_f1&& GetXY,
		_f2&& MakeSlope,
		_f3&& DrawScanline)
	{
		auto [x0, y0, x1, y1, x2, y2] = tuple_cat(GetXY(*v0), GetXY(*v1), GetXY(*v2));

		if (tie(y1, x1) < tie(y0, x0)) { swap(x0, x1); swap(y0, y1); swap(v0, v1); }
		if (tie(y2, x2) < tie(y0, x0)) { swap(x0, x2); swap(y0, y2); swap(v0, v2); }
		if (tie(y2, x2) < tie(y1, x1)) { swap(x1, x2); swap(y1, y2); swap(v1, v2); }

		if (y0 == y2) return; // 0 area

		bool shortside = (y1 - y0) * (x2 - x0) < (x1 - x0) * (y2 - y0);

		invoke_result_t<_f2, const _v&, const _v&, int> sides[2];
		sides[!shortside] = MakeSlope(*v0, *v2, y2 - y0);

		for (auto y = y0, endy = y0; /**/; ++y) // I wonder if this actually helps the inlinning of lambdas
		{
			if (y >= endy)
			{
				if (y >= y2) break;

				// has assignment! i dont like this

				sides[shortside] = apply(MakeSlope, (y < y1) ? tuple(*v0, *v1, (endy = y1) - y0)
													: tuple(*v1, *v2, (endy = y2) - y1));
			}

			DrawScanline(y, sides[0], sides[1]);
		}

		/*bool right = (y1 - y0) * (x2 - x0) < (x1 - x0) * (y2 - y0);
		bool left = !right;

		using Slope = invoke_result_t<_f2, const _v&, const _v&, int>;

		Slope sides[2];
		sides[right] = MakeSlope(*v0, *v2, y2 - y0);
		sides[left] = MakeSlope(*v0, *v1, y1 - y0);

		for (int y = y0; y < y1; y++)
		{
			DrawScanline(y, sides[0], sides[1]);
		}

		sides[left] = MakeSlope(*v1, *v2, y2 - y1);
		for (int y = y1; y < y2; y++)
		{
			DrawScanline(y, sides[0], sides[1]);
		}*/

	}

	// temp way of doing this, later can add way with slops n props if needed
	template<
		typename _v,
		typename _f1, typename _f2, typename _f3>
	void RasterLine(
		const _v* v0, const _v* v1,
		_f1&& GetXY,
		_f2&& MakeSlope,
		_f3&& PlotPixel)
	{
		auto [x0, y0, x1, y1] = tuple_cat(GetXY(*v0), GetXY(*v1));

		float dx = x1 - x0;
		float dy = y1 - y0;

		float distance = sqrt(dx*dx + dy*dy);
		dx /= distance;
		dy /= distance;

		float x = x0,
			 y = y0;

		for (int i = 0; i < ceil(distance); i++)
		{
			if constexpr (std::is_same_v<invoke_result_t<_f3, float, float>, bool>)
			{
				if (PlotPixel(x, y)) {
					break;
				}
			}

			else {
				PlotPixel(x, y);
			}

			x += dx;
			y += dy;
		}

		//if (tie(y1, x1) < tie(y0, x0)) { swap(x0, x1); swap(y0, y1); swap(v0, v1); }

		/*invoke_result_t<_f2, const _v&, const _v&, int, int> slopes
			= MakeSlope(*v0, *v1, x1 - x0, y1 - y0);

		int done = 0;
		do
		{
			PlotPixel(slopes[0].get(), slopes[1].get());

			done = 0;
			for (Slope& slope : slopes)
			{
				slope.step();
				done += int(slope.get() >= slope.end());
			}
		}
		while(done != slopes.size());

		*/
	}

	struct Slope {
		float m_cur, m_end, m_step;

		Slope() = default;
		Slope(float begin, float end, float steps) {
			float inv_step = 1.0f / steps;
			m_cur = begin;
			m_end = end;

			if (steps == 0) {
				m_step = 0.f;
			}

			else {
				m_step = (end - begin) * inv_step;
			}
		}

		float get() { return m_cur; }
		float end() { return m_end; }
		void step() { m_cur += m_step; }
	};

	using vertex = array<int, 4>; // x, y, u, v

	template<
		typename _f>
	void RasterPolygon(
		const vertex& v0, const vertex& v1, const vertex& v2,
		_f&& PlotPoint)
	{
		using SlopeData = array<Slope, 3>; // x, u, v

		RasterTriangle(&v0, &v1, &v2,
			[](const vertex& vert)
			{
				return make_pair(vert[0], vert[1]);
			},
			[](const vertex& from, const vertex& to, int steps)
			{
				SlopeData slopes;
				slopes[0] = Slope(from[0], to[0], steps); // x
				slopes[1] = Slope(from[2], to[2], steps); // u
				slopes[2] = Slope(from[3], to[3], steps); // v

				return slopes;
			},
			[&](int y, SlopeData& left, SlopeData& right)
			{
				float   xf = left[0] .get(),
					endxf = right[0].get();

				int     x =    xf,
					endx = endxf;

				Slope props[2]; // u, v inter scanline slopes
				props[0] = Slope(left[1].get(), right[1].get(), endxf - xf); // need to fix fidelity
				props[1] = Slope(left[2].get(), right[2].get(), endxf - xf);

				for (; x < endx; ++x)
				{
					PlotPoint(x, y, props[0].get(), props[1].get());
					for (auto& prop : props) prop.step();
				}

				for (auto& slope : left)  slope.step();
				for (auto& slope : right) slope.step();
			}
		);
	}

	// should try and make polygon like this where props arent hard defs
	template<
		typename _v,
		typename _f>
	void RasterLine(
		const _v& v0, const _v& v1,
		_f&& PlotPixel)
	{
		constexpr size_t slopeCount = std::tuple_size_v<_v>;

		using SlopeData = array<Slope, slopeCount>; // x, y, props

		RasterLine(&v0, &v1,
			[](const _v& vert)
			{
				return make_pair(vert[0], vert[1]);
			},
			[&](const _v& from, const _v& to, int xsteps, int ysteps)
			{
				SlopeData slopes;
				slopes[0] = Slope(from[0], to[0], xsteps);
				slopes[1] = Slope(from[1], to[1], ysteps);

				return slopes;
			},
			PlotPixel
		);
	}
}
