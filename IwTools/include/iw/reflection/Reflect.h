#pragma once

#if !defined(__clang__) && !defined(__GNUC__)
	#ifdef __attribute__ 
		# undef __attribute__ 
	#endif
	#define __attribute__(a)
	#define REFLECTING 
#endif

#define REFLECT __attribute__((annotate("reflect")))
