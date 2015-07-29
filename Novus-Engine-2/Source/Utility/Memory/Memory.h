/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#ifdef DEBUG
#define TRACK_MALLOC
#endif

void * operator new(size_t size, const char* fileName, const char* functionName, int line);
void * operator new[](size_t size, const char* fileName, const char* functionName, int line);

namespace novus
{

namespace detail
{
	void AllocTracker_Free(void * p, const char* fileName, const char* functionName, int line);
}

}

#ifdef TRACK_MALLOC
	#define NE_NEW new(__FILE__, __FUNCTION__, __LINE__)
	#define NE_DELETE(ptr) novus::detail::AllocTracker_Free(ptr, __FILE__, __FUNCTION__, __LINE__), delete ptr, ptr = 0
	#define NE_DELETEARR(ptr) novus::detail::AllocTracker_Free(ptr, __FILE__, __FUNCTION__, __LINE__), delete [] ptr, ptr = 0
#else
	#define NE_NEW new
	#define NE_DELETE(ptr) delete ptr, ptr = 0
	#define NE_DELETEARR(ptr) delete [] ptr, ptr = 0
#endif