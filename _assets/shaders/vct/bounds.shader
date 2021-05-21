vec3 scaleAndBias(
	vec3 p)
{
	return 0.5f * p + vec3(0.5f); 
}

bool isInsideCube(
	vec3 p,
	vec3 c,
	float e)
{ 
	return abs(p.x) < c.x + e && abs(p.y) < c.y + e && abs(p.z) < c.z + e;
}

