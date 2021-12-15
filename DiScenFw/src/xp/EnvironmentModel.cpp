//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/EnvironmentModel.h>
#include <discenfw/util/MessageLog.h>

#include <gpvulc/text/TextBuffer.h>
#include <gpvulc/text/text_util.h>
#include <gpvulc/path/PathInfo.h>
#include <gpvulc/json/RapidJsonInclude.h> // ParseException, FormatException

#include "../JSON/JsonEnvironmentModel.h"

#include <algorithm>

// TODO: <EnvironmentModel> is it possible to implement feature construction based on state conditions?

namespace discenfw
{
	namespace xp
	{
		static std::shared_ptr<EntityStateType> NullEntityStateType;


		std::map< std::string, std::shared_ptr<EnvironmentModel> >
			EnvironmentModel::EnvironmentModelMap;

		std::string EnvironmentModel::LastModelName;


		EnvironmentModel::~EnvironmentModel()
		{
		}



		std::shared_ptr<EnvironmentModel> EnvironmentModel::GetOrCreate(const std::string& modelName)
		{
			std::string actualName = modelName;
			if (!EnvironmentModelMap.empty() && actualName.empty())
			{
				actualName = LastModelName;
			}
			if (EnvironmentModelMap.find(actualName) == EnvironmentModelMap.end())
			{
				std::shared_ptr<EnvironmentModel> newModel(new EnvironmentModel(actualName));
				EnvironmentModelMap.insert({ actualName,newModel });
				LastModelName = actualName;
			}
			return EnvironmentModelMap[actualName];
		}


		void EnvironmentModel::RemoveModel(const std::string& modelName)
		{
			if (LastModelName == modelName)
			{
				LastModelName.clear();
			}
			if (EnvironmentModelMap.find(modelName) != EnvironmentModelMap.end())
			{
				EnvironmentModelMap.erase(modelName);
			}
		}


		void EnvironmentModel::RemoveAllModels()
		{
			LastModelName.clear();
			EnvironmentModelMap.clear();
		}

		bool EnvironmentModel::IsDefined()
		{
			return !Types.empty() && !Roles.empty();
		}



		bool EnvironmentModel::ContainsState(const EnvironmentState& environmentState) const
		{
			return FindState(environmentState) != nullptr;
		}


		const std::shared_ptr<EnvironmentState> EnvironmentModel::FindState(
			const EnvironmentState& environmentState) const
		{
			auto it = std::find_if(EnvironmentStates.begin(), EnvironmentStates.end(),
				[&environmentState](const std::shared_ptr<EnvironmentState> state)
			{
				return *state == environmentState;
			}
			);
			if (it == EnvironmentStates.end())
			{
				return nullptr;
			}
			return *it;
		}


		const std::shared_ptr<EnvironmentState> EnvironmentModel::FindState(
			const std::shared_ptr<EnvironmentState> environmentState) const
		{
			auto it = std::find(EnvironmentStates.begin(), EnvironmentStates.end(), environmentState);
			if (it != EnvironmentStates.end())
			{
				return *it;
			}
			return FindState(*environmentState);
		}


		const std::shared_ptr<EnvironmentState> EnvironmentModel::GetStoredState(
			const EnvironmentState& environmentState)
		{
			std::shared_ptr<EnvironmentState> state = FindState(environmentState);
			if (!state)
			{
				state = environmentState.Clone();
				EnvironmentStates.push_back(state);
			}
			return state;
		}


		const std::shared_ptr<EnvironmentState> EnvironmentModel::GetStoredState(
			const std::shared_ptr<EnvironmentState> environmentState)
		{
			std::shared_ptr<EnvironmentState> state = FindState(environmentState);
			if (!state)
			{
				state = environmentState;
				if (!state)
				{
					state = EnvironmentState::Make();
				}
				EnvironmentStates.push_back(state);
			}
			return state;
		}


