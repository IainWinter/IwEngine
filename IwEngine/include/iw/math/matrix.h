#pragma once

#include "iwmath.h"
#include <assert.h>
#include <ostream>
#include <sstream>

namespace iw {
namespace math {
	template<typename _t>
	struct matitr {
		_t* element;
		int increment;
		int length;

		matitr(
			_t* current,
			int increment,
			int length
		)
			: element(current)
			, increment(increment)
			, length(length)
		{}

		matitr(
			const matitr<std::remove_const_t<_t>>& toconst
		)
			: element(toconst.element)
			, increment(toconst.increment)
			, length(toconst.length)
		{}

		matitr& operator++() { element += increment; return *this; }

		bool operator>(const matitr& other) const { return element > other.element; }
		bool operator<(const matitr& other) const { return element < other.element; }

		      _t& operator*()       { return *element; }
		const _t& operator*() const { return *element; }
	};

	struct matrix {
		using itr       = matitr<float>;
		using itr_const = matitr<const float>;

		float* elements; // height (column) major 
		//  0 3 6
		//  1 4 7
		//  2 5 8

		size_t height = 1;
		size_t width = 1;
		size_t size = 1;

		bool diagonal = false;

		matrix()
			: height(1)
			, width (1)
			, size(1)
		{
			elements = new float[1];
			elements[0] = 0;
		}

		matrix(
			size_t height, size_t width,
			float diag = 0
		)
			: height(height)
			, width (width)
			, size  (height * width)
		{
			elements = new float[size];
			reset(diag);
		}

		matrix(
			const matrix& copy
		)
			: height(copy.height)
			, width (copy.width)
			, size  (copy.height * copy.width)
		{
			elements = new float[size];
			for (size_t i = 0; i < size; i++) elements[i] = copy.elements[i];
		}

		matrix(
			matrix&& move
		) noexcept
			: height(move.height)
			, width (move.width)
			, size  (move.height * move.width)
			, elements(move.elements)
		{
			move.elements = nullptr;
		}

		matrix& operator=(
			const matrix& copy)
		{
			height = copy.height;
			width  = copy.width;
			size   = copy.height * copy.width;

			delete[] elements;

			elements = new float[size];
			for (size_t i = 0; i < size; i++) elements[i] = copy.elements[i];

			return *this;
		}

		matrix& operator=(
			matrix&& move) noexcept
		{
			height = move.height;
			width  = move.width;
			size   = move.height * move.width;
			elements = move.elements;

			move.elements = nullptr;

			return *this;
		}

		~matrix() {
			delete[] elements;
		}

		void reset(
			float diag)
		{
			for (size_t i = 0; i < size; i++)             elements[i] = 0;
			for (size_t i = 0; i < size; i += height + 1) elements[i] = diag; // if diag should set whole column
		}

		// getting and setting

		inline       float& get(size_t x, size_t y)       { assert(getIndex(x, y) < size); return elements[getIndex(x, y)]; }
		inline const float& get(size_t x, size_t y) const { assert(getIndex(x, y) < size); return elements[getIndex(x, y)]; }

		inline       float& get(size_t y)       { assert(y < size); return elements[y]; }
		inline const float& get(size_t y) const { assert(y < size); return elements[y]; }

		void tset(size_t x, size_t y, float f) { tresize(x,     y);     set(x, y, f); }
		void tset(          size_t y, float f) { tresize(width, y + 1); set(   y, f); }

		inline void set(size_t x, size_t y, float f) { assert(getIndex(x, y) < size); elements[getIndex(x, y)] = f; }
		inline void set(          size_t y, float f) { assert(y < size);              elements[y]              = f; }

		inline size_t getIndex(
			size_t x, size_t y) const
		{
			return y + x * height;
		}

		void insert(
			int x, int y,
			const matrix& m)
		{
			if (x + m.width > width || y + m.height > height) {
				expand(y + m.height, x + m.width);
			}

			for (size_t j = 0; j < m.width;  j++) 
			for (size_t i = 0; i < m.height; i++) {
				set(x + j, y + i, m.get(j, i));
			}
		}

		void insertDiagonals(
			int x,
			const matrix& m)
		{
			if (x + m.width > width || x + m.height > height) {
				expand(x + m.height, x + m.width);
			}

			for (size_t i = 0; i < m.height; i++) {
				set(x + i, x + i, m.get(i, i));
			}
		}

