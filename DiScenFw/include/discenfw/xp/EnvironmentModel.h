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
#include "discenfw/xp/EnvironmentState.h"
#include "discenfw/xp/EntityStateType.h"
#include "discenfw/xp/RoleInfo.h"
#include "discenfw/xp/Condition.h"
#include "discenfw/xp/Action.h"

#include <string>
#include <vector>
#include <memory>


namespace discenfw
{
	namespace xp
	{
		/*!
		Model of an environment, dynamically built.
		*/
		class DISCENFW_API EnvironmentModel
		{
		public:

			~EnvironmentModel();


			/*!
			Get a reference (create it if not found) to the environment model with the given id (if empty use the default model).
			*/
			static std::shared_ptr<EnvironmentModel> GetOrCreate(
				const std::string& modelName = ""
				);

			/*!
			Ramove all the environment models (possible shared references will survive).
			*/
			static void RemoveModel(const std::string& modelName);

			/*!
			Ramove all the environment models (possible shared references will survive).
			*/
			static void RemoveAllModels();


			/*!
			Check if the model was defined (true) or it was only crerated (false).
			*/
			bool IsDefined();

			/*!
			Get the name of the environment model.
			*/
			const std::string& GetName()
			{
				return Name;
			}

			/*!
			Check if the given environment state is known in the model.
			*/
			bool ContainsState(const EnvironmentState& environmentState) const;


			/*!
			Get a reference to the given environment state.
			*/
			const std::shared_ptr<EnvironmentState> FindState(
				const EnvironmentState& environmentState
				) const;


			/*!
			Get a reference to the given environment state.
			*/
			const std::shared_ptr<EnvironmentState> FindState(
				const std::shared_ptr<EnvironmentState> environmentState
				) const;


			/*!
			Get a reference to the given environment state, store it if new.
			*/
			const std::shared_ptr<EnvironmentState> GetStoredState(
				const EnvironmentState& environmentState
				);


			/*!
			Get a reference to the given environment state, store it if new.
			*/
			const std::shared_ptr<EnvironmentState> GetStoredState(
				const std::shared_ptr<EnvironmentState> environmentState
				);


			/*!
			Get the state stored at the given position or null if the position is not valid.
			*/
			std::shared_ptr<EnvironmentState> GetStoredState(int stateIndex);


			/*!
			Get the index of the given environment state or -1 if not found if not found.
			*/
			int IndexOfState(const std::shared_ptr<EnvironmentState> state) const;


			/*!
			Get the number of the stored environment states.
			*/
			int GetNumStates() const;


			/*!
			Apply the given environment state changes and return the new state.
			*/
			std::shared_ptr<EnvironmentState> ChangeState(
				const std::shared_ptr<EnvironmentState> originalState,
				const EnvironmentState& stateChanges
				);


			/*!
			Apply the given environment state changes and return the new state.
			*/
			std::shared_ptr<EnvironmentState> ChangeCurrentState(
				const EnvironmentState& stateChanges
				);


			/*!
			Set the current environment state (return the internal stored state).
			*/
			const std::shared_ptr<EnvironmentState> SetCurrentState(
				const std::shared_ptr<EnvironmentState> environmentState
				);


			/*!
			Get the current environment state (return the internal stored state).
			*/
			const std::shared_ptr<EnvironmentState> GetCurrentState();


			/*!
			Get the stored environment states.
			*/
			const std::vector< std::shared_ptr<const EnvironmentState> >& GetAllStates();


			/*!
			Clear all the stored states.
			*/
			void ClearStoredStates();


			/*!
			Create a new entity state type, inheriting from the given entity state type,
			with the given name, the given default property values, the given possible property values
			and the given links.
			*/
			std::shared_ptr<EntityStateType> CreateEntityStateType(
				const std::string& parentTypeName,
				const std::string& typeName,
				const std::map<std::string, std::string>& defaultPropertyValues,
				const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
				const std::vector<std::string>& links
				);


			/*!
			Update an existing entity state type with the given name
			with the given default property values, the given possible property values
			and the given links.
			*/
			std::shared_ptr<EntityStateType> UpdateEntityStateType(
				const std::string& typeName,
				const std::map<std::string, std::string>& defaultPropertyValues,
				const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
				const std::vector<std::string>& links
				);

			/*!
			Get a list of names of the entity state types defined in this model.
			*/
			const std::vector< std::string >& GetEntityStateTypeNames() const;


			/*!
			Get a list of entity state types defined in this model.
			*/
			const std::vector< std::shared_ptr<EntityStateType> >&
				GetEntityStateTypes() const;


