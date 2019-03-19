#pragma once

#include "iw/engine/Resources/Obj.h"
#include "iw/util/io/Loader.h"

template<>
IwEngine::Obj IwUtil::Load<IwEngine::Obj>(
	const char* fileName);