		std::shared_ptr<EnvironmentState> EnvironmentModel::GetStoredState(int stateIndex)
		{
			if (stateIndex < 0 || stateIndex >= (int)EnvironmentStates.size())
			{
				return nullptr;
			}

			return EnvironmentStates[stateIndex];
		}


		int EnvironmentModel::IndexOfState(const std::shared_ptr<EnvironmentState> state) const
		{
			for (int i = 0; i < (int)EnvironmentStates.size(); i++)
			{
				if (EnvironmentStates[i] == state)
				{
					return i;
				}
			}
			return -1;
		}


		int EnvironmentModel::GetNumStates() const
		{
			return (int)EnvironmentStates.size();
		}


		const std::vector<std::shared_ptr<const EnvironmentState>>& EnvironmentModel::GetAllStates()
		{
			return (const std::vector<std::shared_ptr<const EnvironmentState>>&)(EnvironmentStates);
		}


		std::shared_ptr<EnvironmentState> EnvironmentModel::ChangeState(
				const std::shared_ptr<EnvironmentState> originalState,
				const EnvironmentState& stateChanges
			)
		{
			const std::shared_ptr<EnvironmentState> storedState = FindState(originalState);
			if (!storedState)
			{
				return nullptr;
			}
			std::shared_ptr<EnvironmentState> newState = storedState->Clone();
			auto& currEntStates = newState->EntityStates;
			for (const auto& actionStateChange : stateChanges.EntityStates)
			{
				const std::string& entityId = actionStateChange.first;
				const auto& entityChange = actionStateChange.second;
				bool foundState = currEntStates.find(entityId) != currEntStates.cend();
				if (foundState)
				{
					// if the entity state exists update it...
					auto& entState = currEntStates[entityId];
					auto newEntState = entState->Clone();
					for (const auto& prop : actionStateChange.second->PropertyValues)
					{
						newEntState->PropertyValues[prop.first] = prop.second;
					}
					//for (const auto& rel : actionStateChange.second->Relationships)
					//{
					//	newEntState->Relationships[rel.first] = rel.second;
					//}
					newEntState->Relationships = entityChange->Relationships;
					entState = newEntState;
				}
				else
				{
					// ...else clone it
					currEntStates[entityId] = entityChange->Clone();
				}
			}

			return SetCurrentState(newState);
		}


		std::shared_ptr<EnvironmentState> EnvironmentModel::ChangeCurrentState(
			const EnvironmentState& stateChanges)
		{
			return ChangeState(GetCurrentState(),stateChanges);
		}


		const std::shared_ptr<EnvironmentState> EnvironmentModel::SetCurrentState(
			const std::shared_ptr<EnvironmentState> environmentState)
		{
			CurrentState = GetStoredState(environmentState);
			return CurrentState;
		}


		const std::shared_ptr<EnvironmentState> EnvironmentModel::GetCurrentState()
		{
			if (!CurrentState)
			{
				CurrentState = EnvironmentState::Make();
			}
			return CurrentState;
		}


		void EnvironmentModel::ClearStoredStates()
		{
			EnvironmentStates.clear();
		}


