// Gamma correction

vec3 sRGBToLinear(
	vec3 sRGBCol)
{
	vec3 linearRGBLo = sRGBCol / 12.92f;
	vec3 linearRGBHi = pow((sRGBCol + 0.055f) / 1.055f, vec3(2.4f));
	vec3 linearRGB = (length(sRGBCol) <= 0.04045f) ? linearRGBLo : linearRGBHi;
	return linearRGB;
}

vec3 linearToSRGB(
	vec3 linearCol)
{
	vec3 sRGBLo = linearCol * 12.92f;
	vec3 sRGBHi = (pow(abs(linearCol), vec3(1.0f / 2.4f)) * 1.055f) - 0.055f;
	vec3 sRGB = (length(linearCol) <= 0.0031308f) ? sRGBLo : sRGBHi;
	return sRGB;
}
