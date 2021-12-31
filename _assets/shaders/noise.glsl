/////////////////////////
/////////////////////////
/////////////////////////
///////////////////////// start of random noise?       /////////////////////////
/////////////////////////
/////////////////////////
/////////////////////////
/////////////////////////

/* ***** Jenkins Lookup3 Hash Functions ***** */

/* Source: http://burtleburtle.net/bob/c/lookup3.c */

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))

#define mix(a, b, c) \
  { \
    a -= c; \
    a ^= rot(c, 4); \
    c += b; \
    b -= a; \
    b ^= rot(a, 6); \
    a += c; \
    c -= b; \
    c ^= rot(b, 8); \
    b += a; \
    a -= c; \
    a ^= rot(c, 16); \
    c += b; \
    b -= a; \
    b ^= rot(a, 19); \
    a += c; \
    c -= b; \
    c ^= rot(b, 4); \
    b += a; \
  }

#define final(a, b, c) \
  { \
    c ^= b; \
    c -= rot(b, 14); \
    a ^= c; \
    a -= rot(c, 11); \
    b ^= a; \
    b -= rot(a, 25); \
    c ^= b; \
    c -= rot(b, 16); \
    a ^= c; \
    a -= rot(c, 4); \
    b ^= a; \
    b -= rot(a, 14); \
    c ^= b; \
    c -= rot(b, 24); \
  }

uint hash_uint(uint kx)
{
  uint a, b, c;
  a = b = c = 0xdeadbeefu + (1u << 2u) + 13u;

  a += kx;
  final(a, b, c);

  return c;
}

uint hash_uint2(uint kx, uint ky)
{
  uint a, b, c;
  a = b = c = 0xdeadbeefu + (2u << 2u) + 13u;

  b += ky;
  a += kx;
  final(a, b, c);

  return c;
}

uint hash_uint3(uint kx, uint ky, uint kz)
{
  uint a, b, c;
  a = b = c = 0xdeadbeefu + (3u << 2u) + 13u;

  c += kz;
  b += ky;
  a += kx;
  final(a, b, c);

  return c;
}

uint hash_uint4(uint kx, uint ky, uint kz, uint kw)
{
  uint a, b, c;
  a = b = c = 0xdeadbeefu + (4u << 2u) + 13u;

  a += kx;
  b += ky;
  c += kz;
  mix(a, b, c);

  a += kw;
  final(a, b, c);

  return c;
}

#undef rot
#undef final
#undef mix

uint hash_int(int kx)
{
  return hash_uint(uint(kx));
}

uint hash_int2(int kx, int ky)
{
  return hash_uint2(uint(kx), uint(ky));
}

uint hash_int3(int kx, int ky, int kz)
{
  return hash_uint3(uint(kx), uint(ky), uint(kz));
}

uint hash_int4(int kx, int ky, int kz, int kw)
{
  return hash_uint4(uint(kx), uint(ky), uint(kz), uint(kw));
}

/* Hashing uint or uint[234] into a float in the range [0, 1]. */

float hash_uint_to_float(uint kx)
{
  return float(hash_uint(kx)) / float(0xFFFFFFFFu);
}

float hash_uint2_to_float(uint kx, uint ky)
{
  return float(hash_uint2(kx, ky)) / float(0xFFFFFFFFu);
}

float hash_uint3_to_float(uint kx, uint ky, uint kz)
{
  return float(hash_uint3(kx, ky, kz)) / float(0xFFFFFFFFu);
}

float hash_uint4_to_float(uint kx, uint ky, uint kz, uint kw)
{
  return float(hash_uint4(kx, ky, kz, kw)) / float(0xFFFFFFFFu);
}

/* Hashing float or vec[234] into a float in the range [0, 1]. */

float hash_float_to_float(float k)
{
  return hash_uint_to_float(floatBitsToUint(k));
}

