#include "Systems/Score_System.h"

bool ScoreSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case RUN_GAME:
		{
			Score = 0;
			break;
		}
		case CORE_EXPLODED:
		{
			CoreExploded_Event& event = e.as<CoreExploded_Event>();

			if (event.Entity.Has<EnemyShip>())
			{
				Score += 10;
			}

			break;
		}
		case END_GAME:
		{
			LOG_TRACE << "Final score " << Score << "!";

			break;
		}
	}

	return false;
}
