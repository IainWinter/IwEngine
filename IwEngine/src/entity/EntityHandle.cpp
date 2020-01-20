#include "iw/entity/EntityHandle.h"

namespace IW {
	const EntityHandle EntityHandle::Empty = EntityHandle{ size_t(-1), short(-1), bool(-1) };
}
