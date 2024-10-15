#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
	//- A shader. Can be a single vertex or fragment shader or a combination of them, becoming a program
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final : public core::cresource
	{
	public:
		static void destroy(cshader& shader);

		explicit cshader(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath);
		explicit cshader(shader_type type, const char* vs, const char* fs);
		explicit cshader(shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size);
		cshader();
		~cshader();

		opresult load_from_file(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath);
		opresult load_from_string(shader_type type, const char* vs, const char* fs);
		opresult load_from_memory(shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size);

		inline raylib::Shader shader() const { return m_shader; }
		inline shader_type type() const { return m_type; }

		void set_uniform_float(stringview_t name, float value);
		void set_uniform_int(stringview_t name, int value);
		void set_uniform_vec2(stringview_t name, const vec2_t& value);
		void set_uniform_vec3(stringview_t name, const vec3_t& value);
		void set_uniform_vec4(stringview_t name, const vec4_t& value);
		void set_uniform_matrix(stringview_t name, const mat4_t& value);
		void set_uniform_texture(stringview_t name, const ctexture& value);
		void remove_uniform(stringview_t name);

		cshader& operator=(const cshader& other);

	private:
		raylib::Shader m_shader;
		shader_type m_type;

		RTTR_ENABLE(core::cresource);
	};

} //- sm