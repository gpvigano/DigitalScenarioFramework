//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/PropertyCondition.h>
#include <discenfw/util/CompOp.h>

namespace discenfw
{
	namespace xp
	{
		bool PropertyCondition::Evaluate(const EntityState& entityState) const
		{
			bool defined = entityState.HasProperty(PropertyName);
			if (ComparisonOperator == CompOp::DEFINED)
			{
				return defined;
			}
			if (defined)
			{
				return OpCompare(entityState.PropertyValues.at(PropertyName),
					ComparisonOperator, PropertyValue);
			}

			return false;
		}

	} // namespace xp
} // namespace discenfw

