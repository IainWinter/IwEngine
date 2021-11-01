#pragma once

#include "iw/common/Platform.h"

#ifdef IW_PLATFORM_WINDOWS
#	include <combaseapi.h>
#endif

#include <string>

struct iwGUID // from windows
{
	GUID Id;

	iwGUID()
	{
		CoCreateGuid(&Id);
	}

	iwGUID(
		const std::string& str)
	{
		sscanf(str.c_str(), "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
			(int*)&Id.Data1,    (int*)&Id.Data2,    (int*)&Id.Data3, 
			(int*)&Id.Data4[0], (int*)&Id.Data4[1], (int*)&Id.Data4[2], (int*)&Id.Data4[3],
			(int*)&Id.Data4[4], (int*)&Id.Data4[5], (int*)&Id.Data4[6], (int*)&Id.Data4[7]);
	}

	std::string str()
	{
		std::string str(100, '\0');
		CoCreateGuid(&Id);

		sprintf(str.data(), "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
			Id.Data1,    Id.Data2,    Id.Data3, 
			Id.Data4[0], Id.Data4[1], Id.Data4[2], Id.Data4[3],
			Id.Data4[4], Id.Data4[5], Id.Data4[6], Id.Data4[7]);

		return str;
	}
};
