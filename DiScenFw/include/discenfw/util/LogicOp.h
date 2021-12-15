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


namespace discenfw
{
	/*!
	Logic operator enumeration.
	*/
	enum class LogicOp
	{
		AND,
		OR,
		AND_NOT,
		OR_NOT,
		XOR
	};


	inline std::string LogicOpToString(LogicOp logicOp)
	{
		switch (logicOp)
		{
		case LogicOp::AND:
			return "and";
		case LogicOp::OR:
			return "or";
		case LogicOp::AND_NOT:
			return "and not";
		case LogicOp::OR_NOT:
			return "or not";
		case LogicOp::XOR:
			return "xor";
		}
		return "";
	}


	inline LogicOp LogicOpFromString(const std::string& logicOpName)
	{
		if (logicOpName == "and") return LogicOp::AND;
		if (logicOpName == "or") return LogicOp::OR;
		if (logicOpName == "and not") return LogicOp::AND_NOT;
		if (logicOpName == "or not") return LogicOp::OR_NOT;
		return LogicOp::XOR;
	}
}
