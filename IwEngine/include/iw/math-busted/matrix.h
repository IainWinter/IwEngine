#pragma once

#include "iwmath.h"
#include <initializer_list>
#include <tuple>
#include <functional>

namespace iw {
namespace math {

	// Mainly for function below matrix<>

#define TEMPLATE_CRF template<size_t _c, size_t _r, typename _t, class F>
#define TEMPLATE_CR template<size_t _c, size_t _r, typename _t>
#define TEMPLATE_N template<size_t _n, typename _t>
#define MAT_CR matrix<_c, _r, _t>
#define MAT_N matrix<_n, _n, _t>
#define VEC_N vector<_n, _t>
#define COMP_P std::pair<_t, size_t>

	TEMPLATE_CRF MAT_CR& each(MAT_CR& matrix, F func);

	template<
		size_t _cols, size_t _rows,
		typename _t = float>
	struct mat
	{
		
	};

	template<
		size_t _cols, size_t _rows,
		typename _t = float>
	struct REFLECT matrix {

		using vector_c = vector<_rows, _t>;
		using vector_r = vector<_cols, _t>;

		static constexpr size_t rows = _rows;
		static constexpr size_t cols = _cols;
		static constexpr size_t size = cols * rows;

		REFLECT union {
			REFLECT _t components[_cols * _rows];
			vector_c   columns   [_cols];         // wish this could be a fixed size
		};

		matrix(
			_t diag = 1)
		{
			reset(diag);
		}

		matrix(
			_t elements[_cols * _rows])
		{
			for (size_t i = 0; i < size; i++) {
				components[i] = elements[i];
			}
		}

		matrix(
			std::initializer_list<_t> list)
		{
			assert(list.size() == size, "Must provide correct number of components to matrix constructor");

			for (auto itr = list.begin(); itr != list.end(); itr++) {
				components[RowToColumn(std::distance(list.begin(), itr))] = *itr;
			}
		}

		vector_r row(
			size_t row) const
		{
			vector_r r;
			for (size_t i = 0; i < cols; i++) {
				r.components[i] = columns[i][row];
			}

			return r;
		}

		void reset(
			_t diag)
		{
			for (size_t i = 0; i < cols * _rows; i++) components[i] = _t();
			for (size_t i = 0; i < cols;         i++) columns[i][i] = diag;
		}

		matrix operator+(
			const matrix& other) const
		{
			matrix result;
			for (size_t i = 0; i < size; i++) {
				result.components[i] = components[i] + other.components[i];
			}

			return result;
		}

		template<
			size_t _c = cols>
		matrix<_c, rows> operator*(
			const matrix<_c, cols>& other) const
		{
			matrix<_c, rows> result;
			for (size_t c = 0; c < _c;   c++)
			for (size_t r = 0; r < cols; r++) {
				//result.columns[c][r] = row(r).dot(other.columns[c]);
			}

			return result;
		}

		vector_r operator*(
			const vector_c& other) const
		{
			vector_r result;
			for (size_t i = 0; i < cols; i++) {
				//result.components[i] = other.dot(columns[i]);
			}

			return result;
		}

		matrix operator+(const _t& x) { return each(MAT_CR(*this), [&](float& a) { a += x; }); }
		matrix operator-(const _t& x) { return each(MAT_CR(*this), [&](float& a) { a -= x; }); }
		matrix operator*(const _t& x) { return each(MAT_CR(*this), [&](float& a) { a *= x; }); }
		matrix operator/(const _t& x) { return each(MAT_CR(*this), [&](float& a) { a /= x; }); }

		matrix& operator+=(const _t& x) { return each(*this, [&](float& a) { a += x; }); }
		matrix& operator-=(const _t& x) { return each(*this, [&](float& a) { a -= x; }); }
		matrix& operator*=(const _t& x) { return each(*this, [&](float& a) { a *= x; }); }
		matrix& operator/=(const _t& x) { return each(*this, [&](float& a) { a /= x; }); }

		      vector_c& operator[](size_t index)       { return columns[index]; }
		const vector_c& operator[](size_t index) const { return columns[index]; }

		friend std::ostream& operator<<(
			std::ostream& ostream,
			const matrix& mat)
		{
			ostream << "[";
			for (	size_t r = 0; r < rows; r++) {
				for (size_t c = 0; c < cols; c++) {
					if (c == 0 && r != 0) {
						ostream << " ";
					}

					ostream << mat.columns[c][r];

					if (c < cols - 1) {
						ostream << " ";
					}
				}

				if (r < rows - 1) {
					ostream << "\n";
				}
			}
			return ostream << "]";
		}

		// Helpers

	private:
		void ReorderPivot(
			size_t columnIndex,
			size_t maxRowIndex = 0)
		{
			for (size_t r = rows - 1; r > maxRowIndex; r--) {
				bool isPivot = columnIndex != r || columns[columnIndex - 1][r] != 0;
				bool greater = columns[columnIndex][r - 1] < columns[columnIndex][r];

				if (isPivot && greater) {
					SwapRow(r - 1, r);
				}
			}
		}

		void SwapRow(
			size_t r1,
			size_t r2)
		{
			for (size_t c = 0; c < cols; c++) {
				float temp = columns[c][r1];
				columns[c][r1] = columns[c][r2];
				columns[c][r2] = temp;
			}
		}

