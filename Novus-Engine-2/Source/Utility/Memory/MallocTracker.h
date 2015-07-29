/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include <string>
#include <map>
#include <mutex>

namespace novus
{

class MallocTracker
{
	struct MemAllocation
	{
		size_t Size;
		std::string FileName;
		std::string FunctionName;
		int LineNum;
	};

public:
	/**
	 *	Get the singleton instance of the MallocTracker
	 */
	static MallocTracker* GetInstance();

	/**
	 *	Track the specified point in memory.
	 *	@param p The pointer to memory to track
	 *	@param size The size allocated in bytes
	 *	@param fileName The name of the file that the memory was allocated in, use __FILE__ for this parameter
	 *	@param functionName The name of the function the memory was allocated in, use __FUNCTION__ for this parameter in MSVC
	 *	@param lineNum The line number in the file where the memory was allocated, use __LINE__ for this parameter
	 */
	void Alloc(void* p, size_t Size, const char* FileName, const char* FunctionName, int LineNum);

	/**
	 *	Untrack the specified point in memory.
	 *	@param p The pointer to memory to track
	 *	@param fileName The name of the file that the memory was deleted in, use __FILE__ for this parameter
	 *	@param functionName The name of the function the memory was deleted in, use __FUNCTION__ for this parameter in MSVC
	 *	@param lineNum The line number in the file where the memory was deleted, use __LINE__ for this parameter
	 */
	bool Free(void* p, const char* FileName, const char* FunctionName, int LineNum);

	/**
	 *	Print out the current amount of used memory to the IDE's debugger console
	 */
	void DumpTrackedMemory();

	/**
	 *	Get total number of bytes allocated
	 */
	size_t GetUsedMemory() const { return TotalMemory; }

private:
	MallocTracker();
	~MallocTracker() {}

	MallocTracker(const MallocTracker&) {}
	MallocTracker& operator= (const MallocTracker&) {}

private:
	static MallocTracker* StaticInstance;

	std::mutex AllocLock;

	std::map<void*, MemAllocation> Allocations;

	size_t TotalMemory;
};

};