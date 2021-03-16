#include "App.h"
#include "Events/ActionEvents.h"
#include "iw/physics/Plane.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "Layers/SimpleSandLayer.h"
#include "Layers/ToolLayer.h"

#include "iw/util/io/File.h"

#include <stack>

namespace iw {

	//struct token {
	//	std::string content;
	//	std::string front;
	//	std::string back;
	//	std::vector<token*> children;
	//};

	//int indexOfConsumer = 0;

	//char consume(std::string& str) {
	//	if (indexOfConsumer == str.size()) return '\0';

	//	char c = str.at(indexOfConsumer++);
	//	return c;
	//}

	//char consume(std::string& str, std::string& fill) {
	//	if (indexOfConsumer == str.size()) return '\0';

	//	char c = str.at(indexOfConsumer++);
	//	fill.push_back(c);
	//	return c;
	//}

	//char peek(std::string& str, int off = 0) {
	//	return str.at(indexOfConsumer + off);
	//}

	//token* lex(token* tok, std::string& str) {
	//	while (indexOfConsumer < str.size()) {

	//		char c = peek(str);

	//		if (c == '<') {
	//			// closing tag

	//			if (peek(str, 1) == '/') {
	//				do {
	//					c = consume(str, tok->back);
	//				} while (c != '>');

	//				break;
	//			}
	//			
	//			token* child = new token();
	//			tok->children.push_back(child);

	//			// opening tag
	//			do {
	//				c = consume(str, child->front);
	//			} while (c != '>');

	//			lex(child, str);
	//		}

	//		else {
	//			// content

	//			token* child = new token();
	//			tok->children.push_back(child);

	//			do {
	//				consume(str, child->content);
	//			} while (peek(str) != '<');
	//		}
	//	}
	//	
	//	return tok;
	//}

	//std::string remake(token* tok) {
	//	std::string str = tok->front + tok->content;
	//	for (token* token : tok->children) {
	//		str += remake(token);
	//	}
	//	str += tok->back;

	//	return str;
	//}

	//int indexOfDiv = 0;

	//void addDiv(token* tok, size_t from, size_t to, std::string name = "", std::string classes = "", std::string style = "") {
	//	token* div = new token();
	//	div->front = "<div id='code" + name + std::to_string(indexOfDiv++) + "' class='" + classes + "' style='" + style + "'>";
	//	div->back = "</div>";

	//	for (size_t i = from; i <= to; i++) {
	//		div->children.push_back(tok->children.at(i));
	//	}

	//	tok->children[from] = div;

	//	for (size_t i = to; i > from; i--) {
	//		tok->children.erase(tok->children.begin() + i);
	//	}
	//}

	App::App() {
		Audio = REF<AudioSpaceStudio>("assets/sounds/");

		iw::ref<Context> context = Input->CreateContext("Editor");
		
		context->MapButton(iw::SPACE, "+jump");
		context->MapButton(iw::SHIFT, "-jump");
		context->MapButton(iw::D    , "+right");
		context->MapButton(iw::A    , "-right");
		context->MapButton(iw::W    , "+forward");
		context->MapButton(iw::S    , "-forward");
		context->MapButton(iw::C    , "use");
		context->MapButton(iw::T    , "toolbox");
		context->MapButton(iw::I    , "imgui");

		iw::ref<Device> m  = Input->CreateDevice<Mouse>();
		//iw::ref<Device> rm = Input->CreateDevice<RawMouse>();
		iw::ref<Device> k  = Input->CreateDevice<RawKeyboard>();
		
		context->AddDevice(m);
		//context->AddDevice(rm);
		context->AddDevice(k);

		sandbox = PushLayer<SimpleSandLayer>(); // model system breaks in test layer
		imgui   = PushLayer<ImGuiLayer>(/*Window()*/);
	}

	int App::Initialize(
		iw::InitOptions& options)
	{
		int err = Application::Initialize(options);

		//token* root = new token();
		//lex(root, iw::ReadFile("F:/Documents/_WINTER/ANimation/sand/2/fix.html"));

		//token* code2 = root->children[0]->children[1]->children[1]->children[3]->children[1]->children[1]->children[3]->children[1];

		//addDiv(code2, 75, 159, "2", "noDisplay");
		//addDiv(code2, 76, 107, "2", "showI");
		//addDiv(code2, 22, 28, "2", "showI");
		//addDiv(code2, 23, 36, "2", "noDisplay");

		//iw::WriteFile("F:/Documents/_WINTER/ANimation/sand/2/remake.html", remake(root));

		//return -1;

		if (!err) {
			ImGuiLayer* imgui = GetLayer<ImGuiLayer>("ImGui");
			if (imgui) {
				imgui->BindContext();
			}
		}

		toolbox = PushLayer<ToolLayer>(sandbox->GetMainScene());
		err = toolbox->Initialize();

		PopLayer(toolbox);
		PopLayer(imgui);

		return err;
	}

	void App::Update() {
		if (GetLayer("Toolbox") != nullptr) {
			//sandbox->Update();
			//sandbox->FixedUpdate();
			sandbox->UpdateSystems();
		}

		Application::Update();
	}

	void Editor::App::FixedUpdate() {
		//if (GetLayer("Toolbox") == nullptr) {
			Application::FixedUpdate();
		//}
	}

	bool App::HandleCommand(
		const Command& command)
	{
		if (command.Verb == "jump") {
			Bus->push<JumpEvent>(command.Active);
		}

		else if (command.Verb == "right") {
			Bus->push<RightEvent>(command.Active);
		}

		else if (command.Verb == "forward") {
			Bus->push<ForwardEvent>(command.Active);
		}

		else if (command.Verb == "use") {
			Bus->push<UseEvent>();
		}

		else if (command.Verb == "toolbox") {
			bool dev = GetLayer("Toolbox") != nullptr;
			if (dev) {
				PopLayer(toolbox);
				PushLayer(sandbox);
			}

			else {
				PushLayer(toolbox);
				PopLayer(sandbox);
			}

			Bus->send<DevConsoleEvent>(dev);
		}

		else if (command.Verb == "imgui") {
			if (GetLayer("ImGui") == nullptr) {
				PushLayer(imgui);
				Bus->push<WindowResizedEvent>(Window()->Id(), Renderer->Width(), Renderer->Height());
			}

			else {
				PopLayer(imgui);
			}
		}

		return Application::HandleCommand(command);
	}
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		1920/**4/3*/,
		1080/**4/3*/,
		true,
		iw::DisplayState::NORMAL
	};
	
	return new iw::App();
}
