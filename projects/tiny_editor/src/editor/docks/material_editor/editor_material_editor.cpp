#include "editor_material_editor.hpp"
#include "../../elements/editor_element_window.hpp"
/*#include "../../nodes/editor_basic_node.hpp"*/

namespace editor
{
	namespace
	{
		static const cnode_function::sfunction_data C_ADD_FUNCTION_DATA = 
		{
			{"float", "float"},
			"+",
			"float",
			cnode_function::function_type_binary_operation
		};

		static raylib::RenderTexture2D S_RENDER_TARGET;
		static raylib::Texture2D S_TEXTURE;
		static raylib::Shader S_SHADER;
		static float S_SECONDS = 0;
		static int S_SECONDS_LOC = 0;
		static int S_TEXTURE_LOC = 0;
		constexpr auto C_VS =
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
			"}\n"
			;

		constexpr auto C_PS =
			"#version 330"
			"in vec2 fragTexCoord;"
			"in vec4 fragColor;"
			"uniform sampler2D texture0;"
			"uniform vec4 colDiffuse;"
			"out vec4 finalColor;"
			"uniform float seconds;"
			"uniform vec2 size;"
			"uniform float freqX;"
			"uniform float freqY;"
			"uniform float ampX;"
			"uniform float ampY;"
			"uniform float speedX;"
			"uniform float speedY;"
			"void main() {"
			"float pixelWidth = 1.0 / size.x;"
			"float pixelHeight = 1.0 / size.y;"
			"float aspect = pixelHeight / pixelWidth;"
			"float boxLeft = 0.0;"
			"float boxTop = 0.0;"

			"vec2 p = fragTexCoord;"
			"p.x += cos((fragTexCoord.y - boxTop) * freqX / (pixelWidth * 750.0) + (seconds * speedX)) * ampX * pixelWidth;"
			"p.y += sin((fragTexCoord.x - boxLeft) * freqY * aspect / (pixelHeight * 750.0) + (seconds * speedY)) * ampY * pixelHeight;"

