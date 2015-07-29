#include "MallocTracker.h"
#include "Utility/Logging/Logger.h"
#include <Windows.h>
#include <sstream>

namespace novus
{

MallocTracker* MallocTracker::StaticInstance = nullptr;

MallocTracker* MallocTracker::GetInstance()
{
	if (StaticInstance == nullptr)
	{
		StaticInstance = new MallocTracker();
	}

	return StaticInstance;
}

MallocTracker::MallocTracker()
	:TotalMemory(0)
{
}

void MallocTracker::Alloc(void * p, size_t Size, const char * FileName, const char * FunctionName, int LineNum)
{
	MemAllocation memAlloc;
	memAlloc.Size = Size;
	memAlloc.FileName = FileName;
	memAlloc.FunctionName = FunctionName;
	memAlloc.LineNum = LineNum;

	std::lock_guard<std::mutex> lock(AllocLock);

	Allocations.insert(std::pair<void*, MemAllocation>(p, memAlloc));

	TotalMemory += Size;
}

bool MallocTracker::Free(void * p, const char * FileName, const char * FunctionName, int LineNum)
{
	std::lock_guard<std::mutex> lock(AllocLock);

	auto it = Allocations.find(p);

	if (it != Allocations.end())
	{
		TotalMemory -= it->second.Size;

		Allocations.erase(it);

		return true;
	}
	else if (p != nullptr)
	{
		wchar_t error[256];
		swprintf_s(error, 256, L"%s(%i): Could not find matching allocation %z.\n", FileName, LineNum, p);

		NE_WARN(error, L"MallocTracker");
		OutputDebugString(error);
	}

	return false;
}

void MallocTracker::DumpTrackedMemory()
{
	std::lock_guard<std::mutex> lock(AllocLock);

	wchar_t sizeStr[32] = { 0 };

	swprintf_s(sizeStr, 32, L"%fKB\n", TotalMemory / 1024.0f);

	OutputDebugString(L"Total memory allocated: ");
	OutputDebugString(sizeStr);
	OutputDebugString(L"Remaining memory allocations:\n");

	for (auto&& a: Allocations)
	{
		std::wstringstream debugString;
		
		std::wstring sizeType = L"B";
		float adjustedSize = static_cast<float>(a.second.Size);
		if (adjustedSize > 512.0f)
		{
			adjustedSize /= 1024.0f;
			sizeType = L"KB";
		}
		if (adjustedSize > 512.0f)
		{
			adjustedSize /= 1024.0f;
			sizeType = L"MB";
		}

		debugString << a.second.FileName.c_str() << L"(" << a.second.LineNum << L"): " << adjustedSize << sizeType << L" allocated in function " << a.second.FunctionName.c_str() << std::endl;

		OutputDebugString(debugString.str().c_str());
	}
}

};