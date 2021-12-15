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
#include "discenfw/xp/EntityState.h"

#include <string>
#include <map>
#include <vector>
#include <memory>


namespace discenfw
{
	namespace xp
	{
		/*!
		Environment state as collection of entity states.
		*/
		class DISCENFW_API EnvironmentState
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED

			/*!
			Collection of entity states.
			*/
			std::map< std::string, std::shared_ptr<EntityState> > EntityStates;

			/*!
			Features detected in this state, represented as strings.
			*/
			std::map<std::string, std::string> Features;


			EnvironmentState();

			/*!
			Construct a scenario state based on the given entity states.
			*/
			EnvironmentState(
				const std::map< std::string, std::shared_ptr<EntityState> >& entityStates);

			/*!
			Construct a scenario state based on the given features.
			*/
			EnvironmentState(const std::map<std::string, std::string>& features);

			/*!
			Construct a scenario state based on the given entity states.
			*/
			EnvironmentState(
				const std::map< std::string, std::shared_ptr<EntityState> >& entityStates,
				const std::map<std::string, std::string>& features
				);

		public:

			/*!
			Make a new environment state (allocated in the library module), return a shared pointer to it.
			*/
			static std::shared_ptr<EnvironmentState> Make();


			/*!
			Check if the entity with the given id is addressed by an entity state in this scenario state.
			*/
			bool ContainsEntity(const std::string& entityId) const;

			/*!
			Get the entity states defined in this scenario state.
			*/
			const std::map< std::string, std::shared_ptr<EntityState> >& GetEntityStates() const
			{
				return EntityStates;
			}

			/*!
			Get a reference to the entity state for the entity with the given id.
			*/
			std::shared_ptr<EntityState> GetEntityState(const std::string& entityId) const;

			/*!
			Set a reference to the entity state for the entity with the given id.
			*/
			void SetEntityState(const std::string& entityId, std::shared_ptr<EntityState> entityState);

			/*!
			Remove the entity state for the entity with the given id.
			*/
			void RemoveEntityState(const std::string& entityId);


			/*!
			Check if the given feature is defined in this scenario state.
			*/
			bool HasFeature(const std::string& featureName) const;


			/*!
			Check if the given feature is defined in this scenario state and has the given value.
			*/
			bool FeatureIs(const std::string& featureName, const std::string& feature) const;

			/*!
			Get the features defined in this scenario state.
			*/
			const std::map< std::string, std::string >& GetFeatures() const
			{
				return Features;
			}

			/*!
			Get the feature with the given name (empty string if not defined).
			*/
			const std::string& GetFeature(const std::string& featureName) const;

			/*!
			Set the feature with the given name.
			*/
			void SetFeature(const std::string& featureName, const std::string& feature);

			/*!
			Remove the feature with the given name.
			*/
			void RemoveFeature(const std::string& featureName);


			/*!
			Clear all the entity states stored in this environment state.
			*/
			void Clear();

			/*!
			Clone this instance and return a shared pointer to it.
			*/
			std::shared_ptr<EnvironmentState> Clone() const;

			EnvironmentState& operator = (const EnvironmentState& state);

			bool operator == (const EnvironmentState& state) const;

			bool operator != (const EnvironmentState& state) const { return !(*this == state); }
		};

	}
}

