#pragma once

#include "Cell.h"
#include <vector>
#include <mutex>
#include <algorithm>
#include <functional>
#include "iw/util/enum/val.h"

IW_PLUGIN_SAND_BEGIN

enum class SandField : unsigned { // Should allow user to add to this, see enum { #include "fields.h" }; idea from valve
	CELL,
	SOLID, // For tile textures making it so other cells know about tiles
	COLOR  // For tile textures
};

// for platformer other game
//CELL,
//SOLID,
//COLLISION,

class SandChunk;

using onSetCell_func = std::function<void(SandChunk*, size_t, int, int, void*)>; // would be nice to have type info here

class SandChunk {
public:
	struct Field {
		void* cells;
		std::mutex* locks;

		onSetCell_func onSetCell;
		size_t filledCellCount;

		template<typename _t> _t* GetCells()            { return (_t*)cells; }
		template<typename _t> _t& GetCell(size_t index) { return GetCells<_t>()[index]; }
	};

	std::vector<Field> m_fields;

	const int m_width, m_height;
	const int m_x, m_y;

	size_t m_filledCellCount;

	int m_minX, m_minY,
	    m_maxX, m_maxY; // Dirty rect

	bool m_deleteMe; // delays deletion for processing chunk with 0 filled cells

private:
	std::vector<std::tuple<SandChunk*, size_t, size_t>> m_changes; // source chunk, source, destination

	std::mutex m_filledCellCountMutex;
	std::mutex m_changesMutex;
	std::mutex m_workingRectMutex;

	int m_minXw, m_minYw,
	    m_maxXw, m_maxYw; // working dirty rect

public:
	IW_PLUGIN_SAND_API SandChunk(int width, int height, int x, int y);

	// Getting cells

	template<typename _t = Cell>
	_t& GetCell(
		int x, int y,
		SandField field = SandField::CELL)
	{
		return GetCell<_t>(GetIndex(x, y), field);
	}

	template<typename _t = Cell>
	_t& GetCell(
		size_t index,
		SandField field = SandField::CELL)
	{
		return GetField(field).GetCell<_t>(index);
	}

	Field& GetField(
		SandField field = SandField::CELL)
	{
		return m_fields[val(field)];
	}

	// Locking cell, field is not required to have locks, maybe put check here or the field is already non compile safe?
	
	std::mutex& GetLock(
		int x, int y,
		SandField field = SandField::CELL)
	{
		return GetLock(GetIndex(x, y), field);
	}

	std::mutex& GetLock(
		size_t index,
		SandField field = SandField::CELL)
	{
		return m_fields[val(field)].locks[index]; // named from &&?
	}

	//void UnlockCell(int x, int y, SandField field = SandField::CELL) { UnlockCell(GetIndex(x, y), field); }

	//void UnlockCell(size_t index, SandField field = SandField::CELL) { m_fields[field].locks[index].unlock(); }

	void AddField(
		void* field,
		std::mutex* locks = nullptr,
		onSetCell_func& func = onSetCell_func())
	{
		m_fields.push_back({ field, locks, func, 0u });
	}

	// Setting & moving cells

	//IW_PLUGIN_SAND_API void SetCell(int x, int y, const Cell& cell);
	//IW_PLUGIN_SAND_API void SetCell(size_t index, const Cell& cell);

	template<typename _t = Cell>
	void SetCell(
		int x, int y,
		const _t& cell,
		SandField field = SandField::CELL)
	{
		SetCell<_t>(GetIndex(x, y), cell, field);
	}

	template<typename _t = Cell>
	void SetCell(
		size_t index,
		const _t& cell,
		SandField fieldid = SandField::CELL)
	{
		KeepAlive(index);

		Field& field = GetField(fieldid);

		//if (field.locks) {
		//	field.locks[index].lock();
		//}

		_t& dest = field.GetCell<_t>(index);
		_t default = _t();

		if (    dest == default
			&& cell != default) // Filling a cell
		{
			std::unique_lock lock(m_filledCellCountMutex);
			field.filledCellCount++;
			m_filledCellCount++;
		}

		else
		if (    dest != default
			&& cell == default) // Removing a filled cell
		{
			std::unique_lock lock(m_filledCellCountMutex);
			field.filledCellCount--;
			m_filledCellCount--;
		}

		dest = cell;

		int x = index % m_width;
		int y = index / m_width;

		if (field.onSetCell) {
			field.onSetCell(this, index, x, y, (void*)&dest);
		}

		//if (field.locks) {
		//	field.locks[index].unlock();
		//}
	}

	//IW_PLUGIN_SAND_API void SwapBuffers();

	IW_PLUGIN_SAND_API void MoveCell(SandChunk* source, int x, int y, int xto, int yto);
	//IW_PLUGIN_SAND_API void PushCell(SandChunk* source, int x, int y, int xto, int yto);

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
