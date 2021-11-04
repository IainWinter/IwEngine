#include "Layers/Audio_Layer.h"

void Audio_Layer::OnPush()
{
	m_console = Console->AddHandler(
		[this](const iw::Command& command)
		{
			if (command.Verb == "game-start")
			{
				// play game music

				if (m_song == -1)
				{
					m_song = Audio->CreateInstance("music_bg", false);
				}

				else
				{
					Audio->StartInstance(m_song);
				}
			}

			else if (command.Verb == "game-over")
			{
				Audio->StopInstance(m_song);
				// play game over music
			}

			else if (command.Verb == "game-reform")
			{ 
				// play reforming music
			}

			else if (command.Verb == "quit")
			{
				// play quit game music
			}

			return false;
		}
	);
}

void Audio_Layer::OnPop()
{
	Console->RemoveHandler(m_console);
}

bool Audio_Layer::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
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
