// from https://mikeash.com/pyblog/fluid-simulation-for-dummies.html

#include <math.h>


#define IX(x, y, z) ((x) + (y) * N)


struct FluidCube {
	int size;
	float dt;
	float diff;
	float visc;

	float* s;
	float* density;

	float* Vx;
	float* Vy;

	float* Vx0;
	float* Vy0;
};

FluidCube* FluidCubeCreate(int size, int diffusion, int viscosity, float dt)
{
	FluidCube* cube = (FluidCube*)malloc(sizeof(*cube));
	size_t N = size;

	cube->size = size;
	cube->dt = dt;
	cube->diff = diffusion;
	cube->visc = viscosity;

	cube->s       = (float*)calloc(N * N, sizeof(float));
	cube->density = (float*)calloc(N * N, sizeof(float));

	cube->Vx = (float*)calloc(N * N, sizeof(float));
	cube->Vy = (float*)calloc(N * N, sizeof(float));

	cube->Vx0 = (float*)calloc(N * N, sizeof(float));
	cube->Vy0 = (float*)calloc(N * N, sizeof(float));

	return cube;
}

void FluidCubeFree(FluidCube* cube)
{
	free(cube->s);
	free(cube->density);

	free(cube->Vx);
	free(cube->Vy);

	free(cube->Vx0);
	free(cube->Vy0);

	free(cube);
}

static void set_bnd(int b, float* x, int N)
{
	for (int i = 1; i < N - 1; i++) {
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N - 1)] = b == 2 ? -x[IX(i, N - 2)] : x[IX(i, N - 2)];
	}

	for (int j = 1; j < N - 1; j++) {
		x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
		x[IX(N - 1, j)] = b == 1 ? -x[IX(N - 2, j)] : x[IX(N - 2, j)];
	}

	x[IX(0, 0, 0)] = 0.33f * (x[IX(1, 0, 0)]
		+ x[IX(0, 1, 0)]
		+ x[IX(0, 0, 1)]);
	x[IX(0, N - 1, 0)] = 0.33f * (x[IX(1, N - 1, 0)]
		+ x[IX(0, N - 2, 0)]
		+ x[IX(0, N - 1, 1)]);
	x[IX(N - 1, 0, 0)] = 0.33f * (x[IX(N - 2, 0, 0)]
		+ x[IX(N - 1, 1, 0)]
		+ x[IX(N - 1, 0, 1)]);
	x[IX(N - 1, N - 1, 0)] = 0.33f * (x[IX(N - 2, N - 1, 0)]
		+ x[IX(N - 1, N - 2, 0)]
		+ x[IX(N - 1, N - 1, 1)]);
}

static void lin_solve(int b, float* x, float* x0, float a, float c, int iter, int N)
{
	float cRecip = 1.0 / c;
	for (int k = 0; k < iter; k++) {
		for (int j = 1; j < N - 1; j++)
		for (int i = 1; i < N - 1; i++) {
			x[IX(i, j)] =
				(x0[IX(i, j)]
					+ a * (x[IX(i + 1, j)]
						+ x[IX(i - 1, j)]
						+ x[IX(i, j + 1)]
						+ x[IX(i, j - 1)]
						)) * cRecip;
		}

		set_bnd(b, x, N);
	}
}

static void diffuse(int b, float* x, float* x0, float diff, float dt, int iter, int N)
{
	float a = dt * diff * (N - 2) * (N - 2);
	lin_solve(b, x, x0, a, 1 + 6 * a, iter, N);
}

