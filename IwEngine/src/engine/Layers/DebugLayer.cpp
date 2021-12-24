#include "iw/engine/Layers/DebugLayer.h"
#include "iw/common/Components/Transform.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include <gl\glew.h>
#include "iw/log/logger.h"

// To make a shield
// make each move need a part of the tank
// re 2 type system where each upgrade is a size / each upgrade adds a small part
// bind one of 4 buttons to the moves

// each move can have a special if it has a full tank

// dash   , wind / fuel -> full tank blasts enemies away in some form?
// shield , 

namespace iw {
namespace Engine {
	DebugLayer::DebugLayer()
		: Layer           ("Debug")
		, m_smoothing     (1.f)
		, m_removeOld     (10.f)
		, m_skippedFrames (0)
		, m_frames        (0)
		, m_eventCount    (0)
	{}

	void DebugLayer::PrintTimes()
	{
		ImGui::SliderFloat("Smooth",          &m_smoothing,     0,  1, "%1.2f", .5);
		ImGui::SliderFloat("Remove Old Time", &m_removeOld,     0, 20, "%1.2f", .5);
		//ImGui::SliderInt  ("Speed",           &m_skippedFrames, 0, 50, "%d");
		
		m_frames += 1;
		if (m_frames > m_skippedFrames)
		{
			m_frames = 0;
			_enumerateTree(LOG_GET_TIMES());
		}

		if (ImGui::CollapsingHeader("Flat View"))
		{
			std::vector<std::string> remove;

			std::vector<
				const std::pair<
					const std::string, 
					std::tuple<
						float, 
						float, 
						std::vector<float>
					>
				>*
			> times;

			for (const auto& pair : m_times)
			{
				times.push_back(&pair);
			}

			std::sort(times.begin(), times.end(), [](
				const auto& a, 
				const auto& b)
				{
					return std::get<1>(a->second) > std::get<1>(b->second);
					//return a->first.compare(b->first);
				}
			);

			for (auto* pair : times)
			{
				const auto& [name, item] = *pair;
				const auto& [lastadd, time, list] = item;

				if (iw::TotalTime() - lastadd > m_removeOld)
				{
					remove.push_back(name);
				}

				ImGui::PlotLines("", list.data(), list.size());
				ImGui::SameLine();
				ImGui::Text("%4.4fms - %s", time, name.c_str());
			}

			for (const std::string& name : remove)
			{
				m_times.erase(name);
			}
		}
	}

	void DebugLayer::_enumerateTree(
		const iw::log_time& times)
	{
		auto& [lastadd, time, list] = m_times[times.name];

		lastadd = iw::TotalTime();

		if (list.size() > 200)
		{
			list.erase(list.begin());
		}

		time = iw::lerp(time, times.time * 1000, iw::min(1 - m_smoothing + .01f, 1.f));
		list.push_back(time);

		if (times.children.size() == 0)
		{
			//ImGui::PlotLines("", list.data(), list.size());
			//ImGui::SameLine();
			ImGui::Text("+ %4.4fms - %s", time, times.name.c_str());
		}

		else
		{
			if (ImGui::TreeNode("###", "%4.4fms - %s", time, times.name.c_str()))
			{
				for (const iw::log_time& child : times.children)
				{
					_enumerateTree(child);
				}

				ImGui::TreePop();
			}
		}
	}

	void DebugLayer::ImGui() {
		ImGui::Begin("Debug layer");
		{
			m_eventCount = iw::max<int>(m_eventCount, logger::instance().get_values("event_count").size());
			
			ImGui::Text("Number of events in queue: %d", m_eventCount);

			ImGui::PlotLines(
				"", 
				logger::instance().get_values("event_count").data(), 
				logger::instance().get_values("event_count").size());

			float time = Time::RawFixedTime();
			ImGui::SliderFloat("Fixed timestep", &time, 0.001f, 1);

			float scale = Time::TimeScale();
			ImGui::SliderFloat("Timescale", &scale, 0, 5);

			ImGui::Text("Scaled Delta time (%2.4f)", Time::DeltaTime());
			ImGui::Text("Scaled Fixed time (%2.4f)", Time::FixedTime());

			ImGui::Text("Raw Delta time (%2.4f)", Time::RawDeltaTime());
			ImGui::Text("Raw Fixed time (%2.4f)", Time::RawFixedTime());


			if (time  != Time::RawFixedTime()) Time::SetFixedTime(time);
			if (scale != Time::TimeScale())    Time::SetTimeScale(scale);

			while (m_logs.size() > 20) {
				m_logs.erase(m_logs.begin());
			}

			ImGui::Text("Last 20 Events");
		
			ImGui::BeginTable("Logs", 1);
			for (std::string log : m_logs) {
				ImGui::TableNextRow();
				ImGui::Text(log.c_str());
			}
			ImGui::EndTable();
		}
		ImGui::End();

		ImGui::Begin("Times");
		{
			PrintTimes();
		}
		ImGui::End();
	}

	bool DebugLayer::On(
		MouseWheelEvent& e)
	{
		std::stringstream log;
		log << "Mouse wheel .... delta: " << e.Delta << " Name: " << iw::val(e.Device);
		m_logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		MouseMovedEvent& e)
	{
		std::stringstream log;
		log << "Mouse moved .... X: " << e.X << " Y: " << e.Y << " dX: " << e.DeltaX << " dY: " << e.DeltaY << " Name: " << iw::val(e.Device);
		m_logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		MouseButtonEvent& e)
	{
		std::stringstream log;
		log << "Mouse button ... Button: " << e.Button << " State: " << e.State << " Name: " << iw::val(e.Device);
		m_logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		KeyEvent& e)
	{
		std::stringstream log;
		log << "Key button ..... Button: " << e.Button << " State: " << e.State << " Name: " << iw::val(e.Device);
		m_logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		KeyTypedEvent& e)
	{
		std::stringstream log;
		log << "Key types ...... Button: " << e.Button << " Char: " << e.Character << " Name: " << iw::val(e.Device);
		m_logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		WindowResizedEvent& e)
	{
		std::stringstream log;
		log << "Window resized . W: " << e.Width << " H: " << e.Height;
		m_logs.push_back(log.str());
		return false;
	}
}
}
