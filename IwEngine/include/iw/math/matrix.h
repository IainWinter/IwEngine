#pragma once

#include "iwmath.h"
#include "vector.h"
#include <initializer_list>

namespace iw {
namespace math {
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
			REFLECT _t components[_cols*_rows];
			vector_c   columns   [_cols];
		};

		matrix(
			_t diag = 1)
		{
			reset(diag);
		}

		matrix(
			_t elements[_cols*_rows])
		{
			for(size_t i = 0; i < size; i++) {
				components[i] = elements[i];
			}
		}

		matrix(
			std::initializer_list<_t> list)
		{
			assert(list.size() == size, "Must provide correct number of components to matrix constructor");

			for (auto itr = list.begin(); itr != list.end(); itr++) {
				size_t i = std::distance(list.begin(), itr);
				size_t index = (i % cols) * rows + i / cols; // todo: use this for inplace transpose, converts from row major to column major

				components[index] = *itr;
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
			for (size_t i = 0; i < cols * _rows; i++) {
				components[i] = _t();
			}

			for (size_t i = 0; i < cols; i++) {
				columns[i][i] = diag;
			}
		}

		_t determinant() const { // https://www.youtube.com/watch?v=BX3Kq6-nQps
			static_assert(cols == rows, "Determinant requires square matrix");

			_t det = 0;
			if constexpr (cols == 1) {
				det = components[0];
			}

			else if constexpr (cols == 2) {
				det = components[0] * components[3]
				    - components[1] * components[2];
			}

			else { // else is needed for matrix compile
				int sign = 1;
				for (size_t i = 0; i < cols; i++) {

					matrix<cols - 1, rows - 1> inner;

					size_t q = 0;
					for (size_t c = 0; c < cols; c++) {
						if (c == i) continue;

						for (size_t r = 0; r < inner.rows; r++) {
							inner.columns[q][r] = columns[c][r + 1];
						}
						q++;
					}

					det += sign * columns[i][0] * inner.determinant();
					sign *= -1;
				}
			}

			return det;
		}

		_t trace() const {
			_t result = 0;
			for (size_t i = 0; i < size; i += _cols + 1) {
				result += elements[i];
			}

			return result;
		}

		matrix<rows, cols> transposed() const {
			//if constexpr (_cols == _rows) { // If square, can be done inplace
			//	matrix tmp = *this;
			//	tmp.transpose();
			//	return tmp;
			//}

			matrix<rows, cols> result;

			for (size_t i = 0; i < rows; i++) {
				result.columns[i] = row(i);
			}

			return result;
		}

		matrix inverted() const {
			matrix tmp = *this;
			tmp.invert();
			return tmp;
		}

		matrix normalized() const {
			matrix tmp = *this;
			tmp.normalize();
			return tmp;
		}

		//void transpose() {
		//	static_assert(_cols == _rows);
		//	// todo: impl
		//}

		//void invert() {
		//	// todo: impl
		//}

		void normalize() {
			_t det = determinant();
			if (almost_equal(det, 0, 6)) {
				return;
			}
			*this /= det;
		}

		template<
			size_t _c = cols>
		matrix<_c, rows> operator*(
			const matrix<_c, cols>& other) const
		{
			matrix<_c, rows> result;
			for (size_t c = 0; c < _c;    c++)
			for (size_t r = 0; r < cols; r++) {
				result.columns[c][r] = row(r).dot(other.columns[c]);
			}
			return result;
		}

		vector_r operator*(
			const vector_c& other) const
		{
			vector_r result;
			for (size_t i = 0; i < cols; i++) {
				result.components[i] = other.dot(columns[i]);
			}
			return result;
		}

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
	};

}

using namespace math;
}
