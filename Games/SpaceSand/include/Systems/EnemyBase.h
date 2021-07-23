#pragma once

#include "iw/engine/System.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

#include <vector>
#include <functional>

struct Ship;
struct Objective;
struct Command;

enum class ShipType {
	FIGHTER,
	MINER,
	SUPPLY,
	TRANSPORT
};

enum class ReportType {
	SPOTTED_PLAYER,
	FINISHED_OBJECTIVE/*,
	FINISHED_MINING*/
};

struct Report {
	ReportType Type;
	float Timer; // counts down to 0
	float TimeToProcess;

	Report(ReportType type)
		: Type(type)
	{
		switch (type) {
			case ReportType::SPOTTED_PLAYER: {
				TimeToProcess = Timer = (iw::randf()+2) * 2.5f;
				break;
			}
		}
	}

	template<
		typename _t>
	_t* as() {
		return /*dynamic_cast<*/(_t*)/*>*/(this);
	}
};

struct PlayerSpottedReport
	: Report
{
	float x, y;

	PlayerSpottedReport(
		float x, float y
	)
		: Report(ReportType::SPOTTED_PLAYER)
		, x(x)
		, y(y)
	{}
};

struct FinishedObjectiveReport
	: Report
{
	Ship* ship;

	FinishedObjectiveReport(
		Ship* ship
	)
		: Report(ReportType::FINISHED_OBJECTIVE)
		, ship(ship)
	{}
};

struct Objective {
	std::function<bool()> IsMet;
	std::function<void()> Update;
	std::function<void()> WhenDone;
};

struct Ship {
	ShipType Type;
	std::vector<Objective*> Objectives;
	Command* Commander;

	Ship()
		: Commander(nullptr)
		//, CurrentObjective(nullptr)
	{}

	Ship(
		Command* commander
	)
		: Commander(commander)
		//, CurrentObjective(nullptr)
	{}

	Objective* CurrentObjective() {
		if (Objectives.size() == 0) return nullptr;
		return Objectives.front();
	}

	void AddObjective(Objective* objective) {
		Objectives.push_back(objective);
	}

	void SetObjective(Objective* objective) {
		Objectives.insert(Objectives.begin(), objective);
	}

	void ResetObjectives() {
		Objectives.clear();
	}

	void RemoveObjective(Objective* objective) {
		Objectives.erase(std::find(Objectives.begin(), Objectives.end(), objective));
	}
};

struct Command {
	std::vector<Ship*> Ships;
	//std::vector<Objective*> Objectives;
	std::vector<Report*> Reports;
	int ReporterCount; // how many reports can be handled at once

	Command(int reporterCount = 1)
		: ReporterCount(reporterCount)
	{}

	// return the number of reports to handle at once
	int ReportCount() {
		return iw::min<size_t>(Reports.size(), ReporterCount);
	}

	void Report(Report* report) {
		Reports.push_back(report);
	}
};

struct Base {
	float Rez;
};

struct MinerShip {
	float CollectedRez;
};

struct FighterShip {

};

// make functions

Objective* MakeGotoLocationObjective(
	iw::Entity entity,
	float x, float y, float r)
{
	Objective* obj = new Objective();

	auto delta = [=]() {
		iw::Transform* transform = entity.Find<iw::Transform>();
		float dx = x - transform->Position.x;
		float dy = y - transform->Position.y;

		return std::make_pair(dx, dy);
	};

	obj->IsMet = [=]()
	{
		auto [dx, dy] = delta();
		return dx * dx + dy * dy < r * r;
	};

	obj->Update = [=]()
	{
		auto [dx, dy] = delta();
		entity.Find<iw::Rigidbody>()->Velocity = glm::normalize(glm::vec3(dx, dy, 0)) * 25.0f;
	};

	obj->WhenDone = [=]()
	{
		Ship* ship = entity.Find<Ship>();
		ship->Commander->Report(new FinishedObjectiveReport(ship));

		entity.Find<iw::Rigidbody>()->Velocity = glm::vec3(0);
	};

	return obj;
}

// helper functions
template<
	typename _t,
	typename _i>
void remove(std::vector<_t>& v, _i& i)
{
	v.at(i) = v.back(); v.pop_back(); i--;
}					

struct EnemyCommandSystem : iw::SystemBase
{
	iw::SandLayer* sand;

	EnemyCommandSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Enemy command")
		, sand(sand)
	{}

