#pragma once

#ifdef IW_DEBUG
	#define CHECK_ERROR(err, msg, n) if(CheckError(err, msg)) return n;
#else
	#define CHECK_ERROR(err, msg, n)
#endif