		std::shared_ptr<EntityStateType> EnvironmentModel::CreateEntityStateType(
			const std::string& parentTypeName,
			const std::string& typeName,
			const std::map<std::string, std::string>& defaultPropertyValues,
			const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
			const std::vector<std::string>& links
			)
		{
			if (typeName.empty())
			{
				return nullptr;
			}
			bool alreadyExisting = (TypeMap.find(typeName) != TypeMap.end()) && TypeMap[typeName];
			std::map<std::string, std::string> newDefaultPropertyValues;
			std::map< std::string, std::vector<std::string> > newPossiblePropertyValues;
			std::vector<std::string> newLinks;
			if (!parentTypeName.empty())
			{
				bool parentExisting = (TypeMap.find(parentTypeName) != TypeMap.end());
				if (parentTypeName.empty() || !parentExisting)
				{
					return nullptr;
				}
				std::map<std::string, std::string> parentDefaultPropertyValues;
				TypeMap[parentTypeName]->GetDefaultValues(parentDefaultPropertyValues);
				newDefaultPropertyValues.insert(parentDefaultPropertyValues.cbegin(), parentDefaultPropertyValues.cend());
				newDefaultPropertyValues.insert(defaultPropertyValues.cbegin(), defaultPropertyValues.cend());
				std::map< std::string, std::vector<std::string> > parentPossiblePropertyValues;
				TypeMap[parentTypeName]->GetPossiblePropertyValues(parentPossiblePropertyValues);
				newPossiblePropertyValues.insert(parentPossiblePropertyValues.cbegin(), parentPossiblePropertyValues.cend());
				newPossiblePropertyValues.insert(possiblePropertyValues.cbegin(), possiblePropertyValues.cend());
				newLinks = TypeMap[parentTypeName]->GetLinks();
				newLinks.insert(newLinks.end(), links.cbegin(), links.cend());
			}
			else
			{
				newDefaultPropertyValues = defaultPropertyValues;
				newPossiblePropertyValues = possiblePropertyValues;
				newLinks = links;
			}

			if (alreadyExisting)
			{
				if (
					TypeMap[typeName]->GetParentTypeName()==parentTypeName
					&& TypeMap[typeName]->HasDefaultValues(newDefaultPropertyValues)
					&& TypeMap[typeName]->HasPossiblePropertyValues(newPossiblePropertyValues)
					&& TypeMap[typeName]->HasLinks(newLinks)
					)
				{
					return TypeMap[typeName];
				}
				else
				{
					LogMessage(LOG_ERROR, "Model " + Name + " - A different entity state type '" + typeName + "' already exists.", "DiScenFw");
					return nullptr;
				}
			}

			std::shared_ptr<EntityStateType> newEntityStateType(
				new EntityStateType(
					parentTypeName,
					typeName,
					newDefaultPropertyValues,
					newPossiblePropertyValues,
					links,
					Name
					)
				);
			TypeMap[typeName] = newEntityStateType;
			Types.push_back(newEntityStateType);
			TypeNames.push_back(typeName);

			return newEntityStateType;
		}


		std::shared_ptr<EntityStateType> EnvironmentModel::UpdateEntityStateType(
			const std::string& typeName,
			const std::map<std::string,	std::string>& defaultPropertyValues,
			const std::map<std::string,	std::vector<std::string>>& possiblePropertyValues,
			const std::vector<std::string>& links
			)
		{
			bool alreadyExisting = (TypeMap.find(typeName) != TypeMap.end()) && TypeMap[typeName];
			if (!alreadyExisting)
			{
				return nullptr;
			}
			TypeMap[typeName]->SetDefaultValues(defaultPropertyValues);
			TypeMap[typeName]->SetPossiblePropertyValues(possiblePropertyValues);
			TypeMap[typeName]->SetLinks(links);
			return TypeMap[typeName];
		}

		const std::vector< std::string >& EnvironmentModel::GetEntityStateTypeNames() const
		{
			return TypeNames;
		}


		const std::vector< std::shared_ptr<EntityStateType> >& EnvironmentModel::GetEntityStateTypes() const
		{
			return Types;
		}


		const std::shared_ptr<EntityStateType> EnvironmentModel::GetEntityStateType(
			const std::string& typeName
			) const
		{
			if (TypeMap.find(typeName) == TypeMap.cend())
			{
				return nullptr;
			}
			return TypeMap.at(typeName);
		}


		void EnvironmentModel::ClearEntityStateTypes()
		{
			TypeMap.clear();
			Types.clear();
			TypeNames.clear();
		}


		const std::vector<std::string>& EnvironmentModel::GetRoleNames() const
		{
			return RoleNames;
		}


		bool EnvironmentModel::HasRole(const std::string& roleName) const
		{
			return Roles.find(roleName)!=Roles.cend();
		}