	int Initialize() override
	{
		iw::Entity mb = Space->CreateEntity<Command, Base>();
		Command* mb_c = mb.Set<Command>();

		iw::Entity sb1 = Space->CreateEntity<Command, Ship>();
		Command* sb1_c = sb1.Set<Command>();
		Ship*    sb1_s = sb1.Set<Ship>(mb_c);

		iw::Entity s1 = Space->CreateEntity<Command, Ship, MinerShip>();
		Command*   s1_c  = s1.Set<Command>();
		Ship*      s1_s  = s1.Set<Ship>(sb1_c);
		MinerShip* s1_ms = s1.Set<MinerShip>();

		iw::Entity   f1 = Space->CreateEntity<Ship, FighterShip>();
		Ship*        f1_s  = f1.Set<Ship>(s1_c);
		FighterShip* f1_fs = f1.Set<FighterShip>();

		iw::Entity   f2 = Space->CreateEntity<Ship, FighterShip>();
		Ship*        f2_s  = f2.Set<Ship>(s1_c);
		FighterShip* f2_fs = f2.Set<FighterShip>();

		iw::Entity   f3 = sand->MakeTile("textures/SpaceGame/circle_temp.png", true);
		FighterShip* f3_fs = AddComponentToPhysicsEntity<FighterShip>(f3);
		Ship*        f3_s  = AddComponentToPhysicsEntity<Ship>(f3, s1_c);

		mb_c->Ships.push_back(sb1_s);

		s1_c->Ships.push_back(f1_s);
		s1_c->Ships.push_back(f2_s);
		s1_c->Ships.push_back(f3_s);

		f3_s->Commander->Report(new PlayerSpottedReport(100, 100));

		s1_s->SetObjective(MakeGotoLocationObjective(f3, 50, 50, 20));

		return 0;
	}

	void Update() override
	{
		// Report takes time for a Commander to process
		Space->Query<Ship>().Each([](
			iw::EntityHandle e,
			Ship* ship)
		{
			Objective* objective = ship->CurrentObjective();

			if (objective)
			{
				objective->Update();
				if (objective->IsMet())
				{
					LOG_INFO << "Objective completed";

					objective->WhenDone();
					ship->RemoveObjective(objective);
					delete objective;
				}
			}
		});

		// Report takes time for a Commander to process
		Space->Query<Command>().Each([](
			iw::EntityHandle e,
			Command* command)
		{
			// each commander can handle a differnt number of reports at once

			for (int i = 0; i < command->ReportCount(); i++)
			{
				command->Reports.at(i)->Timer -= iw::DeltaTime();
			}
		});

		// Handling repots is different for each type of ship
		Space->Query<Command, Base>().Each([](
			iw::EntityHandle entity,
			Command* command,
			Base* base)
		{
			for (int i = 0; i < command->ReportCount(); i++)
			{
				Report* r = command->Reports.at(i);
				if (r->Timer <= 0.0f)
				{
					switch (r->Type)
					{
						case ReportType::SPOTTED_PLAYER:
						{
							PlayerSpottedReport* report = r->as<PlayerSpottedReport>();

							// logic for what the base should do when a player is spotted

							break;
						}
						case ReportType::FINISHED_OBJECTIVE:
						{
							FinishedObjectiveReport* report = r->as<FinishedObjectiveReport>();

							// logic for what the base should do when a ship complets an objective

							break;
						}
					}
				}
			}
		});

		// Handling repots is different for each type of ship
		Space->Query<Command, Ship, MinerShip>().Each([](
			iw::EntityHandle entity,
			Command* command,
			Ship* ship,
			MinerShip* miner)
		{
			for (int i = 0; i < command->ReportCount(); i++)
			{
				Report* r = command->Reports.at(i);
				if (r->Timer <= 0.0f)
				{
					switch (r->Type)
					{
						case ReportType::SPOTTED_PLAYER:
						{
							PlayerSpottedReport* report = r->as<PlayerSpottedReport>();

							// logic for what the mining ship wants to do when any of its ships sees a player
							// if rez is high flee to base fully with more than the ships it leaves to fight the player
							// if rez is med flee to base or if it becomes safe on the way, go to another asteroid with only 1 and send more to the player
							// if rez is low flee to another asteroid and send all ships to fight the player?

							break;
						}
					}
				}
			}
		});

		// Bubbling up reports to a ships commander
		Space->Query<Command>().Each([&](
			iw::EntityHandle entity,
			Command* command)
		{
			for (int i = 0; i < command->ReportCount(); i++)
			{
				Report* report = command->Reports.at(i);
				if (report->Timer <= 0.0f)
				{
					if (Ship* ship = Space->FindComponent<Ship>(entity))
					{
						LOG_INFO << "Report(" << report << ") bubbled up";
						report->Timer = report->TimeToProcess;
						ship->Commander->Report(report);
					}

					else {
						LOG_INFO << "Report(" << report << ") completed";

						delete report; // terminate report when at top of command
					}

					remove(command->Reports, i);
				}
			}
		});
	}
};
