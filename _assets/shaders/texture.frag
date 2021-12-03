#version 330

in vec2 TexCoords;
in vec3 FragPos;

out vec4 FragColor;

// flags, why does intel can pass ints? engine problem or driver problem?

uniform float mat_useBoxFade      = 0.f;
uniform float mat_useTransparency = 0.f;
uniform float mat_isFont          = 0.f;
//uniform float mat_isDistanceField = 1.f;

uniform float mat_hasTexture = 0.f;

// standard coloring for texture

uniform vec4 mat_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform sampler2D mat_texture;

// for fonts

uniform float mat_font_width = 0.1f;
uniform float mat_font_edge  = 0.2f;

vec4 getColor()
{
	vec4 color = mat_color;

	if (mat_hasTexture == 1.0f)
	{
		if (mat_isFont == 1.0f)
		{
			float distance = 1.0f - texture2D(mat_texture, TexCoords).a;
			float alpha = 1.0f - smoothstep(
				mat_font_width, 
				mat_font_width + mat_font_edge, 
				distance
			);

			color.a *= alpha;
		}

		else 
		{
			color *= texture(mat_texture, TexCoords);
		}
	}
	
	return color;
}

#include shaders/boxfade.shader

uniform float mat_alphaThresh = 0.0f;

void main()
{
	vec4 color = getColor();

	if (mat_useBoxFade == 1.0f) // form box fade
	{
		color.a *= getBoxFade();
	}

	if (mat_useTransparency == 1.0f)
	{
		if (color.a < mat_alphaThresh) {
			discard;
		}
	}

	FragColor = color;
}