		size_t RowToColumn(size_t index) {
			return (index % cols) * rows + index / cols;
		}
	};

	// Functions

	template<typename _t>
	bool less(const _t& a, const _t& b) { return a < b; }

	template<typename _t>
	bool greater(const _t& a, const _t& b) { return a > b; }

	// Helpers

	TEMPLATE_CRF
	MAT_CR& each(
		MAT_CR& matrix,
		F func)
	{
		for (_t& c : matrix.components) {
			func(c);
		}

		return matrix;
	}

	//	searching for components

	TEMPLATE_CRF
	COMP_P best(
		const MAT_CR& matrix,
		F func)
	{
		_t     result = matrix.components[0];
		size_t index  = 0;

		for (size_t i = 1; i < matrix.size; i++) {
			if (func(matrix.components[i], result)) {
				result = matrix.components[i];
				index  = i;
			}
		}

		return { result, index };
	}

	TEMPLATE_CR COMP_P minor(const MAT_CR& matrix) { return best(matrix, less<_t>); }
	TEMPLATE_CR COMP_P major(const MAT_CR& matrix) { return best(matrix, greater<_t>); }

	//	determinant

	TEMPLATE_N
	_t det(
		const MAT_N& square)
	{
		_t result = 0;

		for (size_t i = 0; i < cols; i++) {

			matrix<_n - 1, _n - 1, _t> inner;

			for (size_t q = 0, c = 0; c < cols; c++) {
				if (c == i) continue;

				for (size_t r = 0; r < inner.rows; r++) {
					inner.columns[q][r] = columns[c][r + 1];
				}

				q++; // only when not continued
			}

			result += (i % 2 == 0 ? 1 : -1) * columns[i][0] * det(inner);
		}

		return result;
	}

	template<typename _t>
	_t det(const matrix<1, 1, _t>& square) {
		return square[0]; }

	template<typename _t>
	_t det(const matrix<2, 2, _t>& square) {
		return square[0] * square[3]
			- square[1] * square[2]; }

	//	transpose

	TEMPLATE_N
	void transpose(
		MAT_N& square)
	{
		size_t from = 1;
		while (from < square.size - 1) {        // Don't need to check first and last corner
			size_t to = square.RowToColumn(from);
			_t t = square.components[from];

			square.components[from] = square.components[to];
			square.components[to]   = t;

			from++;

			if (from % square.rows == 0) {     // Only iterate bottom triangle
				from += from / square.rows + 1;
			}
		}
	}

	TEMPLATE_CR
	MAT_CR transposed(
		const MAT_CR& matrix)
	{
		MAT_CR result;

		for (size_t i = 0; i < _c; i++) {
			result.columns[i] = matrix.row(i);
		}

		return result;
	}

	//	inverse

	TEMPLATE_N
	void invert(
		MAT_N& square)
	{
		matrix<_n * 2, _n, _t> mat(0.0f);

		// Append identity matrix to right half

		for (size_t c = 0; c < _n; c++) mat.columns[c] = square.columns[c];
		for (size_t c = 0; c < _n; c++) mat.columns[c + _n][c] = 1;

		/********** reducing to diagonal  matrix ***********/

		for (int i = 0; i < _n; i++)
		for (int j = 0; j < _n; j++) {
			if (j == i) continue;

			if (mat[i][i] == 0) {
				mat.ReorderPivot(i, j);
			}

			float d = mat[i][j] / mat[i][i];
        
			for (int k = 0; k < _n * 2; k++) {
				mat[k][j] -= mat[k][i] * d;
			}
		}

		/************** reducing to unit matrix *************/

		for (int i = 0; i < _n; i++) {
			float d = mat[i][i];
        
			for (int j = 0; j < _n * 2; j++) {
				mat[j][i] = mat[j][i] / d;
			}
		}

		// Copy from back half

		for (size_t c = 0; c < _n; c++) {
			square.columns[c] = mat.columns[c + _n];
		}
	}

	TEMPLATE_N
	MAT_N inverted(
		const MAT_N& square)
	{
		MAT_N result = square;
		invert(result);
		return result;
	}

	//	normalize

	TEMPLATE_N void normalize(MAT_N& matrix) { matrix /= det(matrix); }
	TEMPLATE_N void normalize(VEC_N& vector) { vector /= length(vector); }

	TEMPLATE_N MAT_N normalized(MAT_N& matrix) { return matrix / det(matrix); }
	TEMPLATE_N VEC_N normalized(VEC_N& vector) { return vector / length(vector); }

	//	length

	TEMPLATE_N
	_t length_sqr(
		const VEC_N& vector)
	{
		_t result = 0;

		for (const _t& c : vector.components) {
			result += c * c;
		}

		return result;
	}

	TEMPLATE_N _t length(const VEC_N& vector) { return sqrt(length_sqr(vector)); }

	// could have trace, rename to scale_part or something along with having a pos/rot part

	matrix<3, 3> Skew(vector<3>& v) {
		auto [x, y, z] = v.components;

		return {
			 0, -z,  y,
			 z,  0, -x,
			-y,  x,  0
		};
	}

#undef TEMPLATE_CRF
#undef TEMPLATE_CR
#undef TEMPLATE_N
#undef MAT_CR
#undef MAT_N
#undef VEC_N
#undef COMP_P
}

using namespace math;
}
