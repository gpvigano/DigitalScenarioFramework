//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/util/CompOp.h>

#include <limits>

namespace discenfw
{

	bool OpCompare(
		const std::string& propertyValue1,
		CompOp comparisonOperator,
		const std::string& propertyValue2)
	{
		if (comparisonOperator == CompOp::DEFINED)
		{
			return propertyValue1.empty() == propertyValue2.empty();
		}
		auto canBeNumber = [](const std::string& valStr)
		{
			if (valStr.empty()) return false;
			if (!isdigit(valStr[0]) && valStr[0] != '-') return false;
			return true;
		};

		if (canBeNumber(propertyValue1) && canBeNumber(propertyValue2))
		{
			auto asInt = [](const std::string& valStr, int& val)
			{
				try
				{
					val = std::stoi(valStr);
					return true;
				}
				catch (const std::exception&)
				{
				}
				return false;
			};

			int intVal1;
			int intVal2;
			if (asInt(propertyValue1, intVal1) && asInt(propertyValue2, intVal2))
			{
				switch (comparisonOperator)
				{
				case CompOp::EQUAL:
					return intVal1 == intVal2;
				case CompOp::DIFFERENT:
					return intVal1 != intVal2;
				case CompOp::GREATER:
					return intVal1 > intVal2;
				case CompOp::GREATER_EQUAL:
					return intVal1 >= intVal2;
				case CompOp::LESSER:
					return intVal1 < intVal2;
				case CompOp::LESSER_EQUAL:
					return intVal1 <= intVal2;
				case CompOp::DEFINED:
					return true;
				default:
					return false;
				}
			}

			auto asDouble = [](const std::string& valStr, double& val)
			{
				try
				{
					val = std::stod(valStr);
					return true;
				}
				catch (const std::exception&)
				{
				}
				return false;
			};

			double doubleVal1;
			double doubleVal2;

			if (asDouble(propertyValue1, doubleVal1) && asDouble(propertyValue2, doubleVal2))
			{
				switch (comparisonOperator)
				{
				case CompOp::EQUAL:
					return doubleVal1 == doubleVal2;
				case CompOp::DIFFERENT:
					return doubleVal1 != doubleVal2;
				case CompOp::GREATER:
					return doubleVal1 > doubleVal2;
				case CompOp::GREATER_EQUAL:
					return doubleVal1 >= doubleVal2;
				case CompOp::LESSER:
					return doubleVal1 < doubleVal2;
				case CompOp::LESSER_EQUAL:
					return doubleVal1 <= doubleVal2;
				case CompOp::DEFINED:
					return true;
				default:
					return false;
				}
			}
		}

		switch (comparisonOperator)
		{
		case CompOp::EQUAL:
			return propertyValue1 == propertyValue2;
		case CompOp::DIFFERENT:
			return propertyValue1 != propertyValue2;
		case CompOp::GREATER:
			return propertyValue1 > propertyValue2;
		case CompOp::GREATER_EQUAL:
			return propertyValue1 >= propertyValue2;
		case CompOp::LESSER:
			return propertyValue1 < propertyValue2;
		case CompOp::LESSER_EQUAL:
			return propertyValue1 <= propertyValue2;
		case CompOp::DEFINED:
			return true;
		default:
			break;
		}
		return false;
	}

} // namespace discenfw

