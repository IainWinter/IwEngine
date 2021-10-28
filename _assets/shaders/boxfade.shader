float boxDistance(
    vec2 a, // min point of box
    vec2 b, // max point of box
    vec2 p) // point to test
{
	float dx = max(max(a.x - p.x, 0.f), p.x - b.x);
	float dy = max(max(a.y - p.y, 0.f), p.y - b.y);
	return sqrt(dx*dx + dy*dy);
}

uniform vec4 mat_box;
uniform float mat_fade;

float getBoxFade()
{
	float dist = boxDistance(mat_box.xy, mat_box.zw, FragPos.xy);
	return clamp(1.f - dist * mat_fade, 0.f, 1.f);
}