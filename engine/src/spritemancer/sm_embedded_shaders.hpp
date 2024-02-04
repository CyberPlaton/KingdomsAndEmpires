#pragma once

namespace sm
{
	namespace programs
	{
		//------------------------------------------------------------------------------------------------------------------------
		namespace pixelperfect
		{
			static inline constexpr const char* s_vs =
				"#version 330\n"
				"in vec3 vertPosition;\n"
				"in vec2 vertTexCoord;\n"
				"in vec4 vertColor;\n"
				"uniform mat4 mvp;\n"
				"uniform mat4 matModel;\n"
				"out vec3 fragPosition;\n"
				"out vec2 fragTexCoord;\n"
				"out vec4 fragColor;\n"
				"void main()\n"
				"{\n"
				"fragPosition = vec3(matModel*vec4(vertPosition, 1.0));\n"
				"fragTexCoord = vertTexCoord;\n"
				"fragColor = vertColor;\n"
				"gl_Position = mvp*vec4(vertPosition, 1.0);\n"
				"}\n";
			//@reference:- https://jorenjoestar.github.io/post/pixel_art_filtering/
			static inline constexpr const char* s_ps =
				"#version 330\n"
				"in vec3 fragPosition;\n"
				"in vec2 fragTexCoord;\n"
				"in vec4 fragColor;\n"
				"uniform sampler2D texture0;\n"
				"uniform vec4 colDiffuse;\n"
				"vec2 uv_klems( vec2 uv, ivec2 texture_size )\n"
				"{\n"
				"vec2 pixels = uv * texture_size + 0.5;\n"
				"vec2 fl = floor(pixels);\n"
				"vec2 fr = fract(pixels);\n"
				"vec2 aa = fwidth(pixels) * 0.75;\n"
				"fr = smoothstep( vec2(0.5, 0.5) - aa, vec2(0.5, 0.5) + aa, fr);\n"
				"return (fl + fr - 0.5) / texture_size;\n"
				"}\n"
				"void main()\n"
				"{\n"
				"vec2 texcoord = uv_klems(fragTexCoord, textureSize(texture0, 0));\n"
				"vec4 color = colDiffuse * texture2D(texture0, texcoord);\n"
				"if (color.w < 1.0/255.0)\n"
				"{\n"
				"discard;\n"
				"}\n"
				"gl_FragColor = color;\n"
				"}\n";

		} //- pixelperfect
		
		//------------------------------------------------------------------------------------------------------------------------
		namespace fxaa
		{
			static inline constexpr const char* s_ps =
				""
				""
				""
				""
				""
				""
				"";

		} //- fxaa

	} //- programs

} //- sm