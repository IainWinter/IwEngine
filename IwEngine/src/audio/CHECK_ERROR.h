#pragma once

#ifdef IW_DEBUG
	#define CHECK_ERROR(stmt, msg, n) if(CheckError(stmt, msg)) return n;
#else
	#define CHECK_ERROR(stmt, msg, n) stmt
#endif
