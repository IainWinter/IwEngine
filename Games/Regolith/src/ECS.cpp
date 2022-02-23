#include "ECS.h"

entity_manager& entities()
{
	static entity_manager manager;
	return manager;
}

#undef entities_defer
command_buffer& entities_defer(const char* where_from)
{
	static command_buffer buffer(&entities());
	buffer.m_where_current = where_from;
	return buffer;
}
