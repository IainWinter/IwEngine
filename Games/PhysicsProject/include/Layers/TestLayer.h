#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/PointLight.h"

#include "glm/vec3.hpp"

#include <vector>
#include <set>

namespace iw {

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

		matrix(
			size_t height = 1, size_t width = 1,
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
		)
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
			matrix&& move)
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
			for (size_t i = 0; i < size; i += height + 1) elements[i] = diag;
		}

		// getting and setting

		inline       float& get(size_t x, size_t y)       { assert(getIndex(x, y) < size); return elements[getIndex(x, y)]; }
		inline const float& get(size_t x, size_t y) const { assert(getIndex(x, y) < size); return elements[getIndex(x, y)]; }

		inline       float& get(size_t y)       { assert(y < size); return elements[y]; }
		inline const float& get(size_t y) const { assert(y < size); return elements[y]; }

		void tset(size_t x, size_t y, float f) { tresize(x, y);         set(x, y, f); }
		void tset(          size_t y, float f) { tresize(y + 1, width); set(   y, f); }

		inline void set(size_t x, size_t y, float f) { assert(getIndex(x, y) < size); elements[getIndex(x, y)] = f; }
		inline void set(          size_t y, float f) { assert(y < size); elements[y] = f; }

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

				matrix result(height); // diag*diag is just multiply between vectors

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

		matrix operator-() {
			matrix result = *this;

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

		itr row_begin(size_t row) { return   itr(&get(0,         row), height, width); }
		itr row_end  (size_t row) { return ++itr(&get(width - 1, row), height, width); }

		itr col_begin(size_t col) { return   itr(&get(col,          0), 1, height); }
		itr col_end  (size_t col) { return ++itr(&get(col, height - 1), 1, height); }

		itr_const row_begin(size_t row) const { return   itr_const(&get(0,         row), height, width); }
		itr_const row_end  (size_t row) const { return ++itr_const(&get(width - 1, row), height, width); }

		itr_const col_begin(size_t col) const { return   itr_const(&get(col,          0), 1, height); }
		itr_const col_end  (size_t col) const { return ++itr_const(&get(col, height - 1), 1, height); }
	};

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

	inline matrix transpose(const matrix& mat) {
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

	inline matrix from_glm_vec3(glm::vec3 vec) {
		matrix result(3);

		result.set(0, vec.x);
		result.set(1, vec.y);
		result.set(2, vec.z);

		return result;
	}

	inline matrix cross_3(matrix& vec3) {
		assert(vec3.height == 3 && vec3.width == 1);

		matrix result(3, 3);

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

	struct Constraint {
		Rigidbody* A;
		Rigidbody* B;

		matrix J_A;
		matrix J_B;

		virtual void init() = 0;

		bool contains(Rigidbody* r) { return A == r || B == r; }

		matrix getJ(Rigidbody* r) {
			if (r == A) return J_A;
			if (r == B) return J_B;
			assert("Help");
		}
	};

	struct VelocityConstraint : Constraint {

		void init() override {
			// J = [J_A J_B]
			// J = [id -cross(a) -id cross(b)]

			J_A.insert(0, 0, matrix(3, 3,  1));
			J_B.insert(0, 0, matrix(3, 3, -1));

			glm::vec3 ra = A->Trans().Rotation * (A->Trans().Scale * glm::vec3(1));
			glm::vec3 rb = B->Trans().Rotation * (B->Trans().Scale * glm::vec3(1));

			J_A.insert(3, 0, cross_3(from_glm_vec3( ra)));
			J_B.insert(3, 0, cross_3(from_glm_vec3(-rb)));
		}

	};

	struct Mechanism {

		std::vector<Constraint*> Constraints;
		std::set<iw::Rigidbody*> Bodies;

		matrix J; // Jacobian
		matrix W; // Inverse mass matrix

		matrix K; // JWJt
		matrix R; // -J * v0

		matrix v0; // Initial vel
		//matrix dV; // Change from last frame

		void init(/*
			matrix lastI*/)
		{
			//for (Constraint* c : Constraints) {
			//	Bodies.insert(c->A);
			//	Bodies.insert(c->B);
			//}

			// Create J

			size_t i = 0;
			for (Rigidbody* r : Bodies)
			{
				size_t j = 0;
				for (Constraint* c : Constraints)
				{
					c->init();

					matrix m;
					if (c->contains(r)) m = c->getJ(r);
					else                m = matrix(3, 6);

					J.insert(i * 6, j * 3, m);

					//LOG_INFO << "J:";  LOG_INFO << J;

					j++;
				}

				i++;
			}

			// Create W & v0

			W = matrix(Bodies.size() * 6);
			W.diagonal = true;

			v0 = matrix(Bodies.size() * 6);

			i = 0;
			for (Rigidbody* r : Bodies)
			{
				matrix transformation(4, 4);

				for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++) {
					glm::mat4 t = r->Trans().WorldTransformation();
					transformation.set(i, j, t[j][i]);
				}

				matrix x(4);
				matrix y(4);
				matrix z(4);

				x.set(0, 1);
				y.set(1, 1);
				z.set(2, 1);

				matrix invInrtLocal = matrix(3, 3, r->InvMass);

				matrix X = transformation * x;
				matrix Y = transformation * y;
				matrix Z = transformation * z;

				matrix world2Local(3, 3);

				world2Local.insert(0, 0, X.resize(3));
				world2Local.insert(1, 0, Y.resize(3));
				world2Local.insert(2, 0, Z.resize(3));

				matrix invInrtWorld = world2Local * invInrtLocal * transpose(world2Local);

				W.set(i,     r->InvMass);
				W.set(i + 1, r->InvMass);
				W.set(i + 2, r->InvMass);
						 
				W.set(i + 3, invInrtWorld.get(0, 0));
				W.set(i + 4, invInrtWorld.get(1, 1));
				W.set(i + 5, invInrtWorld.get(2, 2));

				v0.set(i,     r->Velocity.x);
				v0.set(i + 1, r->Velocity.y);
				v0.set(i + 2, r->Velocity.z);

				v0.set(i + 3, r->AngularVelocity.x);
				v0.set(i + 4, r->AngularVelocity.y);
				v0.set(i + 5, r->AngularVelocity.z);

				i += 6;
			}

			// Precompute

			//matrix jt = transpose(J);

			//matrix jw = J * W;
			//matrix jwjt = jw * jt;

			K = J * W * transpose(J);
			R = -J * v0;

			//LOG_INFO << "J:";  LOG_INFO << J;
			//LOG_INFO << "Jt:"; LOG_INFO << transpose(J);
			//LOG_INFO << "W:";  LOG_INFO << W;
			//LOG_INFO << "v0:"; LOG_INFO << v0;
			//LOG_INFO << "K:";  LOG_INFO << K;
			//LOG_INFO << "R:";  LOG_INFO << R;
		}

		void solve() {
			matrix h(K.width);

			for (size_t _ = 0; _ < 50; _++)
			for (size_t i = 0; i < K.height; i++) {
				float dR = R.get(i) - dot(K.row_begin(i), h.col_begin(0));
				h.get(i) += dR / K.get(i, i);
			}

			//LOG_INFO << "lambda:"; LOG_INFO << h;
			//LOG_INFO << "Jt * lambda:"; LOG_INFO << transpose(J) * h;

			matrix impluse = W * transpose(J) * h;

			LOG_INFO << impluse;

			size_t i = 0;
			for (Rigidbody* body : Bodies) {
				body->       Velocity.x += impluse.get(i);
				body->       Velocity.y += impluse.get(i + 1);
				body->       Velocity.z += impluse.get(i + 2);
				body->AngularVelocity.x += impluse.get(i + 3);
				body->AngularVelocity.y += impluse.get(i + 4);
				body->AngularVelocity.z += impluse.get(i + 5);

				i += 6;
			}
		}

				//float dhi = (R.get(i) - dot(J.row_begin(i), dV.col_begin(0))) / K.get(i, i);
				// project hi ?
	};

	inline void testMultiply() {
		matrix m(3, 3);

		m.set(0, 0, 1);
		m.set(1, 0, 2);
		m.set(2, 0, 3);
		m.set(0, 1, 4);
		m.set(1, 1, 5);
		m.set(2, 1, 6);
		m.set(0, 2, 7);
		m.set(1, 2, 8);
		m.set(2, 2, 9);

		matrix md(3); md.diagonal = true;
		md.set(0, 1);
		md.set(1, 2);
		md.set(2, 3);

		matrix mm   = m * m;
		matrix mmd  = m * md;
		matrix mdm  = md * m;
		matrix mdmd = md * md;

		LOG_INFO << "m:";  LOG_INFO << m;
		LOG_INFO << "md:"; LOG_INFO << md;

		LOG_INFO << "mm:";   LOG_INFO << mm;
		LOG_INFO << "mmd:";  LOG_INFO << mmd;
		LOG_INFO << "mdm:";  LOG_INFO << mdm;
		LOG_INFO << "mdmd:"; LOG_INFO << mdmd;
	}

	inline void testTranspose() {
		matrix m(3, 3);

		m.set(0, 0, 1);
		m.set(1, 0, 2);
		m.set(2, 0, 3);
		m.set(0, 1, 4);
		m.set(1, 1, 5);
		m.set(2, 1, 6);
		m.set(0, 2, 7);
		m.set(1, 2, 8);
		m.set(2, 2, 9);

		matrix mt = transpose(m);

		LOG_INFO << "m:";  LOG_INFO << m;
		LOG_INFO << "mt:"; LOG_INFO << mt;
	}

	class TestLayer
		: public Layer
	{
	private:
		ref<Shader> shader;

		ref<RenderTarget> dirShadowTarget;
		ref<RenderTarget> pointShadowTarget;

		MeshData* sphere;
		MeshData* tetrahedron;
		MeshData* cube;

		MeshData* plane;

		Entity Z, A, B, TestDebug;
		Entity Ground;

		Mechanism mechanism;

	public:
		TestLayer();

		Entity SpawnCube(glm::vec3 s = glm::vec3(1), float m = 1, bool locked = false);

		void CorrectVelocity(Entity entity, Entity target, glm::vec3 offset);

		int Initialize() override;
		void FixedUpdate() override;
		void ImGui() override;
	};
}
