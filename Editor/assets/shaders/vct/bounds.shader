vec3 scaleAndBias(
	vec3 p)
{
	return 0.5f * p + vec3(0.5f); 
}

bool isInsideCube(
	vec3 p, 
	float e) 
{ 
	return abs(p.x) < 2 + e && abs(p.y) < 2 + e && abs(p.z) < 2 + e;
}