		const std::shared_ptr<RoleInfo> EnvironmentModel::GetRole(const std::string& roleName) const
		{
			std::string actualRoleName = roleName;
			if (actualRoleName.empty() && !Roles.empty())
			{
				return Roles.begin()->second;
			}
			if (!HasRole(roleName))
			{
				LogMessage(LOG_ERROR, "Model " + Name + " - Undefined role: " + roleName, "DiScenFw");
				return nullptr;
			}
			return Roles.at(roleName);
		}


		bool EnvironmentModel::SetRole(
				const std::string& roleName,
				const Condition& successCondition,
				const Condition& failureCondition,
				const Condition& deadlockCondition,
				const StateRewardRules& stateRewardRules
			)
		{
			if (roleName.empty())
			{
				LogMessage(LOG_ERROR, "Model " + Name + " - Role name cannot be empty.", "DiScenFw");
				return false;
			}
			if (!Roles[roleName])
			{
				Roles[roleName] = std::make_shared<RoleInfo>( roleName, successCondition, failureCondition, deadlockCondition, stateRewardRules, Name);
				RoleNames.push_back(roleName);
			}
			else
			{
				*(Roles[roleName]) = { roleName, successCondition, failureCondition, deadlockCondition, stateRewardRules, Name };
			}
			return true;
		}


		bool EnvironmentModel::StoreRole(const RoleInfo& role)
		{
			if (role.RoleName.empty())
			{
				LogMessage(LOG_ERROR, "Model " + Name + " - Role name cannot be empty.", "DiScenFw");
				return false;
			}
			if (!Roles[role.RoleName])
			{
				Roles[role.RoleName] = std::make_shared<RoleInfo>( role );
				Roles[role.RoleName]->RoleName = role.RoleName;
				RoleNames.push_back(role.RoleName);
			}
			*(Roles[role.RoleName]) = role;
			Roles[role.RoleName]->ModelName = Name;

			return true;
		}


		bool EnvironmentModel::StoreRole(const std::shared_ptr<RoleInfo> role)
		{
			if (!role)
			{
				LogMessage(LOG_ERROR, "Model " + Name + " - Role cannot be null.", "DiScenFw");
				return false;
			}
			if (role->RoleName.empty())
			{
				LogMessage(LOG_ERROR, "Model " + Name + " - Role name cannot be empty.", "DiScenFw");
				return false;
			}
			if (std::find(RoleNames.begin(),RoleNames.end(), role->RoleName)==RoleNames.end())
			{
				RoleNames.push_back(role->RoleName);
			}
			Roles[role->RoleName] = role;
			Roles[role->RoleName]->ModelName = Name;

			return true;
		}


		std::shared_ptr<Action> EnvironmentModel::EncodeAction(const Action& action) const
		{
			std::string actionString;
			return EncodeAction(action, actionString);
		}


		std::shared_ptr<Action> EnvironmentModel::EncodeAction(const Action& action, std::string& actionString) const
		{
			actionString = action.ToString();
			std::shared_ptr<Action> storedAction = EncodedActions[actionString];
			if (!storedAction)
			{
				storedAction = std::make_shared<Action>();
				*storedAction = action;
				EncodedActions[actionString] = storedAction;
			}
			return storedAction;
		}


		std::shared_ptr<Action> EnvironmentModel::DecodeAction(const std::string& actionString) const
		{
			if (EncodedActions.find(actionString) != EncodedActions.cend())
			{
				return EncodedActions.at(actionString);
			}
			std::shared_ptr<Action> storedAction = std::make_shared<Action>();
			gpvulc::TextBuffer actionBuf(actionString);
			std::vector<std::string> args = actionBuf.Split('|');
			if (args.empty())
			{
				return nullptr;
			}
			storedAction->TypeId = args[0];
			for (size_t i = 1; i < args.size(); i++)
			{
				storedAction->Params.push_back(args[i]);
			}
			EncodedActions[actionString] = storedAction;

			return storedAction;
		}


		EnvironmentModel::EnvironmentModel(const std::string& name)
			: Name(name)
		{
		}


	} // namespace xp
} // namespace discenfw
