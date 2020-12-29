#pragma once

#include "Cell.h"
#include <vector>

class SandWorld {
public:
	const size_t m_width  = 0;
	const size_t m_height = 0;
	const double m_scale = 1;
private:
	Cell* m_cells = nullptr;
	std::vector<std::pair<size_t, size_t>> m_changes; // destination, source

public:
	SandWorld(
		size_t width,
		size_t height,
		double scale)
		: m_width (width  / scale)
		, m_height(height / scale)
		, m_scale(scale)
	{
		m_cells = new Cell[m_width * m_height];
	}

	~SandWorld() {
		delete[] m_cells;
	}

	// Getting cells

	const Cell& GetCell(size_t index)       { return m_cells[index]; }
	const Cell& GetCell(size_t x, size_t y) { return GetCell(GetIndex(x, y)); }

	size_t GetIndex(size_t x, size_t y) { return x + y * m_width; }

	// Helpers

	bool InBounds(size_t x, size_t y) { return x < m_width && y < m_height; }
	bool IsEmpty (size_t x, size_t y) { return InBounds(x, y) && GetCell(x, y).Type == CellType::EMPTY; }

	// Setting & moving cells

	void SetCell(
		size_t x, size_t y,
		const Cell& cell)
	{
		m_cells[GetIndex(x, y)] = cell;
	}

	void MoveCell(
		size_t x,   size_t y,
		size_t xto, size_t yto)
	{
		m_changes.emplace_back(
			GetIndex(xto, yto),
			GetIndex(x,   y)
		);
	}

	void CommitCells() {
		// remove moves that have their destinations filled

		for (size_t i = 0; i < m_changes.size(); i++) {
			if (m_cells[m_changes[i].first].Type != CellType::EMPTY) {
				m_changes[i] = m_changes.back(); m_changes.pop_back();
				i--;
			}
		}

		// sort by destination

		std::sort(m_changes.begin(), m_changes.end(),
			[](auto& a, auto& b) { return a.first < b.first; }
		);


		// pick random source for each destination

		size_t iprev = 0;

		m_changes.emplace_back(-1, -1); // to catch final move

		for (size_t i = 0; i < m_changes.size(); i++) {
			if (m_changes[i + 1].first != m_changes[i].first) {
				size_t rand = iprev + iw::randi(i - iprev);

				size_t dst = m_changes[rand].first;
				size_t src = m_changes[rand].second;

				m_cells[dst] = m_cells[src];
				m_cells[src] = Cell();

				iprev = i + 1;
			}
		}

		m_changes.clear();
	}
};
