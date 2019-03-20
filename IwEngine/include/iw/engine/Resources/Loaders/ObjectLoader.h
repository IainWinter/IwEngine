#pragma once

#include "iw/engine/Resources/Object.h"
#include "iw/util/io/Loader.h"

template<>
IwEngine::Object IwUtil::Load<IwEngine::Object>(
	const char* fileName);
