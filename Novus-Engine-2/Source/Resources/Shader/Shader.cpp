#include "Shader.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <cassert>
#include "Utility/Memory/Memory.h"

using namespace std::experimental::filesystem::v1;
using std::chrono::system_clock;
using std::chrono::steady_clock;

namespace novus
{

bool ShaderBase::IsOutdated() const
{
	path localShaderPath(ShaderPath);

	assert(exists(localShaderPath));

	file_time_type modifyTime = last_write_time(localShaderPath);

	steady_clock::time_point lastCompileTimeSteadyOffset = LastCompileTime;
	lastCompileTimeSteadyOffset -= std::chrono::duration_cast<std::chrono::seconds>(steady_clock::now().time_since_epoch());

	//Convert the steady clock time to system time to avoid issues caused by the clock time changing from DST or the user
	system_clock::time_point lastCompileTime = system_clock::now() + std::chrono::duration_cast<std::chrono::seconds>(lastCompileTimeSteadyOffset.time_since_epoch());

	return lastCompileTime < modifyTime;
}

SHA1Hash ShaderBase::HashMacros(const ShaderMacro * macroArr, uint32_t macroCount)
{
	SHA1 hasher;

	std::vector<const ShaderMacro*> macroPtrs;

	macroPtrs.reserve(macroCount);

	for (unsigned int i = 0; i < macroCount; i++)
	{
		macroPtrs.push_back(&macroArr[i]);
	}

	//Sort the macros based on their name in alphebetical order, then definition if the names are the same
	std::sort(macroPtrs.begin(), macroPtrs.end(), [](const ShaderMacro* a, const ShaderMacro* b) {
		bool next = a->Name == b->Name;

		return (next ? (a->Definition < b->Definition) : (a->Name < b->Name));
	});

	//Hash name and definitons in the macro list
	for (auto & it : macroPtrs)
	{
		hasher.Update(reinterpret_cast<const unsigned char*>(it->Name.c_str()), static_cast<unsigned int>(it->Name.size()));
		hasher.Update(reinterpret_cast<const unsigned char*>(it->Definition.c_str()), static_cast<unsigned int>(it->Definition.size()));
	}

	hasher.Finalize();

	return hasher.GetHash();
}

}