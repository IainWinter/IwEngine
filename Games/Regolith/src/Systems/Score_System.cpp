#include "Systems/Score_System.h"

bool ScoreSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();

			if (event.Config.Hit.Has<Enemy>())
			{
				Bus->push<ChangePlayerScore_Event>(Score + 1);
			}

			break;
		}
		case CORE_EXPLODED:
		{
			CoreExploded_Event& event = e.as<CoreExploded_Event>();
			
			if (event.Entity.Has<Enemy>())
			{
				Bus->push<ChangePlayerScore_Event>(Score + 100);
			}

			break;
		}
		case CHANGE_PLAYER_SCORE:
		{
			int newScore = e.as<ChangePlayerScore_Event>().Score; // validation of highscores could be done
			LOG_INFO << newScore;                                 // by comparing fesability based on all past games
			Score = newScore;
			break;
		}
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case GAME_START_STATE:
					Score = 0;
					break;
				case GAME_OVER_STATE:
					LOG_TRACE << "Final score " << Score << "!";
					break;
			}

			break;
		}
	}

	return false;
}