			/*!
			Get a reference to the entity state type with the given name.
			*/
			const std::shared_ptr<EntityStateType> GetEntityStateType(
				const std::string& typeName
				) const;

			/*!
			Clear all the defined entity state types. Warning! All the EntityState instances will lose their type in this way.
			*/
			void ClearEntityStateTypes();

			/*!
			Get the names of the stored roles (they must be defined with SetRole() or StoreRole()).
			*/
			const std::vector<std::string>& GetRoleNames() const;

			/*!
			Check if a role with the given name was stored (with SetRole() or StoreRole()).
			*/
			bool HasRole(const std::string& roleName) const;

			/*!
			Get the role with the given name (it must be defined with SetRole() or StoreRole()).
			*/
			const std::shared_ptr<RoleInfo> GetRole(const std::string& roleName) const;

			/*!
			Create or update the role with the given name.
			*/
			bool SetRole(
				const std::string& roleName,
				const Condition& successCondition,
				const Condition& failureCondition,
				const Condition& deadlockCondition,
				const StateRewardRules& stateRewardRules
				);


			/*!
			Store the role with the given name (it is replaced if already existing).
			*/
			bool StoreRole(const RoleInfo& role);


			/*!
			Store the role with the given name (it is replaced if already existing).
			*/
			bool StoreRole(const std::shared_ptr<RoleInfo> role);


			/*!
			Encode an action and store it.
			*/
			std::shared_ptr<Action> EncodeAction(const Action& action) const;

			/*!
			Encode an action and store it.
			*/
			std::shared_ptr<Action> EncodeAction(
				const Action& action,
				std::string& actionString
				) const;

			/*!
			Decode an action froma a string (use internal actions map if it was already encoded, else decode it).
			*/
			std::shared_ptr<Action> DecodeAction(const std::string& actionString) const;


		protected:

			static std::map< std::string, std::shared_ptr<EnvironmentModel> >
				EnvironmentModelMap;

			static std::string LastModelName;

			/*!
			Name of the environment model.
			*/
			std::string Name;

			/*!
			Information about agents' roles.
			*/
			std::map< std::string, std::shared_ptr<RoleInfo> > Roles;

			/*!
			Index of entity state types defined in this model.
			*/
			std::map< std::string, std::shared_ptr<EntityStateType> >
				TypeMap;

			/*!
			Names of the roles defined in this model.
			*/
			std::vector< std::string > RoleNames;

			/*!
			Entity state types defined in this model.
			*/
			std::vector< std::shared_ptr<EntityStateType> > Types;

			/*!
			Names of the entity state types defined in this model.
			*/
			std::vector< std::string > TypeNames;


			/*!
			Stored environment states.
			*/
			std::vector< std::shared_ptr<EnvironmentState> > EnvironmentStates;


			/*!
			Current environment state.
			*/
			std::shared_ptr<EnvironmentState> CurrentState;

			/*!
			Stored actions with their results, mapped from their string encoding.
			*/
			mutable std::map< std::string, std::shared_ptr<Action> > EncodedActions;


			/*!
			Construct an environment model with the given name.
			*/
			EnvironmentModel(const std::string& name = "");

		};


		/*!
		Get a reference (create it if not found) to the environment model with the given name.
		If the given name is omitted or empty use the last defined model (create a default model if not found).
		*/
		inline std::shared_ptr<EnvironmentModel> GetModel(const std::string& modelName)
		{
			return EnvironmentModel::GetOrCreate(modelName);
		}


		/*!
		Get a reference to the last defined model (create a default model if not found).
		*/
		inline std::shared_ptr<EnvironmentModel> GetLastModel()
		{
			return EnvironmentModel::GetOrCreate();
		}


		/*!
		Get a reference to the given role in the given model (create a default role if not found).
		*/
		inline const std::shared_ptr<RoleInfo> GetRole(
			const std::string& roleName,
			const std::string& modelName
			)
		{
			return EnvironmentModel::GetOrCreate(modelName)->GetRole(roleName);
		}


		/*!
		Create or update the role with the given name.
		*/
		inline void SetRole(
				const std::string& roleName,
				Condition successCondition,
				Condition failureCondition,
				Condition deadlockCondition,
				StateRewardRules stateRewardRules,
				const std::string& modelName = "")
		{
			EnvironmentModel::GetOrCreate(modelName)->SetRole(
				roleName,
				successCondition,
				failureCondition,
				deadlockCondition,
				stateRewardRules
				);
		}
	}
}