		matrix operator*(
			const matrix& other) const // this functions flow could def be made cleaner
		{
			if (diagonal && other.diagonal)
			{
				assert(height == other.height);

				matrix result(height, 1); // diag*diag is just multiply between vectors

				for (size_t i = 0; i < result.height; i++)
				{
					result.set(i, get(i) * other.get(i));
				}

				return result;
			}

			if (!diagonal && other.diagonal) // scale cols
			{
				assert(width == other.height);

				matrix result(height, other.height); // use other.height because width = '1', but is really height

				for (size_t j = 0; j < result.width;  j++) 
				for (size_t i = 0; i < result.height; i++)
				{
					result.set(j, i, get(j, i) * other.get(j));
				}

				return result;
			}

			matrix result(height, other.width);

			if (diagonal && !other.diagonal) // scale rows
			{
				assert(height == other.height);

				for (size_t j = 0; j < result.width;  j++)
				for (size_t i = 0; i < result.height; i++)
				{
					result.set(j, i, get(i) * other.get(j, i));
				}
			}

			// regular multiply

			else {
				assert(width == other.height);

				for (size_t j = 0; j < result.width;  j++)
				for (size_t i = 0; i < result.height; i++)
				{
					float dot = 0.0f;

					for (size_t k = 0; k < width; k++) // cant unrol maybe wrap matrix in a templateed subclass
					{ 
						dot += get(k, i) * other.get(j, k);
					}

					result.set(j, i, dot);
				}
			}

			return result;
		}

		matrix operator/(
			const matrix& other) const
		{
			assert(other.height == height && other.width == width);

			matrix result(height, width);

			for (size_t i = 0; i < size; i++) {
				result.elements[i] = elements[i] / other.elements[i];
			}

			return result;
		}

		matrix operator+(
			const matrix& other) const
		{
			assert(other.height == height && other.width == width);

			matrix result(height, width);

			for (size_t i = 0; i < size; i++) {
				result.elements[i] = elements[i] + other.elements[i];
			}

			return result;
		}

		matrix operator-(
			const matrix& other) const
		{
			return operator+(-other);
		}

		matrix operator*(
			float x) const
		{
			matrix result(height, width);

			for (size_t i = 0; i < size; i++) {
				result.elements[i] = elements[i] * x;
			}

			return result;
		}

		matrix operator/(
			float x) const
		{
			return operator*(1 / x);
		}

		matrix operator+(
			float x) const
		{
			matrix result(height, width);

			for (size_t i = 0; i < size; i++) {
				result.elements[i] = elements[i] + x;
			}

			return result;
		}

		matrix operator-(
			float x) const
		{
			return operator+(-x);
		}

		matrix operator-() const {
			matrix result(height, width);

			for (size_t i = 0; i < size; i++) {
				result.elements[i] = -elements[i];
			}

			return result;
		}

		// changing size

		void tresize(
			size_t x, size_t y)
		{
			if (x >= width || y >= height) {
				resize(x, y);
			}
		}

		matrix resize(
			size_t newHeight, size_t newWidth = 1)
		{
			matrix newMat(newHeight, newWidth);

			size_t minWidth  = newWidth  > width  ? width  : newWidth;
			size_t minHeight = newHeight > height ? height : newHeight;

			for (size_t j = 0; j < minWidth;  j++) 
			for (size_t i = 0; i < minHeight; i++) {
				newMat.set(j, i, get(j, i));
			}

			*this = newMat;

			return *this;
		}

		void expand(
			size_t newHeight, size_t newWidth = 1)
		{
			resize(newHeight > height ? newHeight : height,
				  newWidth  > width  ? newWidth  : width);
		}

		// helpers

		size_t rowToColumn(
			size_t index) const
		{
			return (index % width) * height + index / width;
		}

		// printing

		friend std::ostream& operator<<(
			std::ostream& ostream,
			const matrix& mat)
		{
			size_t maxLength = 0;

			for (size_t i = 0; i < mat.size; i++) {
				std::stringstream sb;
				sb << mat.elements[i];

				size_t length = sb.str().length();
				if (maxLength < length) {
					maxLength = length;
				}
			}

			ostream << "[";
			for (	size_t r = 0; r < mat.height; r++) {
				for (size_t c = 0; c < mat.width; c++) {
					if (c == 0 && r != 0) {
						ostream << " ";
					}

					std::stringstream sb;
					sb << mat.get(c, r);

					std::string str = sb.str();

					ostream << str;

					if (c < mat.width - 1) {
						for (size_t i = str.length(); i <= maxLength; i++) {
							ostream << ' ';
						}
					}
				}

				if (r < mat.height - 1) {
					ostream << "\n";
				}
			}
			return ostream << "]";
		}

		itr row_begin(size_t row) { return   itr(&get(0,         row), int(height), int(width)); }
		itr row_end  (size_t row) { return ++itr(&get(width - 1, row), int(height), int(width)); }

		itr col_begin(size_t col) { return   itr(&get(col,          0), 1, int(height)); }
		itr col_end  (size_t col) { return ++itr(&get(col, height - 1), 1, int(height)); }

		itr_const row_begin(size_t row) const { return   itr_const(&get(0,         row), int(height), int(width)); }
		itr_const row_end  (size_t row) const { return ++itr_const(&get(width - 1, row), int(height), int(width)); }