static void advect(int b, float* d, float* d0, float* velocX, float* velocY, float dt, int N)
{
	float i0, i1, j0, j1;

	float dtx = dt * (N - 2);
	float dty = dt * (N - 2);

	float s0, s1, t0, t1;
	float tmp1, tmp2, tmp3, x, y;

	float Nfloat = N;
	float ifloat, jfloat;
	int i, j;

	for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
		for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
			tmp1 = dtx * velocX[IX(i, j)];
			tmp2 = dty * velocY[IX(i, j)];
			x = ifloat - tmp1;
			y = jfloat - tmp2;

			if (x < 0.5f) x = 0.5f;
			if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
			i0 = floorf(x);
			i1 = i0 + 1.0f;
			if (y < 0.5f) y = 0.5f;
			if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
			j0 = floorf(y);
			j1 = j0 + 1.0f;

			s1 = x - i0;
			s0 = 1.0f - s1;
			t1 = y - j0;
			t0 = 1.0f - t1;

			int i0i = i0;
			int i1i = i1;
			int j0i = j0;
			int j1i = j1;

			d[IX(i, j)] =
				s0 * (t0 * (d0[IX(i0i, j0i)])
					+ (t1 * (d0[IX(i0i, j1i)])))
				+ s1 * (t0 * (d0[IX(i1i, j0i)])
					+ (t1 * (d0[IX(i1i, j1i)])));
		}
	}

	set_bnd(b, d, N);
}

static void project(float* velocX, float* velocY, float* p, float* div, int iter, int N)
{

	for (int j = 1; j < N - 1; j++)
	for (int i = 1; i < N - 1; i++) {
		div[IX(i, j)] = -0.5f / N * (  velocX[IX(i + 1, j    )] - velocX[IX(i - 1, j    )]
				                   + velocY[IX(i,     j + 1)] - velocY[IX(i,     j - 1)]);
		p[IX(i, j)] = 0;
	}

	set_bnd(0, div, N);
	set_bnd(0, p,   N);

	lin_solve(0, p, div, 1, 6, iter, N);

	for (int j = 1; j < N - 1; j++)
	for (int i = 1; i < N - 1; i++) {
		velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * N;
		velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * N;
	}

	set_bnd(1, velocX, N);
	set_bnd(2, velocY, N);
}

void FluidCubeStep(FluidCube* cube)
{
	int N = cube->size;
	float visc = cube->visc;
	float diff = cube->diff;
	float dt = cube->dt;
	float* Vx = cube->Vx;
	float* Vy = cube->Vy;
	float* Vx0 = cube->Vx0;
	float* Vy0 = cube->Vy0;
	float* s = cube->s;
	float* density = cube->density;

	diffuse(1, Vx0, Vx, visc, dt, 4, N);
	diffuse(2, Vy0, Vy, visc, dt, 4, N);

	project(Vx0, Vy0, Vx, Vy, 4, N);

	advect(1, Vx, Vx0, Vx0, Vy0, dt, N);
	advect(2, Vy, Vy0, Vx0, Vy0, dt, N);

	project(Vx, Vy, Vx0, Vy0, 4, N);

	diffuse(0, s, density, diff, dt, 4, N);
	advect(0, density, s, Vx, Vy, dt, N);
}

void FluidCubeAddDensity(FluidCube* cube, int x, int y, float amount)
{
	int N = cube->size;
	cube->density[IX(x, y)] += amount;
}

void FluidCubeAddVelocity(FluidCube* cube, int x, int y, float amountX, float amountY)
{
	int N = cube->size;
	int index = IX(x, y);

	cube->Vx[index] += amountX;
	cube->Vy[index] += amountY;
}


// from GDC03.pdf

