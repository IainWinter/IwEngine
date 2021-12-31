#include "Layers/Menu_Title_Layer.h"
#include "iw/engine/Systems/Camera/EditorCameraControllerSystem.h"
#include "iw/engine/Systems/SpaceInspectorSystem.h"
#include "gl/glew.h"
#include "gl/wglew.h"

int Menu_Title_Layer::Initialize()
{
	iw::MeshDescription desc;
	desc.DescribeBuffer(iw::bName::POSITION,  iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::NORMAL,    iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::TANGENT,   iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::BITANGENT, iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::UV,        iw::MakeLayout<float>(2));

	iw::Mesh mesh = iw::MakeUvSphere(desc, 48, 48)->MakeInstance();
	mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/test/pbr_noise.shader"));
	Renderer->Now->InitShader(mesh.Material->Shader, iw::CAMERA | iw::LIGHTS | iw::SHADOWS);

	mesh.Material->Set("albedo", iw::Color(1, .26, 0));
	mesh.Material->Set("metallic", .73f);
	mesh.Material->Set("roughness", .87f);
	mesh.Material->Set("reflectance", 0.0f);
	mesh.Material->Set("ao", 0.0f);
	mesh.Data->GenTangents();

	ball = Space->CreateEntity<iw::Transform, iw::Mesh>();
	ball.Set<iw::Transform>(glm::vec3(1.15, -3.75, .7), glm::vec3(3.f));
	ball.Set<iw::Mesh>(mesh);

	MainScene->SetAmbiance(.03);
	MainScene->AddLight(new iw::PointLight());
	MainScene->PointLights().at(0)->SetPosition(glm::vec3(.77, .52, .8));
	
	iw::EditorCameraControllerSystem* camController = PushSystem<iw::EditorCameraControllerSystem>();
	camController->MakeOrthoOnInit = true;
	camController->Initialize();
	iw::OrthographicCamera* cam = (iw::OrthographicCamera*)camController->GetCamera();
	cam->Width  = 4;
	cam->Height = 5;
	cam->Transform.Parent()->Rotation = glm::quat(0, 0, 0, 0);
	cam->Transform.Parent()->Position = glm::vec3(0, 0, 10);
	cam->RecalculateView();

	Renderer->Now->Device->SetClearColor(0, 0, 0, 1);

	MainScene->SetMainCamera(camController->GetCamera());

	//iw::ISystem* sis = PushSystem<iw::SpaceInspectorSystem>();
	//sis->Initialize();

	// axis guide
	/*
	glm::vec3 pos[6] =
	{
		glm::vec3(0, 0, 0), glm::vec3(1, 0, 0),
		glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)
	};
	glm::vec3 col[6] =
	{
		glm::vec3(1, 0, 0), glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0), glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1), glm::vec3(0, 0, 1),
	};
	unsigned idx[6] =
	{
		0, 1, 2, 3, 4, 5
	};
	iw::MeshDescription axis_desc;
	axis_desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	axis_desc.DescribeBuffer(iw::bName::COLOR,    iw::MakeLayout<float>(3));
	iw::MeshData* axis_data = new iw::MeshData(axis_desc);
	axis_data->SetBufferData(iw::bName::POSITION, 6, pos);
	axis_data->SetBufferData(iw::bName::COLOR,    6, col);
	axis_data->SetIndexData(6, idx);
	axis_data->SetTopology(iw::MeshTopology::LINES);

	iw::Mesh axis_mesh = axis_data->MakeInstance();
	axis_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/test/axis_color.shader"));
	Renderer->Now->InitShader(axis_mesh.Material->Shader, iw::CAMERA);

	axis = Space->CreateEntity<iw::Transform, iw::Mesh>();
	axis.Set<iw::Transform>(glm::vec3(.8, 1.2, 10), glm::vec3(1.f));
	axis.Set<iw::Mesh>(axis_mesh);
	*/

	// title

	iw::Mesh title_mesh = Asset->Load<iw::Model>("models/SpaceGame/title_regolith.glb")->GetMesh(0);
	title_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/texture_cam.shader"));
	title_mesh.Material->Set("color", iw::Color(1));
	Renderer->Now->InitShader(title_mesh.Material->Shader, iw::CAMERA);

	title = Space->CreateEntity<iw::Transform, iw::Mesh>();
	title.Set<iw::Transform>(glm::vec3(-1.88, 1.446, 0), glm::vec3(1.5f));
	title.Set<iw::Mesh>(title_mesh);

	//stars

	glm::vec3 star_pos[1] =
	{
		glm::vec3(0, 0, 0)
	};
	unsigned star_idx[1] =
	{
		0
	};
	iw::MeshDescription star_desc;
	star_desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	iw::MeshData* star_data = new iw::MeshData(star_desc);
	star_data->SetBufferData(iw::bName::POSITION, 1, star_pos);
	star_data->SetIndexData(1, star_idx);
	star_data->SetTopology(iw::MeshTopology::POINTS);

	iw::Mesh star_mesh = star_data->MakeInstance();
	star_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/particle/color_cam.shader"));
	star_mesh.Material->Set("color", iw::Color(1));
	Renderer->Now->InitShader(star_mesh.Material->Shader, iw::CAMERA);

	iw::StaticPS ps;
	ps.SetParticleMesh(star_mesh);

	for (int i = 0; i < 100; i++)
	{
		ps.SpawnParticle(iw::Transform(
			glm::vec3(iw::randfs() * 2, iw::randfs() * 2.5, 0),
			glm::vec3(.01f)
		));
	}

	stars = Space->CreateEntity<iw::Transform, iw::StaticPS>();
	stars.Set<iw::Transform>();
	iw::StaticPS* p = stars.Set<iw::StaticPS>(ps);

	p->Update();
	p->UpdateParticleMesh();

	bg = REF<iw::RenderTarget>();
	bg->AddTexture(REF<iw::Texture>(Renderer->Width(), Renderer->Height()));
	bg->AddTexture(REF<iw::Texture>(Renderer->Width(), Renderer->Height(), iw::TEX_2D, iw::DEPTH));

	//wglSwapIntervalEXT(1);

	return 0;
}

void Menu_Title_Layer::UI()
{
	ImGui::GetIO().FontGlobalScale = bg_w / 400;

	ImGui::SetNextWindowPos (ImVec2(bg_x + bg_w / 6, bg_h / 2));
	ImGui::SetNextWindowSize(ImVec2(-1, -1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
	ImGui::Begin("Main Title Buttons", nullptr, commonFlags);
	{
		if (ImGui::Button("Play")) Console->QueueCommand("set-state play");
		ImGui::Button("Highscores");
		ImGui::Button("Settings");
		ImGui::Button("Credits");
	}
	ImGui::End();
	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(4);

	iw::ITexture* tex = bg->Tex(0)->Handle();
	if (tex)
	{
		ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
		ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::Begin("Main Title Background", nullptr, commonFlags);
		{
			ImGui::Image((void*)tex->Id(), ImVec2(100, 100));
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	Renderer->BeginScene(MainScene/*, bg, true*/);
	Renderer->	DrawMesh(ball .Find<iw::Transform>(), ball  .Find<iw::Mesh>());
	//Renderer->	DrawMesh(axis .Find<iw::Transform>(), axis  .Find<iw::Mesh>());
	Renderer->	DrawMesh(title.Find<iw::Transform>(), title .Find<iw::Mesh>());
	Renderer->	DrawMesh(stars.Find<iw::Transform>(), &stars.Find<iw::StaticPS>()->GetParticleMesh());
	Renderer->EndScene();
}
