#pragma once

#include "iw/engine/Window.h"
#include "iw/engine/InitOptions.h"
#include "iw/engine/Layer.h"

#ifdef IW_REMOTE_EDITOR
#	include "iw/editor/remote.h"
#endif

namespace iw {
namespace Engine {
	class Application {
	private:
		ref<IDevice> m_device;

		EventStack<Layer*> m_layers;

#ifdef IW_REMOTE_EDITOR
		RemoteSession m_remote;
#endif

		std::thread m_renderThread;
		bool m_running;
		bool m_isInitialized;
		float m_frameDelay;
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

		void Stop()
		{
			m_running = false;
		}

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
			PushLayer(layer);
			return layer;
		}

		template<
			typename L,
			typename... Args>
		L* PushLayerFront(
			Args&& ... args)
		{
			L* layer = new L(std::forward<Args>(args)...);
			PushLayerFront(layer);
			return layer;
		}

		template<
			typename L>
		void PushLayer(
			L* layer)
		{
			LOG_INFO << "Pushing " << layer->Name() << " layer";

			if (m_layers.Contains(layer))
			{
				LOG_WARNING << "\tFailed to push " << layer->Name() << " layer. Already on stack";
				return;
			}

			layer->SetAppVars(MakeAppVars());
			layer->OnPush();

			if (InitLayer(layer))
			{
				return;
			}

			m_layers.PushBack(layer);
		}

		template<
			typename L>
		void PushLayerFront(
			L* layer)
		{
			LOG_INFO << "Pushing " << layer->Name() << " layer to front";

			if (m_layers.Contains(layer))
			{
				LOG_WARNING << "\tFailed to push " << layer->Name() << " layer to front. Already on stack";
				return;
			}

			layer->SetAppVars(MakeAppVars());
			layer->OnPush();

			if (InitLayer(layer))
			{
				return;
			}

			m_layers.PushFront(layer);
		}

		template<
			typename L>
		void PopLayer(
			L* layer)
		{
			LOG_INFO << "Popping " << layer->Name() << " layer";
			m_layers.Pop(layer);
			layer->OnPop();
		}

		template<
			typename L>
		void DestroyLayer(
			L*/*&*/ layer)
		{
			LOG_INFO << "Destroying " << layer->Name() << " layer";
			m_layers.Pop(layer);
			layer->OnPop();
			layer->Destroy();
			delete layer;
			//layer = nullptr;
		}
	protected:
		inline EventSequence CreateSequence() {
			EventSequence seq;
			seq.SetAppVars(MakeAppVars());

			return seq;
		}
	private:
		bool InitLayer(
			Layer* layer)
		{
			LOG_DEBUG << "Initializing " << layer->Name() << " layer...";

			if (     m_isInitialized
				&& !layer->IsInitialized)
			{
				if (int err = layer->Initialize())
				{
					LOG_ERROR
						<< "Failed to init "
						<< layer->Name()
						<< " layer with error code "
						<< err;

					return true;
				}

				layer->IsInitialized = true;
			}

			return false;
		}

		MAKE_APP_VARS
	};
}

	using namespace Engine;
}

extern "C" __declspec(dllexport)
iw::Application* CreateApplication(
	iw::InitOptions& options);
