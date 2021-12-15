//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/FeatureCondition.h>
#include <discenfw/util/CompOp.h>

namespace discenfw
{
	namespace xp
	{
		bool FeatureCondition::Evaluate(const EnvironmentState& state) const
		{
			bool defined = state.HasFeature(FeatureName);
			if (ComparisonOperator == CompOp::DEFINED)
			{
				return defined;
			}
			if (defined)
			{
				return OpCompare(state.GetFeature(FeatureName),
					ComparisonOperator, FeatureValue);
			}

			return false;
		}

	} // namespace xp
} // namespace discenfw

