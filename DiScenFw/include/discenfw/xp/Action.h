//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once


#include <memory>
#include <vector>
#include <string>

#include "discenfw/xp/ActionResult.h"

namespace discenfw
{
	namespace xp
	{
		/*!
		Action definition and results.

		@note Actions are compared only using their definition.
		Action results are computed after the action is executed.
		*/
		struct Action
		{
			/*!
			Action type encoded as a string (e.g. command).
			*/
			std::string TypeId;


			/*!
			Action parameters encoded as strings.
			*/
			std::vector<std::string> Params;


			/*!
			Default constructor (essential for action containers).
			*/
			Action()
			{
			}


			/*!
			Construct an action using its definition (essential if using initializers).
			*/
			Action(const std::string& typeId)
				: TypeId(typeId)
			{
			}


			/*!
			Construct an action using its definition (essential if using initializers).
			*/
			Action(const std::string& typeId, const std::vector<std::string>& params)
				: TypeId(typeId), Params(params)
			{
			}


			/*!
			Equality operator, only TypeId and Params are compared.
			*/
			bool operator == (const Action& action) const
			{
				return TypeId == action.TypeId && Params == action.Params;
			}


			/*!
			Equality operator, the encoded string is compared.
			*/
			bool operator == (const std::string& actionString) const
			{
				return ToString() == actionString;
			}


			/*!
			Check if the action is defined (false if invalid).
			*/
			bool Defined() const
			{
				return !TypeId.empty();
			}


			/*!
			Encode action type and parameters definition into a single string.
			*/
			std::string ToString() const;
		};


		inline std::string Action::ToString() const
		{
			std::string str = TypeId;
			for (const std::string& param : Params)
			{
				str += "|";
				str += param;
			}
			return str;
		}

	}
}

