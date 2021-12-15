//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <string>

namespace discenfw_test
{

	///@addtogroup TestUtil
	///@{

	//! Highlight differences between two strings.
	std::string DiffString(const std::string& str1, const std::string& str2);

	///@}
}
