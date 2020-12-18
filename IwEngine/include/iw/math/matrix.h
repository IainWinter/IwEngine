#pragma once

#include "iwmath.h"
#include "vector.h"

namespace iw {
namespace math {
	template<
		size_t _cols, size_t _rows,
		typename _t = float>
	struct REFLECT matrix {
		using vector_c = vector<_rows, _t>;
		using vector_r = vector<_cols, _t>;

		//static const matrix identity;

		REFLECT union {
			REFLECT _t components[_cols*_rows];
			vector_c   columns   [_cols];
		};

		matrix(
			_t diag = 1)
		{
			reset(diag);
		}

		//matrix(
		//	vector_c columns...)
		//{
		//	reset(0);
		//	// todo: impl
		//};

		//matrix() from other matrix

		vector_r row(
			size_t row) const
		{
			vector_r r;
			for (size_t i = 0; i < _cols; i++) {
				r.components[i] = columns[i][row];
			}
			return r;
		}

		void reset(
			_t diag)
		{
			memset(components, 0, _cols*_rows*sizeof(_t));
			for (size_t i = 0; i < _cols; i++) {
				columns[i][i] = diag;
			}
		}

		_t determinant() const {
			_t det = 0; // todo: impl https://www.mathsisfun.com/algebra/matrix-determinant.html
		 //	for (size_t i = 0; i < _count; i += _cols) {
			//	det += elements[]
			//}
			return det;
		}

		_t trace() const {
			_t trace = 0;
			for (size_t i = 0; i < _count; i += _cols + 1) {
				det += elements[i];
			}
			return trace;
		}

		matrix transposed() const {
			matrix tmp = *this;
			tmp.transpose();
			return tmp;
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

		void transpose() {
			// todo: impl
		}

		void invert() {
			// todo: impl
		}

		void normalize() {
			_t det = determinant();
			if (almost_equal(det, 0, 6)) {
				return;
			}

			*this /= det;
		}

		template<
			size_t _c = _cols>
		matrix<_c, _rows> operator*(
			const matrix<_c, _cols>& other) const
		{
			matrix<_c, _rows> total;
			for (size_t c = 0; c < _c;    c++)
			for (size_t r = 0; r < _cols; r++) {
				total.columns[c][r] = row(r).dot(other.columns[c]);
			}
			return total;
		}

		vector_r operator*(
			const vector_c& other) const
		{
			vector_r total;
			for (size_t i = 0; i < _cols; i++) {
				total.components[i] = other.dot(columns[i]);
			}
			return total;
		}

		//vector_r operator*(
		//	const vector_c& other) const
		//{
		//	vector_r total;
		//	for (size_t i = 0; i < _cols; i++) {
		//		total.components[i] = other.dot(columns[i]);
		//	}
		//	return total;
		//}

		friend std::ostream& operator<<(
			std::ostream& ostream,
			const matrix& mat)
		{
			ostream << "[";
			for (	size_t r = 0; r < _rows; r++) {
				for (size_t c = 0; c < _cols; c++) {
					if (c == 0 && r != 0) {
						ostream << " ";
					}

					ostream << mat.columns[c][r];

					if (c < _cols - 1) {
						ostream << " ";
					}
				}

				if (r < _rows - 1) {
					ostream << "\n";
				}
			}
			return ostream << "]";
		}
	};

}

using namespace math;
}
