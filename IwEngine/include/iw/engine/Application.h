#pragma once

#include "iw/engine/Window.h"
#include "iw/engine/InitOptions.h"
#include "iw/engine/Layer.h"

namespace iw {
namespace Engine {
	class Application {
	private:
		IWindow* m_window;
		ref<IDevice> m_device;

		EventStack<Layer*> m_layers;

		std::thread m_renderThread;
		bool m_running;
	public:
		APP_VARS

	public:
		IWENGINE_API
		Application();

		IWENGINE_API
		virtual ~Application();

		IWENGINE_API
		virtual int Initialize(
			InitOptions& options);

		IWENGINE_API
		virtual void Run();

		IWENGINE_API
		virtual void Destroy();

		IWENGINE_API
		virtual void Update();

		IWENGINE_API
		virtual void FixedUpdate();
		//virtual void Pause();

		IWENGINE_API
		virtual void HandleEvent(
			iw::event& e);

		IWENGINE_API
		virtual bool HandleCommand(
			const Command& command);

		IWENGINE_API
		virtual void HandleCollision(
			Manifold& manifold,
			scalar dt);

		IWENGINE_API inline const IWindow* Window() const { return m_window; }
		IWENGINE_API inline       IWindow* Window()       { return m_window; }

		template<
			typename L = Layer>
		L* GetLayer(
			const char* name)
		{
			return (L*)m_layers.Get(name);
		}

		template<
			typename L,
			typename... Args>
		L* PushLayer(
			Args&&... args)
		{
			L* layer = new L(std::forward<Args>(args)...);
			layer->SetAppVars(MakeAppVars());

			PushLayer(layer);
			return layer;
		}

		template<
			typename L,
			typename... Args>
		L* PushOverlay(
			Args&& ... args)
		{
			L* layer = new L(std::forward<Args>(args)...);
			layer->SetAppVars(MakeAppVars());

			PushOverlay(layer);
			return layer;
		}

		template<
			typename L>
		void PushLayer(
			L* layer)
		{
			LOG_INFO << "Pushed " << layer->Name() << " layer";
			layer->SetAppVars(MakeAppVars());
			m_layers.PushBack(layer);

			layer->OnPush();
		}

		template<
			typename L>
		void PushOverlay(
			L* layer)
		{
			LOG_INFO << "Pushed " << layer->Name() << " overlay";
			layer->SetAppVars(MakeAppVars());
			m_layers.PushFront(layer);

			layer->OnPush();
		}

		template<
			typename L>
		void PopLayer(
			L* layer)
		{
			LOG_INFO << "Popped " << layer->Name() << " layer";
			m_layers.Pop(layer);

			layer->OnPop();
		}
	protected:
		inline EventSequence CreateSequence() {
			EventSequence seq;
			seq.SetAppVars(MakeAppVars());

			return seq;
		}
	private:
		MAKE_APP_VARS
	};
}

	using namespace Engine;
}

iw::Application* CreateApplication(
	iw::InitOptions& options);
