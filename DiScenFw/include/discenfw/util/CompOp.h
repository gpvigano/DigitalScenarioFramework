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
#include <string>



namespace discenfw
{
	/*!
	Comparison operator selector.
	*/
	enum class CompOp
	{
		EQUAL,
		DIFFERENT,
		GREATER,
		GREATER_EQUAL,
		LESSER,
		LESSER_EQUAL,
		DEFINED
	};


	/*!
	Compare strings values.
	*/
	bool DISCENFW_API OpCompare(
		const std::string& propertyValue1,
		CompOp comparisonOperator,
		const std::string& propertyValue2);



	inline const char* CompOpToString(CompOp compOp)
	{
		switch (compOp)
		{
		case CompOp::EQUAL:
			return "==";
			break;
		case CompOp::DIFFERENT:
			return "!=";
			break;
		case CompOp::GREATER:
			return ">";
			break;
		case CompOp::GREATER_EQUAL:
			return ">=";
			break;
		case CompOp::LESSER:
			return "<";
			break;
		case CompOp::LESSER_EQUAL:
			return "<=";
			break;
		case CompOp::DEFINED:
			return "";
		}
		return "";
	}


	inline CompOp CompOpFromString(const std::string& compOpName)
	{
		if (compOpName == "==") return CompOp::EQUAL;
		if (compOpName == "!=") return CompOp::DIFFERENT;
		if (compOpName == ">") return CompOp::GREATER;
		if (compOpName == ">=") return CompOp::GREATER_EQUAL;
		if (compOpName == "<") return CompOp::LESSER;
		if (compOpName == "<=") return CompOp::LESSER_EQUAL;
		return CompOp::DEFINED;
	}

}

