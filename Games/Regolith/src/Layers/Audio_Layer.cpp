#include "Layers/Audio_Layer.h"

int Audio_Layer::Initialize()
{
	Audio->Load("Regolith/Master.strings.bank");
	Audio->Load("Regolith/Master.bank");
	Audio->Load("Regolith/Music.bank");
	Audio->Load("Regolith/Weapons.bank");
	Audio->Load("Regolith/Impacts.bank");
	Audio->Load("Regolith/Pickups.bank");
	Audio->Load("Regolith/UI.bank");

	m_timer.SetTime("event:/impacts/player_hit", 0.1f, .25f);
	m_timer.SetTime("event:/impacts/asteroid",   0.1f, .25f);

	m_timer.SetTime("event:/weapons/fire_cannon",  0.01f); // give these a little time for insurance against event spam
	m_timer.SetTime("event:/weapons/fire_laser",   0.01f);
	m_timer.SetTime("event:/weapons/fire_minigun", 0.01f);

	m_timer.SetTime("event:/ui/click", 0.01f);
	m_timer.SetTime("event:/ui/hover", 0.01f);

	m_timer.SetTime("event:/pickups/health", 0.01f);


	//m_timer.SetTime("metal hit",  0.1f, .25f);

	/*srand(time(nullptr));

	int i = iw::randi(7) + 1;
	std::stringstream ss;
	ss << "event:/music/bg" << i;

	Audio->Play(ss.str());

	Audio->SetVolume(Audio->GetHandle("vca:/music"), .1);
	Audio->SetVolume(Audio->GetHandle("vca:/effects"), .9);

#ifdef IW_DEBUG
	Audio->SetVolume(Audio->GetHandle("vca:/music"), .0);
	Audio->SetVolume(Audio->GetHandle("vca:/effects"), .5);
#endif*/

	return 0;
}

bool Audio_Layer::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case Actions::PLAY_SOUND:
		{
			PlaySound_Event& event = e.as<PlaySound_Event>();

			if (m_timer.Can(event.Name))
			{
				int handle = Audio->Play(event.Name);
				if (handle > 0)
				{
					for (const auto& [name, arg] : event.Parameters)
					{
						Audio->Set(handle, name, arg);
					}
				}
			}

			break;
		}

		// shoot
		//	main gun
		//  mini gun
		//  electric shock
		//  electric ball
		//  big laser
		//  small laser
		//  bomb explosion
		//  station spawnning
		// damage on player
		// player low health
		// player low core health
		// player core imminant explosion
		// player movement (tiny woosh)
		// player explodes

		// idk how levels are going to change yet, maybe every couple
		// of bars it tests for if the song should change and does a fill
		// then switches based on difficulty

		// there are going to be set boss encounters, so maybe there will be
		// two mins of normal play, then calm, then an intro will play
		// for a boss entrance and they will have some opener move 
		// and then the fight begins, and then procees into more fights for a
		// few minuets, or you kill everyone

	default:
		break;
	}

	return false;
}
