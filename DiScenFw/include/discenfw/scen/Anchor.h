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
	Information for anchoring points.
	*/
	struct Anchor
	{
		Vector3D Offset; //! Offset from the anchor point (in local coordinates).
		std::string ElementId; //! Element to which the socket is relative (absolute if it is not defined).
		std::string SocketId; //! Identifier of the socket (optional).

		/// @name Constructors
		///@{

		Anchor()
		{
		}

		Anchor(const Vector3D& offset, const std::string& anchorElementId = "", const std::string& anchorSocketId = "")
			: Offset(offset), ElementId(anchorElementId), SocketId(anchorSocketId)
		{
		}

		///@}
	};

}
