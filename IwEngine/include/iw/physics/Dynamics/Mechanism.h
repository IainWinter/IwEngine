#pragma once

#include "Rigidbody.h"
#include "iw/log/logger.h"
#include <set>
#include <initializer_list>

#include "iw/math/matrix.h"

namespace iw {
	struct Constraint {
		Rigidbody* A;
		Rigidbody* B;

		Constraint(
			Rigidbody* a,
			Rigidbody* b
		)
			: A(a)
			, B(b)
		{}

		virtual void solve(float dt) = 0;

		bool contains(Rigidbody* r) {
			return r == A
			    || r == B;
		}

		Rigidbody* other(Rigidbody* r) {
			if (r == A) return B;
			            return A;
		}
	};

	struct VelocityConstraint : Constraint {
		matrix J; // jacobian
		matrix W; // inv mass / inertia

		matrix M; // W * Jt

		matrix K; // constraint space
		matrix P; // saved impluse

		matrix E; // position error
		float biasStrength = .1f; // strength of position correction

		VelocityConstraint(
			Rigidbody* a,
			Rigidbody* b
		)
			: Constraint(a, b)
		{}

		virtual void init(
			float dt)
		{
			W = matrix(12, 1); W.diagonal = true;

			Rigidbody* r = A;
			for (size_t i = 0; i < 12; i += 6) {
																			// prob a way to condence this \/
				matrix transformation(4, 4);
			
				for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++) {
					glm::mat4 t = r->Trans().WorldTransformation();

					transformation.set(i, j, t[j][i]);
				}
			
				matrix x(4, 1);
				matrix y(4, 1);
				matrix z(4, 1);
			
				x.set(0, 1);
				y.set(1, 1);
				z.set(2, 1);
			
				matrix invInrtLocal = from_glm(r->Inertia);
			
				matrix X = transformation * x;
				matrix Y = transformation * y;
				matrix Z = transformation * z;
			
				matrix world2Local(3, 3);
			
				world2Local.insert(0, 0, X.resize(3));
				world2Local.insert(1, 0, Y.resize(3));
				world2Local.insert(2, 0, Z.resize(3));
			
				matrix invInrtWorld = world2Local * invInrtLocal * transpose(world2Local); // prob a way to condence this /\

				W.set(i + 0,  r->InvMass);             // [ 1/mass                           ]
				W.set(i + 1,  r->InvMass);             // |     1/mass                       |
				W.set(i + 2,  r->InvMass);             // |         1/mass                   |
				W.set(i + 3,  invInrtWorld.get(0, 0)); // |              [                 ] |
				W.set(i + 4,  invInrtWorld.get(1, 1)); // |              | inertial tensor | |
				W.set(i + 5,  invInrtWorld.get(2, 2)); // [              [  (world space)  ] ]

				r = B;
			}

			M = W * transpose(J);
			K = J * M;

			if (P.height == 1) P = vec<3>();
		}