//constexpr int N = 400;
//constexpr int size = (N + 2) * (N + 2);
//
//float u[size];
//float v[size];
//float u_prev[size];
//float v_prev[size];
//float dens[size];
//float dens_prev[size];
//
//#define IX(i,j) ((i)+(N+2)*(j))
//#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}
//
//void set_bnd(int N, int b, float* x)
//{
//	int i;
//	for (i = 1; i <= N; i++) {
//		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
//		x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
//		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
//		x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
//	}
//	x[IX(0, 0)] = 0.5 * (x[IX(1, 0)] + x[IX(0, 1)]);
//	x[IX(0, N + 1)] = 0.5 * (x[IX(1, N + 1)] + x[IX(0, N)]);
//	x[IX(N + 1, 0)] = 0.5 * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
//	x[IX(N + 1, N + 1)] = 0.5 * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
//}
//
//void add_source(int N, float* x, float* s, float dt)
//{
//	int i, size = (N + 2) * (N + 2);
//	for (i = 0; i < size; i++) x[i] += dt * s[i];
//}
//
//void diffuse(int N, int b, float* x, float* x0, float diff, float dt)
//{
//	int i, j, k;
//	float a = dt * diff * N * N;
//	for (k = 0; k < 20; k++) {
//		for (i = 1; i <= N; i++) {
//			for (j = 1; j <= N; j++) {
//				x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] +
//					x[IX(i, j - 1)] + x[IX(i, j + 1)])) / (1 + 4 * a);
//			}
//		}
//		set_bnd(N, b, x);
//	}
//}
//
//void advect(int N, int b, float* d, float* d0, float* u, float* v, float dt)
//{
//	int i, j, i0, j0, i1, j1;
//	float x, y, s0, t0, s1, t1, dt0;
//	dt0 = dt * N;
//	for (i = 1; i <= N; i++) {
//		for (j = 1; j <= N; j++) {
//			x = i - dt0 * u[IX(i, j)]; y = j - dt0 * v[IX(i, j)];
//			if (x < 0.5) x = 0.5; if (x > N + 0.5) x = N + 0.5; i0 = (int)x; i1 = i0 + 1;
//			if (y < 0.5) y = 0.5; if (y > N + 0.5) y = N + 0.5; j0 = (int)y; j1 = j0 + 1;
//			s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1;
//			d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
//				s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
//		}
//	}
//	set_bnd(N, b, d);
//}
//
//void dens_step(int N, float* x, float* x0, float* u, float* v, float diff,
//	float dt)
//{
//	add_source(N, x, x0, dt);
//	SWAP(x0, x); diffuse(N, 0, x, x0, diff, dt);
//	SWAP(x0, x); advect(N, 0, x, x0, u, v, dt);
//}
//
//void project(int N, float* u, float* v, float* p, float* div)
//{
//	int i, j, k;
//	float h;
//	h = 1.0 / N;
//	for (i = 1; i <= N; i++) {
//		for (j = 1; j <= N; j++) {
//			div[IX(i, j)] = -0.5 * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] +
//				v[IX(i, j + 1)] - v[IX(i, j - 1)]);
//			p[IX(i, j)] = 0;
//		}
//	}
//	set_bnd(N, 0, div); set_bnd(N, 0, p);
//	for (k = 0; k < 20; k++) {
//		for (i = 1; i <= N; i++) {
//			for (j = 1; j <= N; j++) {
//				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
//					p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
//			}
//		}
//		set_bnd(N, 0, p);
//	}
//	for (i = 1; i <= N; i++) {
//		for (j = 1; j <= N; j++) {
//			u[IX(i, j)] -= 0.5 * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
//			v[IX(i, j)] -= 0.5 * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
//		}
//	}
//	set_bnd(N, 1, u); set_bnd(N, 2, v);
//}
//
//
//void vel_step(int N, float* u, float* v, float* u0, float* v0,
//	float visc, float dt)
//{
//	add_source(N, u, u0, dt); add_source(N, v, v0, dt);
//	SWAP(u0, u); diffuse(N, 1, u, u0, visc, dt);
//	SWAP(v0, v); diffuse(N, 2, v, v0, visc, dt);
//	project(N, u, v, u0, v0);
//	SWAP(u0, u); SWAP(v0, v);
//	advect(N, 1, u, u0, u0, v0, dt); advect(N, 2, v, v0, u0, v0, dt);
//	project(N, u, v, u0, v0);
//}
//
//
//
