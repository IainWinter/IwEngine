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

	bool Scene::AddLight(
		PointLight* pointLight)
	{
		bool noLight = AddLight((Light*)pointLight);

		if (!noLight) {
			LOG_WARNING << "Tried to add duplicate directional light!";
		}

		else {
			m_pointLights.push_back(pointLight);
		}

		return noLight;
	}

	bool Scene::AddLight(
		DirectionalLight* directionalLight)
	{
		bool noLight = AddLight((Light*)directionalLight);

		if (!noLight) {
			LOG_WARNING << "Tried to add duplicate directional light!";
		}

		else {
			m_directionalLights.push_back(directionalLight);
		}

		return noLight;
	}

	bool Scene::AddLight(
		Light* light)
	{
		auto itr = std::find(m_lights.begin(), m_lights.end(), light);

		bool foundLight = itr != m_lights.end();

		if (foundLight) {
			LOG_WARNING << "Tried to add duplicate light!";
		}

		else {
			m_lights.push_back(light);
		}

		return !foundLight;
	}

	bool Scene::RemoveLight(
		const Light* light)
	{
		auto itr  = std::find(m_lights.begin(), m_lights.end(), light);
		auto itrp = std::find(m_pointLights.begin(), m_pointLights.end(), light);
		auto itrd = std::find(m_directionalLights.begin(), m_directionalLights.end(), light);

		if (itrp != m_pointLights.end()) {
			m_pointLights.erase(itrp);
		}

		else if (itrd != m_directionalLights.end()) {
			m_directionalLights.erase(itrd);
		}

		bool foundLight = itr != m_lights.end();

		if (foundLight) {
			m_lights.erase(itr);
		}

		else {
			LOG_WARNING << "Tried to remove light that does not exists!";
		}

		return foundLight;
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
