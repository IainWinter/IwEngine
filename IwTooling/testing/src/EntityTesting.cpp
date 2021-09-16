// This shouldnt be needed idk why space::trysend event functions activate with define
#define IW_USE_EVENTS

#include "iw/entity/Space.h"
#include "iw/log/sink/std_sink.h"
#include <iostream>

int main()
{
#ifdef IW_USE_EVENTS
	std::cout << "test";
#endif


	LOG_SINK(iw::log::stdout_sink, iw::log::loglevel::INFO);

	iw::Space space;

	std::vector<iw::EntityHandle> handles;

	for (int i = 0; i < 100000000; i++)
	{
		int random = rand() % 3;
		if (handles.size() == 0) random = 0;

		switch (random)
		{
			case 0: 
			case 1: 
			{
				handles.push_back(space.CreateEntity<int>().Handle);
				break;
			}
			case 2:
			{
				size_t index = rand() % handles.size();
				
				iw::EntityHandle handle = handles.at(index); 
				space.DestroyEntity(handle); 

				handles.at(index) = handles.back(); handles.pop_back();

				break;
			}
		}
	}
}
