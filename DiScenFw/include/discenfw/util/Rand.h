//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFwConfig.h>

namespace discenfw
{

	/*!
	Obtain a random integer between first and last.
	*/
	DISCENFW_API int RandInt(int first, int last);


	/*!
	Obtain a random floating point number between first and last.
	*/
	DISCENFW_API float RandFloat(float first, float last);


	/*!
	Obtain a random index for the given vector size.
	*/
	DISCENFW_API int RandIndex(int vectorSize);

}
