//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


#include "discenfw/xp/EnvironmentState.h"
#include <discenfw/xp/EnvironmentModel.h>

namespace discenfw
{
	namespace xp
	{


		std::shared_ptr<EnvironmentState> EnvironmentState::Make()
		{
			return std::make_shared<EnvironmentState>();
		}


		EnvironmentState::EnvironmentState()
		{
		}


		EnvironmentState::EnvironmentState(const std::map<std::string, std::shared_ptr<EntityState>>& entityStates)
			: EntityStates(entityStates)
		{
		}


		EnvironmentState::EnvironmentState(const std::map<std::string, std::string>& features)
			: EntityStates(), Features(features)
		{
		}


		EnvironmentState::EnvironmentState(
			const std::map<std::string, std::shared_ptr<EntityState>>& entityStates,
			const std::map<std::string, std::string>& features
			)
			: EntityStates(entityStates), Features(features)
		{
		}



		bool EnvironmentState::ContainsEntity(const std::string& entityId) const
		{
			return EntityStates.find(entityId) != EntityStates.cend();
		}


		std::shared_ptr<EntityState> EnvironmentState::GetEntityState(const std::string& entityId) const
		{
			if (!ContainsEntity(entityId))
			{
				return nullptr;
			}
			return EntityStates.at(entityId);
		}


		void EnvironmentState::SetEntityState(const std::string& entityId, std::shared_ptr<EntityState> entityState)
		{
			if (!entityState)
			{
				return;
			}
			EntityStates[entityId] = entityState;
		}


		void EnvironmentState::RemoveEntityState(const std::string& entityId)
		{
			EntityStates.erase(entityId);
		}


		bool EnvironmentState::HasFeature(const std::string& featureName) const
		{
			return Features.find(featureName) != Features.cend();
		}


		bool EnvironmentState::FeatureIs(const std::string& featureName, const std::string & feature) const
		{
			return HasFeature(featureName) && Features.at(featureName) == feature;
		}


		const std::string& EnvironmentState::GetFeature(const std::string& featureName) const
		{
			if (!HasFeature(featureName))
			{
				static std::string emptyString;
				return emptyString;
			}
			return Features.at(featureName);
		}


		void EnvironmentState::SetFeature(const std::string& featureName, const std::string & feature)
		{
			Features[featureName] = feature;
		}


		void EnvironmentState::RemoveFeature(const std::string& featureName)
		{
			Features.erase(featureName);
		}


		void EnvironmentState::Clear()
		{
			EntityStates.clear();
			Features.clear();
		}



		/*!
		Clone this instance and return a shared pointer to it.
		*/

		std::shared_ptr<EnvironmentState> EnvironmentState::Clone() const
		{
			std::shared_ptr<EnvironmentState> environmentState = EnvironmentState::Make();
			*environmentState = *this;
			return environmentState;
		}


		EnvironmentState& EnvironmentState::operator = (const EnvironmentState& state)
		{
			EntityStates.clear();
			for (const auto& item : state.EntityStates)
			{
				EntityStates[item.first] = item.second->Clone();
			}
			Features = state.Features;
			return *this;
		}


		bool EnvironmentState::operator == (const EnvironmentState& state) const
		{
			if (state.Features != Features)
			{
				return false;
			}
			if (state.EntityStates.size() != EntityStates.size())
			{
				return false;
			}
			// compare the state of each entity
			for (const auto& entStateEntry : state.EntityStates)
			{
				const auto& thisEntStateEntry = EntityStates.find(entStateEntry.first);
				if (thisEntStateEntry != EntityStates.cend())
				{
					if (!thisEntStateEntry->second->EqualTo(entStateEntry.second))
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			return true;
		}
	}
}