		void solve(
			float dt) override
		{
			vec<12> v0;

			vec<3> eA, eB;
			eA.set(0, E.get(0)); eB.set(0, E.get(0));
			eA.set(1, E.get(1)); eB.set(1, E.get(1));
			eA.set(2, E.get(2)); eB.set(2, E.get(2));

			vec<3> vA = from_glm(A->Velocity) - eA;
			vec<3> vB = from_glm(B->Velocity) + eB;

			v0.insert(0, 0, vA); v0.insert(0, 3, from_glm(A->AngularVelocity));
			v0.insert(0, 6, vB); v0.insert(0, 9, from_glm(B->AngularVelocity));

			vec<3> R = -J * v0;

			vec<3> lambda = P;

			for (size_t i = 0; i < K.height; i++)
			{
				float dR = R.get(i) - dot(K.row_begin(i), lambda.col_begin(0));
				lambda.get(i) += dR / K.get(i, i);
			}

			matrix impluse = M * lambda;

			if ( A->IsAxisLocked.x && !B->IsAxisLocked.x) { impluse.get(6) -= impluse.get(0); impluse.get(0) = 0; } // is 1 object is locked add vel to other
			if ( A->IsAxisLocked.y && !B->IsAxisLocked.y) { impluse.get(7) -= impluse.get(1); impluse.get(1) = 0; }
			if ( A->IsAxisLocked.z && !B->IsAxisLocked.z) { impluse.get(8) -= impluse.get(2); impluse.get(2) = 0; }

			if (!A->IsAxisLocked.x &&  B->IsAxisLocked.x) { impluse.get(1) += impluse.get(6); impluse.get(6) = 0; }
			if (!A->IsAxisLocked.y &&  B->IsAxisLocked.y) { impluse.get(2) += impluse.get(7); impluse.get(7) = 0; }
			if (!A->IsAxisLocked.z &&  B->IsAxisLocked.z) { impluse.get(3) += impluse.get(8); impluse.get(8) = 0; }

			Rigidbody* r = A;
			for (size_t i = 0; i < 12; i += 6) {
				r->       Velocity.x += impluse.get(i);
				r->       Velocity.y += impluse.get(i + 1);
				r->       Velocity.z += impluse.get(i + 2);
				r->AngularVelocity.x += impluse.get(i + 3);
				r->AngularVelocity.y += impluse.get(i + 4);
				r->AngularVelocity.z += impluse.get(i + 5);

				//r->Velocity        *= 0.98f;
				//r->AngularVelocity *= 0.98f;

				r = B;
			}

			P = lambda;
		}
	};

	struct BallInSocketConstraint : VelocityConstraint {
		vec<3> localAnchorA;
		vec<3> localAnchorB;

		BallInSocketConstraint(
			Rigidbody* a,
			Rigidbody* b,
			vec<3> localAnchorA,
			vec<3> localAnchorB
		)
			: VelocityConstraint(a, b)
			, localAnchorA(localAnchorA)
			, localAnchorB(localAnchorB)
		{}

		vec<3> target(Rigidbody* r) {
			if (r == A) return from_glm(A->Trans().Rotation * (A->Trans().Scale * to_glm(localAnchorA)));
			else        return from_glm(B->Trans().Rotation * (B->Trans().Scale * to_glm(localAnchorB)));
		}

		vec<3> world_target(Rigidbody* r) {
			if (r == A) return from_glm(A->Trans().Position) + target(A);
			else        return from_glm(B->Trans().Position) + target(B);
		}

		void init(
			float dt) override
		{
			// translate target into local space for both objects

			vec<3> r_A = target(A);
			vec<3> r_B = target(B);

			J = mat<3, 12>(); // [I -cross(a) -I cross(b)]

			J.insert(0, 0, matrix(3, 3, 1));
			J.insert(3, 0, cross_3(r_A)); // this one should be neg tho?

			J.insert(6, 0, matrix(3, 3, -1));
			J.insert(9, 0, cross_3(-r_B));

			E = (world_target(B) - world_target(A)) * (biasStrength / dt);

			VelocityConstraint::init(dt);
		}
	};

	//struct Mechanism {

	//	std::vector<VelocityConstraint*> Constraints;
	//	std::set<iw::Rigidbody*> Bodies;

	//	float Beta = .02f; // error correction

	//	void init() {
	//		Bodies.clear(); // sloppy

	//		for (Constraint* c : Constraints) {
	//			Bodies.insert(c->A);
	//			Bodies.insert(c->B);
	//		}
	//	}

	//	void solve(
	//		float dt)
	//	{
	//		matrix J; // Jacobian
	//		matrix W; // Inverse mass matrix

	//		matrix K; // JWt'
	//		matrix R; // -J * v0

	//		matrix v0; // Initial vel

	//		matrix error(Bodies.size() * 3);

	//		// Create J

	//		size_t i = 0;
	//		for (Rigidbody* r : Bodies)
	//		{
	//			size_t j = 0;
	//			for (VelocityConstraint* c : Constraints)
	//			{
	//				c->init();

	//				matrix m;
	//				if (c->contains(r)) {
	//					m = c->getJ(r);

	//					glm::vec3 aPos =          r ->Trans().Position + c->target(r);
	//					glm::vec3 bPos = c->other(r)->Trans().Position + c->target(c->other(r));

	//					glm::vec3 b = Beta / dt / 2* (bPos - aPos);

	//					error.set(i * 3,     b.x);
	//					error.set(i * 3 + 1, b.y);
	//					error.set(i * 3 + 2, b.z);
	//				}

	//				else {
	//					m = matrix(3, 6);
	//				}

	//				J.insert(i * 6, j * 3, m);

	//				//LOG_INFO << "J:";  LOG_INFO << J;

	//				j++;
	//			}

	//			i++;
	//		}

	//		// Create W & v0

	//		W = matrix(Bodies.size() * 6);
	//		W.diagonal = true;

	//		v0 = matrix(Bodies.size() * 6);

	//		i = 0;
	//		for (Rigidbody* r : Bodies)
	//		{
	//			matrix transformation(4, 4);

	//			for (size_t i = 0; i < 4; i++)
	//			for (size_t j = 0; j < 4; j++) {
	//				glm::mat4 t = r->Trans().WorldTransformation();
	//				transformation.set(i, j, t[j][i]);
	//			}

	//			matrix x(4);
	//			matrix y(4);
	//			matrix z(4);

	//			x.set(0, 1);
	//			y.set(1, 1);
	//			z.set(2, 1);

	//			matrix invInrtLocal = matrix(3, 3, r->InvMass);

	//			matrix X = transformation * x;
	//			matrix Y = transformation * y;
	//			matrix Z = transformation * z;

	//			matrix world2Local(3, 3);

	//			world2Local.insert(0, 0, X.resize(3));
	//			world2Local.insert(1, 0, Y.resize(3));
	//			world2Local.insert(2, 0, Z.resize(3));

	//			matrix invInrtWorld = world2Local * invInrtLocal * transpose(world2Local);

	//			W.set(i,     r->InvMass);
	//			W.set(i + 1, r->InvMass);
	//			W.set(i + 2, r->InvMass);
	//					 
	//			W.set(i + 3, invInrtWorld.get(0, 0));
	//			W.set(i + 4, invInrtWorld.get(1, 1));
	//			W.set(i + 5, invInrtWorld.get(2, 2));

	//			v0.set(i,     r->Velocity.x - error.get(i / 2)    );
	//			v0.set(i + 1, r->Velocity.y - error.get(i / 2 + 1));
	//			v0.set(i + 2, r->Velocity.z - error.get(i / 2 + 2));

	//			v0.set(i + 3, r->AngularVelocity.x);
	//			v0.set(i + 4, r->AngularVelocity.y);
	//			v0.set(i + 5, r->AngularVelocity.z);

	//			i += 6;
	//		}

	//		K = J * W * transpose(J);
	//		R = -J * v0;

	//		// Solve

	//		matrix h(K.width);

	//		for (size_t _ = 0; _ < 50; _++)
	//		for (       i = 0; i < K.height; i++) {
	//			float dR = R.get(i) - dot(K.row_begin(i), h.col_begin(0));
	//			h.get(i) += dR / K.get(i, i);
	//		}

	//		matrix impluse = W * transpose(J) * h;

	//		i = 0;
	//		for (Rigidbody* body : Bodies) {
	//			body->       Velocity.x += impluse.get(i);
	//			body->       Velocity.y += impluse.get(i + 1);
	//			body->       Velocity.z += impluse.get(i + 2);
	//			body->AngularVelocity.x += impluse.get(i + 3);
	//			body->AngularVelocity.y += impluse.get(i + 4);
	//			body->AngularVelocity.z += impluse.get(i + 5);

	//			body->       Velocity *= 1 - dt / 10;
	//			body->AngularVelocity *= 1 - dt / 10;

	//			i += 6;
	//		}

	//		//LOG_INFO << K * h - R;
	//	}

	//	//float dhi = (R.get(i) - dot(J.row_begin(i), dV.col_begin(0))) / K.get(i, i);
	//	// project hi ?
	//};

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

		matrix md(3, 1); md.diagonal = true;
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
}
