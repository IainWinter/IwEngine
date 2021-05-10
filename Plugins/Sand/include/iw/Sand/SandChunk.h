#pragma once

#include "Cell.h"
#include <vector>
#include <mutex>
#include <algorithm>

IW_PLUGIN_SAND_BEGIN

class SandChunk {
public:
	// First field is always Cells
	// Next field can be anything?

	// Type& t =  GetField<Type>(index i, int x, int y)

	// In world could have map of type_id and index

	// m_world.GetField<Type>(int x, int y)
	//     -> GetChunk(x, y)->GetField<Type>(typeToIndex[typeid], x, y)

	// m_world.AddField<Type>()
	//     -> 

	//Cell* m_cells;

	struct Field {
		void* cells;
		std::mutex* locks;
		size_t filledCellCount;
	};

	template<typename _t>
	struct Field_t {
		_t* cells;
		std::mutex* locks;
		size_t filledCellCount;

		Field_t(
			Field& f
		)
			: cells((_t*)f.cells)
			, locks(f.locks)
			, filledCellCount(filledCellCount)
		{}
	};

	std::vector<Field> m_fields;

	const int m_width, m_height;
	const int m_x, m_y;

	size_t m_filledCellCount;

	int m_minX, m_minY,
	    m_maxX, m_maxY; // Dirty rect

private:
	std::vector<std::tuple<SandChunk*, size_t, size_t>> m_changes; // source chunk, source, destination

	std::mutex m_filledCellCountMutex;
	std::mutex m_changesMutex;
	std::mutex m_workingRectMutex;

	int m_minXw, m_minYw,
	    m_maxXw, m_maxYw; // working dirty rect

public:
	IW_PLUGIN_SAND_API SandChunk(int width, int height, int x, int y);

	~SandChunk() {
		for (Field& field : m_fields) {
			delete[] field.locks;
		}
	}

	// Getting cells

	//IW_PLUGIN_SAND_API Cell& GetCell(int x, int y);
	//IW_PLUGIN_SAND_API Cell& GetCell(size_t index);

	template<typename _t = Cell>
	_t& GetCell(
		int x, int y,
		size_t field = 0)
	{
		return GetCell<_t>(GetIndex(x, y), field);
	}

	template<typename _t = Cell>
	_t& GetCell(
		size_t index,
		size_t field = 0)
	{
		return GetField<_t>(field).cells[index];
	}

	template<typename _t = Cell>
	Field_t<_t> GetField(
		size_t index = 0)
	{
		return Field_t<_t>(m_fields[index]); /* { (_t*).cells, };*/
	}

	// Locking cell, field is not required to have locks, maybe put check here or the field is already non compile safe?
	
	std::mutex& GetLock(
		int x, int y,
		size_t field = 0)
	{
		return GetLock(GetIndex(x, y), field);
	}

	std::mutex& GetLock(
		size_t index,
		size_t field = 0)
	{
		return m_fields[field].locks[index]; // named from &&?
	}

	//void UnlockCell(int x, int y, size_t field = 0) { UnlockCell(GetIndex(x, y), field); }

	//void UnlockCell(size_t index, size_t field = 0) { m_fields[field].locks[index].unlock(); }

	void AddField(
		void* field,
		std::mutex* locks = nullptr)
	{
		m_fields.push_back({ field, locks, 0u });
	}

	// Setting & moving cells

	//IW_PLUGIN_SAND_API void SetCell(int x, int y, const Cell& cell);
	//IW_PLUGIN_SAND_API void SetCell(size_t index, const Cell& cell);

	template<typename _t = Cell>
	void SetCell(
		int x, int y,
		const Cell& cell,
		size_t field = 0)
	{
		SetCell<_t>(GetIndex(x, y), cell, field);
	}

	template<typename _t = Cell>
	void SetCell(
		size_t index,
		const Cell& cell,
		size_t field = 0)
	{
		KeepAlive(index);

		Cell& dest = GetCell(index, field);// m_cells[index];

		if (    dest.Type == CellType::EMPTY
			&& cell.Type != CellType::EMPTY) // Filling a cell
		{
			std::unique_lock lock(m_filledCellCountMutex);
			m_fields[field].filledCellCount++;
			m_filledCellCount++;
		}

		else
		if (    dest.Type != CellType::EMPTY
			&& cell.Type == CellType::EMPTY) // Removing a filled cell
		{
			std::unique_lock lock(m_filledCellCountMutex);
			m_fields[field].filledCellCount--;
			m_filledCellCount--;
		}

		dest = cell;

		// set location if its not set, this is a hack

		if (dest.x == 0 && dest.y == 0) {
			dest.x = m_x + index % m_width;
			dest.y = m_y + index / m_width;
		}

		// set location everytime it changed whole number

		dest.x = float(dest.x - int(dest.x)) + index % m_width + m_x;
		dest.y = float(dest.y - int(dest.y)) + index / m_width + m_y;
	}

	//IW_PLUGIN_SAND_API void SwapBuffers();

	IW_PLUGIN_SAND_API void MoveCell(SandChunk* source, int x, int y, int xto, int yto);
	IW_PLUGIN_SAND_API void PushCell(SandChunk* source, int x, int y, int xto, int yto);

	IW_PLUGIN_SAND_API void CommitCells();

	// Helpers

	IW_PLUGIN_SAND_API size_t GetIndex(int x, int y);

	IW_PLUGIN_SAND_API bool InBounds(int x, int y);
	IW_PLUGIN_SAND_API bool IsEmpty (int x, int y);

	IW_PLUGIN_SAND_API void KeepAlive(int x, int y);
	IW_PLUGIN_SAND_API void KeepAlive(size_t index);

	IW_PLUGIN_SAND_API void UpdateRect();
};

IW_PLUGIN_SAND_END
