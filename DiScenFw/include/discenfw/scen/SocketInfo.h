//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/scen/CommonData.h"

#include <string>
#include <vector>

namespace discenfw
{
	/*!
	Information about socket used for physical (and logical) connections between elements.
	*/
	struct SocketInfo
	{
		//! Identifier of the socket.
		std::string Id;

		//! Type of socket, used to test connection compatibility.
		std::string Type;

		//! Position and orientation of the socket.
		CoordSys3D Location;

		//! List of socket types compatible with (connectible to) this socket.
		std::vector<std::string> Compatibility;
	};
}