float hash_vec2_to_float(vec2 k)
{
  return hash_uint2_to_float(floatBitsToUint(k.x), floatBitsToUint(k.y));
}

float hash_vec3_to_float(vec3 k)
{
  return hash_uint3_to_float(floatBitsToUint(k.x), floatBitsToUint(k.y), floatBitsToUint(k.z));
}

float hash_vec4_to_float(vec4 k)
{
  return hash_uint4_to_float(
      floatBitsToUint(k.x), floatBitsToUint(k.y), floatBitsToUint(k.z), floatBitsToUint(k.w));
}

/* Hashing vec[234] into vec[234] of components in the range [0, 1]. */

vec2 hash_vec2_to_vec2(vec2 k)
{
  return vec2(hash_vec2_to_float(k), hash_vec3_to_float(vec3(k, 1.0)));
}

vec3 hash_vec3_to_vec3(vec3 k)
{
  return vec3(
      hash_vec3_to_float(k), hash_vec4_to_float(vec4(k, 1.0)), hash_vec4_to_float(vec4(k, 2.0)));
}

vec4 hash_vec4_to_vec4(vec4 k)
{
  return vec4(hash_vec4_to_float(k.xyzw),
              hash_vec4_to_float(k.wxyz),
              hash_vec4_to_float(k.zwxy),
              hash_vec4_to_float(k.yzwx));
}

/* Hashing float or vec[234] into vec3 of components in range [0, 1]. */

vec3 hash_float_to_vec3(float k)
{
  return vec3(
      hash_float_to_float(k), hash_vec2_to_float(vec2(k, 1.0)), hash_vec2_to_float(vec2(k, 2.0)));
}

vec3 hash_vec2_to_vec3(vec2 k)
{
  return vec3(
      hash_vec2_to_float(k), hash_vec3_to_float(vec3(k, 1.0)), hash_vec3_to_float(vec3(k, 2.0)));
}

vec3 hash_vec4_to_vec3(vec4 k)
{
  return vec3(hash_vec4_to_float(k.xyzw), hash_vec4_to_float(k.zxwy), hash_vec4_to_float(k.wzyx));
}

/* Other Hash Functions */

float integer_noise(int n)
{
  int nn;
  n = (n + 1013) & 0x7fffffff;
  n = (n >> 13) ^ n;
  nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
  return 0.5 * (float(nn) / 1073741824.0);
}
/* clang-format off */
#define FLOORFRAC(x, x_int, x_fract) { float x_floor = floor(x); x_int = int(x_floor); x_fract = x - x_floor; }
/* clang-format on */

/* Bilinear Interpolation:
 *
 * v2          v3
 *  @ + + + + @       y
 *  +         +       ^
 *  +         +       |
 *  +         +       |
 *  @ + + + + @       @------> x
 * v0          v1
 *
 */
float bi_mix(float v0, float v1, float v2, float v3, float x, float y)
{
  float x1 = 1.0 - x;
  return (1.0 - y) * (v0 * x1 + v1 * x) + y * (v2 * x1 + v3 * x);
}

/* Trilinear Interpolation:
 *
 *   v6               v7
 *     @ + + + + + + @
 *     +\            +\
 *     + \           + \
 *     +  \          +  \
 *     +   \ v4      +   \ v5
 *     +    @ + + + +++ + @          z
 *     +    +        +    +      y   ^
 *  v2 @ + +++ + + + @ v3 +       \  |
 *      \   +         \   +        \ |
 *       \  +          \  +         \|
 *        \ +           \ +          +---------> x
 *         \+            \+
 *          @ + + + + + + @
 *        v0               v1
 */
float tri_mix(float v0,
              float v1,
              float v2,
              float v3,
              float v4,
              float v5,
              float v6,
              float v7,
              float x,
              float y,
              float z)
{
  float x1 = 1.0 - x;
  float y1 = 1.0 - y;
  float z1 = 1.0 - z;
  return z1 * (y1 * (v0 * x1 + v1 * x) + y * (v2 * x1 + v3 * x)) +
         z * (y1 * (v4 * x1 + v5 * x) + y * (v6 * x1 + v7 * x));
}

