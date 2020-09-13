#pragma once

#ifdef IW_DEBUG
	#define CHECK_ERROR(stmt, msg, n) stmt
#else
	#define CHECK_ERROR(stmt, msg, n) if(CheckError(stmt, msg)) return n;
#endif
