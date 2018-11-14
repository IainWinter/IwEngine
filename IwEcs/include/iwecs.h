#pragma once

#ifdef IWECS_EXPORT
#	define IWECS_API __declspec(dllexport)
#else
#	define IWECS_API __declspec(dllimport)
#endif

namespace iwecs {
	using entity = unsigned int;
}




/*
ecs_manager
 |- component_registry
     |- map of archtype id to component_data
 component_data
  |- vector of chunks





*/