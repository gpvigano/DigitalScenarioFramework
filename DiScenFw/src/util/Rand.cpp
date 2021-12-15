//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/util/Rand.h>

#if defined(__GNUC__) && defined(_CODE_BLOCKS) && !defined(_WIN64)
    #pragma GCC warning "The 32 bit configuration could not work with MinGW 64bit (see comment below)."

// Note: with MinGW-w64, x86 configuration, as installed along with Code::Blocks,
// we get the following error: expected unqualified-id before '__int128'
// and 32 bit libraries are not found.
// The toolchain installed along with Code::Blocks-mingw (64 bit) cannot build 32 bit.
// You need to install the 32 bit toolchain yourself and point CodeBlocks to use that one to build for 32 bit.
// See this link for details:
// https://forums.codeblocks.org/index.php/topic,23817.msg162483.html#msg162483

#endif

#include <random>

namespace
{
	// This is used to obtain a seed for the random number engine
	static std::random_device rd;

	// Standard mersenne_twister_engine seeded with rd()
	static std::mt19937 gRandomEngine(rd());
}


namespace discenfw
{

	int RandInt(int first, int last)
	{
		std::uniform_int_distribution<int> dis(first, last);
		return dis(gRandomEngine);
	}


	float RandFloat(float first, float last)
	{
		std::uniform_real_distribution<float> dis(first, last);
		return dis(gRandomEngine);
	}


	int RandIndex(int vectorSize)
	{
		return RandInt(0, vectorSize - 1);
	}
}