			"finalColor = texture(texture0, p) * colDiffuse * fragColor;"
			"}"
			;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::cmaterial_editor(scontext& ctx) :
		clayer_base("##material_editor", ctx), m_editor(ImNodes::CreateEditor())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::~cmaterial_editor()
	{
		ImNodes::DestroyEditor(m_editor);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmaterial_editor::init()
	{
		m_graph = std::make_shared<cmaterial_graph>();

		auto add = m_graph->create_node<cnode_function>(C_ADD_FUNCTION_DATA);
		auto constant_a = m_graph->create_node<cnode_constant>("A", 1.0f);
		auto constant_b = m_graph->create_node<cnode_constant>("B", 1.0f);
		auto master_node = m_graph->create_node<cnode_master>();

		m_graph->create_link(constant_a->id(), add->id(), 0, 0);
		m_graph->create_link(constant_b->id(), add->id(), 0, 1);

		//- Testing drawing stuff and playing with shaders
		S_RENDER_TARGET = raylib::LoadRenderTexture(500, 500);
		S_SHADER = raylib::LoadShaderFromMemory(C_VS, C_PS);
		auto cwd = core::cfilesystem::cwd();
		cwd = cwd.parent().parent();
		const auto path = cwd.path().string();
		S_TEXTURE = raylib::LoadTexture(fmt::format("{}/resources/figure_paladin_14.png", path).data());

		S_SECONDS_LOC = raylib::GetShaderLocation(S_SHADER, "seconds");
		S_TEXTURE_LOC= raylib::GetShaderLocation(S_SHADER, "texture0");
		int freqXLoc = raylib::GetShaderLocation(S_SHADER, "freqX");
		int freqYLoc = raylib::GetShaderLocation(S_SHADER, "freqY");
		int ampXLoc = raylib::GetShaderLocation(S_SHADER, "ampX");
		int ampYLoc = raylib::GetShaderLocation(S_SHADER, "ampY");
		int speedXLoc = raylib::GetShaderLocation(S_SHADER, "speedX");
		int speedYLoc = raylib::GetShaderLocation(S_SHADER, "speedY");

		// Shader uniform values that can be updated at any time
		float freqX = 500.0f;
		float freqY = 25.0f;
		float ampX = 50.0f;
		float ampY = 10.0f;
		float speedX = 25.0f;
		float speedY = 22.0f;

		float screenSize[2] = { (float)S_TEXTURE.width, (float)S_TEXTURE.height };
		raylib::SetShaderValue(S_SHADER, raylib::GetShaderLocation(S_SHADER, "size"), &screenSize, raylib::SHADER_UNIFORM_VEC2);
		raylib::SetShaderValue(S_SHADER, freqXLoc, &freqX, raylib::SHADER_UNIFORM_FLOAT);
		raylib::SetShaderValue(S_SHADER, freqYLoc, &freqY, raylib::SHADER_UNIFORM_FLOAT);
		raylib::SetShaderValue(S_SHADER, ampXLoc, &ampX, raylib::SHADER_UNIFORM_FLOAT);
		raylib::SetShaderValue(S_SHADER, ampYLoc, &ampY, raylib::SHADER_UNIFORM_FLOAT);
		raylib::SetShaderValue(S_SHADER, speedXLoc, &speedX, raylib::SHADER_UNIFORM_FLOAT);
		raylib::SetShaderValue(S_SHADER, speedYLoc, &speedY, raylib::SHADER_UNIFORM_FLOAT);

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::shutdown()
	{
		raylib::UnloadRenderTexture(S_RENDER_TARGET);
		raylib::UnloadShader(S_SHADER);
		raylib::UnloadTexture(S_TEXTURE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::on_update(float dt)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::on_ui_render()
	{
		//- Testing drawing stuff and playing with shaders
		if (const auto scope = imgui::cwindow_scope("Shaders Window", &active()))
		{
			if (raylib::IsRenderTextureReady(S_RENDER_TARGET))
			{
				imgui::cui::image(&S_RENDER_TARGET.texture, {500, 500});
			}
		}

		return;

		static const auto calcPinId = [](const int nodeId, const int type) -> int
			{
				if (type == 0)
				{
					return (nodeId + 1) << 15;
				}
				return (nodeId + 1) << 10;
			};

		if (ImGui::Begin("##MaterialEditor"))
		{
			if (ImGui::Button("Generate"))
			{
				cmaterial_generator generator(m_graph);

				generator.generate();
			}

			CORE_ASSERT(m_editor, "Invalid operation. Node editor context was not created!");

			ImNodes::SetCurrentEditor(m_editor);

			ImNodes::Begin("Node Editor");

			for (auto& node : m_graph->nodes())
			{
				ImNodes::BeginNode(node->id());
				ImGui::TextUnformatted(fmt::format("Node {}", node->id()).data());

				//- Inputs
				for (auto& in : node->inputs())
				{
					imnodes::cpin pin(in.m_id);

					pin
						.type(imnodes::cpin::pin_type_input)
						.begin();

					ImGui::TextUnformatted(fmt::format("{}", in.m_name.data()).data());

					pin.end();
				}

				//- Outputs
				for (auto& out : node->outputs())
				{
					imnodes::cpin pin(out.m_id);

					pin
						.type(imnodes::cpin::pin_type_output)
						.begin();

					ImGui::TextUnformatted(fmt::format("{}", out.m_name.data()).data());

					pin.end();
				}

				ImNodes::EndNode();
			}

			for (auto& link : m_graph->links())
			{
				imnodes::clink(link.m_id, link.m_from_slot, link.m_to_slot).draw();
			}

			ImNodes::End();
		}
		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::on_world_render()
	{
		S_SECONDS += raylib::GetFrameTime() * 100.0f;
		raylib::SetShaderValue(S_SHADER, S_SECONDS_LOC, &S_SECONDS, raylib::SHADER_UNIFORM_FLOAT);
		raylib::SetShaderValue(S_SHADER, S_TEXTURE_LOC, &S_TEXTURE, raylib::RL_SHADER_UNIFORM_SAMPLER2D);

		raylib::Camera2D camera{raylib::Vector2{250.0f, 250.0f}, raylib::Vector2{0.0f, 0.0f}, 0.0f, 1.0f};

		raylib::BeginTextureMode(S_RENDER_TARGET);
		raylib::ClearBackground(raylib::WHITE);
		raylib::BeginBlendMode(raylib::BLEND_ALPHA);
		raylib::BeginMode2D(camera);

		if (raylib::IsShaderReady(S_SHADER))
		{
			raylib::BeginShaderMode(S_SHADER);
		}

		raylib::DrawTextureEx(S_TEXTURE, { 10, 10 }, S_SECONDS, 0.4f, raylib::WHITE);
		raylib::DrawTextureEx(S_TEXTURE, { 10, 10 }, S_SECONDS, 0.33f, raylib::RED);

		raylib::EndShaderMode();
		raylib::EndMode2D();
		raylib::EndTextureMode();
	}

} //- editor