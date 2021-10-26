#pragma once

#ifdef IW_PLATFORM_WINDOWS

#ifdef _WIN32
#	define _WIN32_WINNT 0x0A00
#endif
#	define WIN32_LEAN_AND_MEAN // this is needed for asio to not give error about winsock.h?
#	include <Windows.h>
#endif