		itr_const col_begin(size_t col) const { return   itr_const(&get(col,          0), 1, int(height)); }
		itr_const col_end  (size_t col) const { return ++itr_const(&get(col, height - 1), 1, int(height)); }
	};

	template<
		size_t _height, size_t _width>
	struct mat
		: matrix
	{
		mat(
			float diag = 0
		)
			: matrix(_height, _width, diag)
		{}

		mat(const matrix& copy) { // should use a #define for these (name, height, width)
			assert(copy.height == _height && copy.width == _width);
			*(matrix*)this = copy;
		}
	};

	template<
		size_t _height>
	struct vec
		: matrix
	{
		vec(
			float diag = 0
		)
			: matrix(_height, 1, diag)
		{}

		vec(const matrix& copy) {
			assert(copy.height == _height && copy.width == 1);
			*(matrix*)this = copy;
		}
	};

	struct vec2 : vec<2> {
		vec2() : vec<2>() {}

		vec2(float xy) : vec<2>() {
			set(0, xy); set(1, xy);
		}

		vec2(float x, float y) : vec<2>() {
			set(0, x); set(1, y);
		}

		vec2(const matrix& copy) {
			assert(copy.height == 2 && copy.width == 1);
			*(matrix*)this = copy;
		}

		inline float& x() { return get(0); }
		inline float& y() { return get(1); }
	};

	struct vec3 : vec<3> {
		vec3() : vec<3>() {}

		vec3(float xyz) : vec<3>() {
			set(0, xyz); set(1, xyz); set(2, xyz);
		}

		vec3(float x, float y, float z) : vec<3>() {
			set(0, x); set(1, y); set(2, z);
		}

		vec3(const matrix& copy) {
			assert(copy.height == 3 && copy.width == 1);
			*(matrix*)this = copy;
		}

		inline float& x() { return get(0); }
		inline float& y() { return get(1); }
		inline float& z() { return get(2); }
	};

	struct vec4 : vec<4> {
		vec4() : vec<4>() {}

		vec4(float xyzw) : vec<4>() {
			set(0, xyzw); set(1, xyzw); set(2, xyzw); set(3, xyzw);
		}

		vec4(float x, float y, float z, float w) : vec<4>() {
			set(0, x); set(1, y); set(2, z); set(3, w);
		}

		vec4(const matrix& copy) {
			assert(copy.height == 4 && copy.width == 1);
			*(matrix*)this = copy;
		}

		inline float& x() { return get(0); }
		inline float& y() { return get(1); }
		inline float& z() { return get(2); }
		inline float& w() { return get(3); }
	};

	// math functions

	inline float dot(
		matrix::itr_const a, matrix::itr_const b)
	{
		assert(a.length == b.length);

		float result = 0.0f;

		for (int i = 0; i < a.length; i++)
		{
			result += *a * *b;
			++a;
			++b;
		}

		return result;
	}

	inline matrix transpose(
		const matrix& mat)
	{
		if (mat.diagonal) return mat;

		matrix result(mat.width, mat.height);

		for (size_t i = 0; i < mat.height; i++)
		{
			matrix::itr_const row = mat.row_begin(i);
			matrix::itr_const end = mat.row_end  (i);

			matrix::itr col = result.col_begin(i);

			for (; row < end; ++row, ++col) {
				*col = *row;
			}
		}

		return result;
	}

	inline matrix normalize(
		const matrix& mat)
	{
		float length = 0.0f;

		for (size_t i = 0; i < mat.size; i++) {
			length += mat.elements[i];
		}

		length /= mat.size;

		matrix result(mat.width, mat.height);

		for (size_t i = 0; i < mat.size; i++) {
			result.elements[i] = mat.elements[i] / length;
		}

		return result;
	}

	inline mat<3, 3> cross_3(
		const vec<3>& vec3)
	{
		assert(vec3.height == 3 && vec3.width == 1);

		mat<3, 3> result;

		float x = vec3.get(0);
		float y = vec3.get(1);
		float z = vec3.get(2);

		result.set(1, 0,  z);
		result.set(0, 1, -z);
		result.set(2, 0, -y);
		result.set(0, 2,  y);
		result.set(2, 1,  x);
		result.set(1, 2, -x);

		return result;
	}

	template<>
	inline matrix clamp(
		const matrix& mat,
		const matrix& min,
		const matrix& max)
	{
		assert(mat.size == min.size && min.size == max.size);

		matrix result(mat.height, mat.width);

		for (size_t i = 0; i < mat.size; i++) {
			float e = mat.elements[i];
			if (e > max.elements[i]) e = max.elements[i];
			if (e < min.elements[i]) e = min.elements[i];

			result.elements[i] = e;
		}

		return result;
	}
}

	using namespace math;
}