float quad_mix(float v0,
               float v1,
               float v2,
               float v3,
               float v4,
               float v5,
               float v6,
               float v7,
               float v8,
               float v9,
               float v10,
               float v11,
               float v12,
               float v13,
               float v14,
               float v15,
               float x,
               float y,
               float z,
               float w)
{
  return mix(tri_mix(v0, v1, v2, v3, v4, v5, v6, v7, x, y, z),
             tri_mix(v8, v9, v10, v11, v12, v13, v14, v15, x, y, z),
             w);
}

float fade(float t)
{
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float negate_if(float value, uint condition)
{
  return (condition != 0u) ? -value : value;
}

float noise_grad(uint hash, float x)
{
  uint h = hash & 15u;
  float g = 1u + (h & 7u);
  return negate_if(g, h & 8u) * x;
}

float noise_grad(uint hash, float x, float y)
{
  uint h = hash & 7u;
  float u = h < 4u ? x : y;
  float v = 2.0 * (h < 4u ? y : x);
  return negate_if(u, h & 1u) + negate_if(v, h & 2u);
}

float noise_grad(uint hash, float x, float y, float z)
{
  uint h = hash & 15u;
  float u = h < 8u ? x : y;
  float vt = ((h == 12u) || (h == 14u)) ? x : z;
  float v = h < 4u ? y : vt;
  return negate_if(u, h & 1u) + negate_if(v, h & 2u);
}

float noise_grad(uint hash, float x, float y, float z, float w)
{
  uint h = hash & 31u;
  float u = h < 24u ? x : y;
  float v = h < 16u ? y : z;
  float s = h < 8u ? z : w;
  return negate_if(u, h & 1u) + negate_if(v, h & 2u) + negate_if(s, h & 4u);
}

float noise_perlin(float x)
{
  int X;
  float fx;

  FLOORFRAC(x, X, fx);

  float u = fade(fx);

  float r = mix(noise_grad(hash_int(X), fx), noise_grad(hash_int(X + 1), fx - 1.0), u);

  return r;
}

float noise_perlin(vec2 vec)
{
  int X, Y;
  float fx, fy;

  FLOORFRAC(vec.x, X, fx);
  FLOORFRAC(vec.y, Y, fy);

  float u = fade(fx);
  float v = fade(fy);

  float r = bi_mix(noise_grad(hash_int2(X, Y), fx, fy),
                   noise_grad(hash_int2(X + 1, Y), fx - 1.0, fy),
                   noise_grad(hash_int2(X, Y + 1), fx, fy - 1.0),
                   noise_grad(hash_int2(X + 1, Y + 1), fx - 1.0, fy - 1.0),
                   u,
                   v);

  return r;
}

float noise_perlin(vec3 vec)
{
  int X, Y, Z;
  float fx, fy, fz;

  FLOORFRAC(vec.x, X, fx);
  FLOORFRAC(vec.y, Y, fy);
  FLOORFRAC(vec.z, Z, fz);

  float u = fade(fx);
  float v = fade(fy);
  float w = fade(fz);

  float r = tri_mix(noise_grad(hash_int3(X, Y, Z), fx, fy, fz),
                    noise_grad(hash_int3(X + 1, Y, Z), fx - 1, fy, fz),
                    noise_grad(hash_int3(X, Y + 1, Z), fx, fy - 1, fz),
                    noise_grad(hash_int3(X + 1, Y + 1, Z), fx - 1, fy - 1, fz),
                    noise_grad(hash_int3(X, Y, Z + 1), fx, fy, fz - 1),
                    noise_grad(hash_int3(X + 1, Y, Z + 1), fx - 1, fy, fz - 1),
                    noise_grad(hash_int3(X, Y + 1, Z + 1), fx, fy - 1, fz - 1),
                    noise_grad(hash_int3(X + 1, Y + 1, Z + 1), fx - 1, fy - 1, fz - 1),
                    u,
                    v,
                    w);

  return r;
}

float noise_perlin(vec4 vec)
{
  int X, Y, Z, W;
  float fx, fy, fz, fw;

  FLOORFRAC(vec.x, X, fx);
  FLOORFRAC(vec.y, Y, fy);
  FLOORFRAC(vec.z, Z, fz);
  FLOORFRAC(vec.w, W, fw);

  float u = fade(fx);
  float v = fade(fy);
  float t = fade(fz);
  float s = fade(fw);

  float r = quad_mix(
      noise_grad(hash_int4(X, Y, Z, W), fx, fy, fz, fw),
      noise_grad(hash_int4(X + 1, Y, Z, W), fx - 1.0, fy, fz, fw),
      noise_grad(hash_int4(X, Y + 1, Z, W), fx, fy - 1.0, fz, fw),
      noise_grad(hash_int4(X + 1, Y + 1, Z, W), fx - 1.0, fy - 1.0, fz, fw),
      noise_grad(hash_int4(X, Y, Z + 1, W), fx, fy, fz - 1.0, fw),
      noise_grad(hash_int4(X + 1, Y, Z + 1, W), fx - 1.0, fy, fz - 1.0, fw),
      noise_grad(hash_int4(X, Y + 1, Z + 1, W), fx, fy - 1.0, fz - 1.0, fw),
      noise_grad(hash_int4(X + 1, Y + 1, Z + 1, W), fx - 1.0, fy - 1.0, fz - 1.0, fw),
      noise_grad(hash_int4(X, Y, Z, W + 1), fx, fy, fz, fw - 1.0),
      noise_grad(hash_int4(X + 1, Y, Z, W + 1), fx - 1.0, fy, fz, fw - 1.0),
      noise_grad(hash_int4(X, Y + 1, Z, W + 1), fx, fy - 1.0, fz, fw - 1.0),
      noise_grad(hash_int4(X + 1, Y + 1, Z, W + 1), fx - 1.0, fy - 1.0, fz, fw - 1.0),
      noise_grad(hash_int4(X, Y, Z + 1, W + 1), fx, fy, fz - 1.0, fw - 1.0),
      noise_grad(hash_int4(X + 1, Y, Z + 1, W + 1), fx - 1.0, fy, fz - 1.0, fw - 1.0),
      noise_grad(hash_int4(X, Y + 1, Z + 1, W + 1), fx, fy - 1.0, fz - 1.0, fw - 1.0),
      noise_grad(hash_int4(X + 1, Y + 1, Z + 1, W + 1), fx - 1.0, fy - 1.0, fz - 1.0, fw - 1.0),
      u,
      v,
      t,
      s);

  return r;
}

/* Remap the output of noise to a predictable range [-1, 1].
 * The scale values were computed experimentally by the OSL developers.
 */
float noise_scale1(float result)
{
  return 0.2500 * result;
}

float noise_scale2(float result)
{
  return 0.6616 * result;
}

float noise_scale3(float result)
{
  return 0.9820 * result;
}

float noise_scale4(float result)
{
  return 0.8344 * result;
}

/* Safe Signed And Unsigned Noise */

float snoise(float p)
{
  float r = noise_perlin(p);
  return (isinf(r)) ? 0.0 : noise_scale1(r);
}

float noise(float p)
{
  return 0.5 * snoise(p) + 0.5;
}

float snoise(vec2 p)
{
  float r = noise_perlin(p);
  return (isinf(r)) ? 0.0 : noise_scale2(r);
}

float noise(vec2 p)
{
  return 0.5 * snoise(p) + 0.5;
}

float snoise(vec3 p)
{
  float r = noise_perlin(p);
  return (isinf(r)) ? 0.0 : noise_scale3(r);
}

float noise(vec3 p)
{
  return 0.5 * snoise(p) + 0.5;
}

float snoise(vec4 p)
{
  float r = noise_perlin(p);
  return (isinf(r)) ? 0.0 : noise_scale4(r);
}

float noise(vec4 p)
{
  return 0.5 * snoise(p) + 0.5;
}
/* The fractal_noise functions are all exactly the same except for the input type. */
float fractal_noise(float p, float octaves)
{
  float fscale = 1.0;
  float amp = 1.0;
  float sum = 0.0;
  octaves = clamp(octaves, 0.0, 16.0);
  int n = int(octaves);
  for (int i = 0; i <= n; i++) {
    float t = noise(fscale * p);
    sum += t * amp;
    amp *= 0.5;
    fscale *= 2.0;
  }
  float rmd = octaves - floor(octaves);
  if (rmd != 0.0) {
    float t = noise(fscale * p);
    float sum2 = sum + t * amp;
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    sum2 *= (float(1 << (n + 1)) / float((1 << (n + 2)) - 1));
    return (1.0 - rmd) * sum + rmd * sum2;
  }
  else {
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    return sum;
  }
}

/* The fractal_noise functions are all exactly the same except for the input type. */
float fractal_noise(vec2 p, float octaves)
{
  float fscale = 1.0;
  float amp = 1.0;
  float sum = 0.0;
  octaves = clamp(octaves, 0.0, 16.0);
  int n = int(octaves);
  for (int i = 0; i <= n; i++) {
    float t = noise(fscale * p);
    sum += t * amp;
    amp *= 0.5;
    fscale *= 2.0;
  }
  float rmd = octaves - floor(octaves);
  if (rmd != 0.0) {
    float t = noise(fscale * p);
    float sum2 = sum + t * amp;
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    sum2 *= (float(1 << (n + 1)) / float((1 << (n + 2)) - 1));
    return (1.0 - rmd) * sum + rmd * sum2;
  }
  else {
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    return sum;
  }
}

/* The fractal_noise functions are all exactly the same except for the input type. */
float fractal_noise(vec3 p, float octaves)
{
  float fscale = 1.0;
  float amp = 1.0;
  float sum = 0.0;
  octaves = clamp(octaves, 0.0, 16.0);
  int n = int(octaves);
  for (int i = 0; i <= n; i++) {
    float t = noise(fscale * p);
    sum += t * amp;
    amp *= 0.5;
    fscale *= 2.0;
  }
  float rmd = octaves - floor(octaves);
  if (rmd != 0.0) {
    float t = noise(fscale * p);
    float sum2 = sum + t * amp;
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    sum2 *= (float(1 << (n + 1)) / float((1 << (n + 2)) - 1));
    return (1.0 - rmd) * sum + rmd * sum2;
  }
  else {
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    return sum;
  }
}

/* The fractal_noise functions are all exactly the same except for the input type. */
float fractal_noise(vec4 p, float octaves)
{
  float fscale = 1.0;
  float amp = 1.0;
  float sum = 0.0;
  octaves = clamp(octaves, 0.0, 16.0);
  int n = int(octaves);
  for (int i = 0; i <= n; i++) {
    float t = noise(fscale * p);
    sum += t * amp;
    amp *= 0.5;
    fscale *= 2.0;
  }
  float rmd = octaves - floor(octaves);
  if (rmd != 0.0) {
    float t = noise(fscale * p);
    float sum2 = sum + t * amp;
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    sum2 *= (float(1 << (n + 1)) / float((1 << (n + 2)) - 1));
    return (1.0 - rmd) * sum + rmd * sum2;
  }
  else {
    sum *= (float(1 << n) / float((1 << (n + 1)) - 1));
    return sum;
  }
}

/* The following offset functions generate random offsets to be added to texture
 * coordinates to act as a seed since the noise functions don't have seed values.
 * A seed value is needed for generating distortion textures and color outputs.
 * The offset's components are in the range [100, 200], not too high to cause
 * bad precision and not to small to be noticeable. We use float seed because
 * OSL only support float hashes.
 */

float random_float_offset(float seed)
{
  return 100.0 + hash_float_to_float(seed) * 100.0;
}

vec2 random_vec2_offset(float seed)
{
  return vec2(100.0 + hash_vec2_to_float(vec2(seed, 0.0)) * 100.0,
              100.0 + hash_vec2_to_float(vec2(seed, 1.0)) * 100.0);
}

vec3 random_vec3_offset(float seed)
{
  return vec3(100.0 + hash_vec2_to_float(vec2(seed, 0.0)) * 100.0,
              100.0 + hash_vec2_to_float(vec2(seed, 1.0)) * 100.0,
              100.0 + hash_vec2_to_float(vec2(seed, 2.0)) * 100.0);
}

vec4 random_vec4_offset(float seed)
{
  return vec4(100.0 + hash_vec2_to_float(vec2(seed, 0.0)) * 100.0,
              100.0 + hash_vec2_to_float(vec2(seed, 1.0)) * 100.0,
              100.0 + hash_vec2_to_float(vec2(seed, 2.0)) * 100.0,
              100.0 + hash_vec2_to_float(vec2(seed, 3.0)) * 100.0);
}

void node_noise_texture_1d(
    vec3 co, float w, float scale, float detail, float distortion, out float value, out vec4 color)
{
  float p = w * scale;
  if (distortion != 0.0) {
    p += snoise(p + random_float_offset(0.0)) * distortion;
  }

  value = fractal_noise(p, detail);
  color = vec4(value,
               fractal_noise(p + random_float_offset(1.0), detail),
               fractal_noise(p + random_float_offset(2.0), detail),
               1.0);
}

void node_noise_texture_2d(
    vec3 co, float w, float scale, float detail, float distortion, out float value, out vec4 color)
{
  vec2 p = co.xy * scale;
  if (distortion != 0.0) {
    p += vec2(snoise(p + random_vec2_offset(0.0)) * distortion,
              snoise(p + random_vec2_offset(1.0)) * distortion);
  }

  value = fractal_noise(p, detail);
  color = vec4(value,
               fractal_noise(p + random_vec2_offset(2.0), detail),
               fractal_noise(p + random_vec2_offset(3.0), detail),
               1.0);
}

void node_noise_texture_3d(
    vec3 co, float w, float scale, float detail, float distortion, out float value, out vec4 color)
{
  vec3 p = co * scale;
  if (distortion != 0.0) {
    p += vec3(snoise(p + random_vec3_offset(0.0)) * distortion,
              snoise(p + random_vec3_offset(1.0)) * distortion,
              snoise(p + random_vec3_offset(2.0)) * distortion);
  }

  value = fractal_noise(p, detail);
  color = vec4(value,
               fractal_noise(p + random_vec3_offset(3.0), detail),
               fractal_noise(p + random_vec3_offset(4.0), detail),
               1.0);
}

void node_noise_texture_4d(
    vec3 co, float w, float scale, float detail, float distortion, out float value, out vec4 color)
{
  vec4 p = vec4(co, w) * scale;
  if (distortion != 0.0) {
    p += vec4(snoise(p + random_vec4_offset(0.0)) * distortion,
              snoise(p + random_vec4_offset(1.0)) * distortion,
              snoise(p + random_vec4_offset(2.0)) * distortion,
              snoise(p + random_vec4_offset(3.0)) * distortion);
  }

  value = fractal_noise(p, detail);
  color = vec4(value,
               fractal_noise(p + random_vec4_offset(4.0), detail),
               fractal_noise(p + random_vec4_offset(5.0), detail),
               1.0);
}

/////////////////////////
/////////////////////////
/////////////////////////
///////////////////////// END OF NOISE ?
/////////////////////////
/////////////////////////
/////////////////////////