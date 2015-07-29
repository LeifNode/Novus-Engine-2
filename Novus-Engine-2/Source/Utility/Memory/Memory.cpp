#include "Memory.h"
#include "MallocTracker.h"
#include <cassert>

void * operator new(size_t Size, const char* FileName, const char* FunctionName, int line)
{
	void* mem = ::operator new(Size);
	assert(mem != nullptr);

	novus::MallocTracker::GetInstance()->Alloc(mem, Size, FileName, FunctionName, line);

	return mem;
}

void * operator new[](size_t Size, const char* FileName, const char* FunctionName, int line)
{
	void* mem = ::operator new[](Size);
	assert(mem != nullptr);

	novus::MallocTracker::GetInstance()->Alloc(mem, Size, FileName, FunctionName, line);

	return mem;
}

void novus::detail::AllocTracker_Free(void * p, const char* FileName, const char* FunctionName, int line)
{
	novus::MallocTracker::GetInstance()->Free(p, FileName, FunctionName, line);
}