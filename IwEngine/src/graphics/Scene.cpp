#include "iw/graphics/Scene.h"

namespace iw {
namespace Graphics {
	Scene::Scene() 
		: m_camera(nullptr)
		, m_ambiance(0.03f)
	{}

	void Scene::SetMainCamera(
		Camera* camera)
	{
		m_camera = camera;
	}

	void Scene::SetAmbiance(
		float ambiance)
	{
		m_ambiance = ambiance;
	}

	void Scene::AddLight(
		PointLight* pointLight)
	{
		auto itr = std::find(m_pointLights.begin(), m_pointLights.end(), pointLight);

		if (itr != m_pointLights.end()) {
			LOG_WARNING << "Tried to add duplicate light!";
			return;
		}

		m_pointLights.push_back(pointLight);
		m_lights     .push_back(pointLight);
	}

	void Scene::AddLight(
		DirectionalLight* pointLight)
	{
		auto itr = std::find(m_directionalLights.begin(), m_directionalLights.end(), pointLight);

		if (itr != m_directionalLights.end()) {
			LOG_WARNING << "Tried to add duplicate light!";
			return;
		}

		m_directionalLights.push_back(pointLight);
		m_lights           .push_back(pointLight);
	}

	void Scene::RemoveLight(
		const Light* light)
	{
		auto itr  = std::find(m_lights.begin(), m_lights.end(), light);
		auto itrp = std::find(m_pointLights.begin(), m_pointLights.end(), light);
		auto itrd = std::find(m_directionalLights.begin(), m_directionalLights.end(), light);

		if (itrp != m_pointLights.end()) {
			m_lights     .erase(itr);
			m_pointLights.erase(itrp);
		}

		else if (itrd != m_directionalLights.end()) {
			m_lights           .erase(itr);
			m_directionalLights.erase(itrd);
		}

		LOG_WARNING << "Tried to remove light that does not exists!";
	}

	Camera* Scene::MainCamera() {
		return m_camera;
	}

	float& Scene::Ambiance() {
		return m_ambiance;
	}

	std::vector<Light*> Scene::Lights() {
		return m_lights;
	}

	std::vector<PointLight*> Scene::PointLights() {
		return m_pointLights;
	}

	std::vector<DirectionalLight*> Scene::DirectionalLights() {
		return m_directionalLights;
	}
}
}
