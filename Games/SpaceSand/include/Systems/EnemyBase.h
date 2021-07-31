#pragma once

#include "iw/engine/System.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/Flocker.h"

#include <vector>
#include <functional>

struct Ship;
struct Objective;
struct Command;

enum class ReportType {
	SPOTTED_PLAYER,
	FINISHED_OBJECTIVE,
	NEW_SHIP_UNDER_COMMAND/*,
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
			case ReportType::NEW_SHIP_UNDER_COMMAND: {
				TimeToProcess = .25;
				break;
			}
			default: {
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
		float x, float y,
		bool bubble = true
	)
		: Report(ReportType::SPOTTED_PLAYER)
		, x(x)
		, y(y)
	{}
};

struct FinishedObjectiveReport
	: Report
{
	iw::Entity ShipEntity;

	FinishedObjectiveReport(
		iw::Entity shipEntity
	)
		: Report(ReportType::FINISHED_OBJECTIVE)
		, ShipEntity(shipEntity)
	{}
};

struct NewShipUnderCommandReport
	: Report
{
	iw::Entity ShipEntity;

	NewShipUnderCommandReport(
		iw::Entity shipEntity
	)
		: Report(ReportType::NEW_SHIP_UNDER_COMMAND)
		, ShipEntity(shipEntity)
	{}
};

struct Objective {
	std::function<bool()> IsMet;
	std::function<void()> Update;
	std::function<void()> WhenDone;
};

struct Ship {
	std::vector<Objective*> Objectives;
	Command* Commander;

	float Speed = 25.0f;

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
	std::vector<iw::Entity> Ships;
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

	obj->IsMet = [=]()
	{
		glm::vec3 position = entity.Find<iw::Transform>()->Position;
		float dx = x - position.x;
		float dy = y - position.y;

		return dx * dx + dy * dy < r * r;
	};

	obj->Update = [=]()
	{
		entity.Find<Flocker>()->Target = glm::vec3(x, y, 0.f);
	};

	obj->WhenDone = [=]()
	{
		entity.Find<Ship>()->Commander->Report(new FinishedObjectiveReport(entity));
	};

	return obj;
}

Objective* MakeAssistShipObjective(
	iw::Entity entity,
	iw::Entity shipToAssistEntity)
{
	Objective* obj = new Objective();

	obj->Update = [=]()
	{
		glm::vec3 target = shipToAssistEntity.Find<iw::Transform>()->Position;
		entity.Find<Flocker>()->Target = glm::vec2(target.x, target.y);
	};

	return obj;
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
		auto makeBase = [&]()
		{
			iw::Entity baseEntity = sand->MakeTile("textures/SpaceGame/circle_temp.png", true);

			AddComponentToPhysicsEntity<Command>(baseEntity);
			AddComponentToPhysicsEntity<Base>(baseEntity);

			iw::Rigidbody* r = baseEntity.Find<iw::Rigidbody>();

			r->AngularVelocity.z = .01;
			r->SetMass(100000);
			r->Transform.Position.x = 100;
			//r->Transform.Scale.x = 2.1;
			//r->Transform.Scale.y = 2.1;

			return baseEntity;
		};

		auto makeShip = [&](Command* commander, bool isCommander)
		{
			iw::Entity shipEntity = sand->MakeTile("textures/SpaceGame/circle_temp.png", true);

			AddComponentToPhysicsEntity<Ship>(shipEntity, commander);
			Flocker* flocker = AddComponentToPhysicsEntity<Flocker>(shipEntity);

			if (isCommander) {
				AddComponentToPhysicsEntity<Command>(shipEntity);
			}

			if (commander) {
				commander->Report(new NewShipUnderCommandReport(shipEntity));
				flocker->ForceWeight = .0;
			}

			return shipEntity;
		};

		auto makeFighter = [&](Command* commander, bool isCommander = false)
		{
			iw::Entity shipEntity = makeShip(commander, isCommander);
			AddComponentToPhysicsEntity<FighterShip>(shipEntity);

			shipEntity.Find<iw::Rigidbody>()->Transform.Scale = glm::vec3(.2);
			shipEntity.Find<iw::Rigidbody>()->Transform.Position = glm::vec3(iw::randf() * 200, iw::randf() * 200, 0.f);

			return shipEntity;
		};

		auto makeMiner = [&](Command* commander, bool isCommander = false)
		{
			iw::Entity shipEntity = makeShip(commander, isCommander);
			AddComponentToPhysicsEntity<MinerShip>(shipEntity);

			shipEntity.Find<iw::Rigidbody>()->Transform.Position = glm::vec3(iw::randf() * 200, iw::randf() * 200, 0.f);

			return shipEntity;
		};

		iw::Entity mb = makeBase();
		Command* mb_c = mb.Find<Command>();

		//iw::Entity sb1 = Space->CreateEntity<Command, Base, Ship>();
		//Command* sb1_c = sb1.Set<Command>();
		//Ship*    sb1_s = sb1.Set<Ship>(mb_c);

		iw::Entity m1 = makeMiner(mb_c, true);
		Command* m1_c = m1.Find<Command>();

		makeFighter(m1_c);
		makeFighter(m1_c);
		makeFighter(m1_c);

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

				if (    objective->IsMet
					&& objective->IsMet())
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
				Report* r = command->Reports.at(i);
				r->Timer -= iw::DeltaTime();

				if (r->Timer < 0.0f)
				{
					switch (r->Type) {
						case ReportType::NEW_SHIP_UNDER_COMMAND:
						{
							NewShipUnderCommandReport* report = r->as<NewShipUnderCommandReport>();
							Ship* ship = report->ShipEntity.Find<Ship>();

							if (ship->Commander == command)
							{
								command->Ships.push_back(report->ShipEntity);
							}

							break;
						}
					}
				}
			}
		});

		// Handling repots is different for each type of ship
		Space->Query<Command, Base>().Each([&](
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
						case ReportType::NEW_SHIP_UNDER_COMMAND:
						{
							NewShipUnderCommandReport* report = r->as<NewShipUnderCommandReport>();

							if (report->ShipEntity.Find<Ship>()->Commander != command)
							{
								break;
							} // then can continue to finished objective only if no more data gets added to struct
						}
						case ReportType::FINISHED_OBJECTIVE:
						{
							FinishedObjectiveReport* report = r->as<FinishedObjectiveReport>();

							// logic for what the base should do when a ship complets an objective

							// give random location
							Ship* ship = report->ShipEntity.Find<Ship>();

							int x = iw::randf() * 500,
							    y = iw::randf() * 500,
							    r = 100;

							ship->AddObjective(MakeGotoLocationObjective(report->ShipEntity, x, y, r));

							int numbPointsInCircle = 200;

							// debug
							sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
							for (int i = 0; i < numbPointsInCircle; i++) {
								float a = i * iw::Pi / numbPointsInCircle;
								float ax = x + cos(a) * r;
								float ay = y + sin(a) * r;

								sand->m_world->SetCell(ax, ay, iw::Cell::GetDefault(iw::CellType::ROCK));
							}

							break;
						}
					}
				}
			}
		});

		iw::Space* space = Space.get();

		// Handling repots is different for each type of ship
		Space->Query<Command, Ship>().Each([space](
			iw::EntityHandle entity,
			Command* command,
			Ship* ship)
		{
			for (int i = 0; i < command->ReportCount(); i++)
			{
				Report* r = command->Reports.at(i);
				if (r->Timer <= 0.0f)
				{
					switch (r->Type)
					{
						case ReportType::NEW_SHIP_UNDER_COMMAND:
						{
							NewShipUnderCommandReport* report = r->as<NewShipUnderCommandReport>();
							Ship* ship = report->ShipEntity.Find<Ship>();

							if (ship->Commander == command)
							{
								ship->SetObjective(MakeAssistShipObjective(report->ShipEntity, iw::Entity(entity, space)));
							}

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
						LOG_INFO << "Report(" << (int)report->Type << ") bubbled up";
						report->Timer = report->TimeToProcess;
						ship->Commander->Report(report);
					}

					else {
						LOG_INFO << "Report(" << (int)report->Type << ") completed";

						delete report; // terminate report when at top of command
					}

					command->Reports.erase(command->Reports.begin() + i);
				}
			}
		});
	}
};
