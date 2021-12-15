//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonEnvironmentModelWriter.h"


namespace discenfw
{
	namespace json
	{
		using namespace discenfw::xp;


		JsonEnvironmentModelWriter::JsonEnvironmentModelWriter()
		{
		}


		void JsonEnvironmentModelWriter::WriteEnvironmentModel(
			const std::shared_ptr<xp::EnvironmentModel> environmentModel,
			std::string& jsonText,
			std::string& knowlJsonText
			)
		{
			WriteEnvironmentModelDefinition(environmentModel, jsonText);
			WriteEnvironmentModelKnowledge(environmentModel, knowlJsonText);
		}


		void JsonEnvironmentModelWriter::WriteEnvironmentModelDefinition(const std::shared_ptr<xp::EnvironmentModel> environmentModel, std::string& jsonText)
		{
			StartDocument();
			StartObject("EnvironmentModel");
			WriteString("Name", environmentModel->GetName(), true);

			const std::vector< std::shared_ptr<EntityStateType> >& entityTypes = environmentModel->GetEntityStateTypes();
			//if (!entityTypes.empty())
			//{
			StartArray("EntityStateTypes");
			for (size_t i = 0; i < entityTypes.size(); i++)
			{
				WriteEntityStateType(nullptr, entityTypes[i]);
			}
			EndArray();
			//}

			//const std::vector< std::string >& typeNames = environmentModel->GetEntityStateTypeNames();
			//if (!typeNames.empty())
			//{
			//	StartArray("TypeNames");
			//	for (size_t i = 0; i < typeNames.size(); i++)
			//	{
			//		WriteString(nullptr, typeNames[i]);
			//	}
			//	EndArray();
			//}

			const std::vector< std::string >& roleNames = environmentModel->GetRoleNames();
			//if (!roleNames.empty())
			//{
				StartArray("Roles");
				for (size_t i = 0; i < roleNames.size(); i++)
				{
					WriteRoleDefinition(nullptr, *(environmentModel->GetRole(roleNames[i])));
				}
				EndArray();
			//}

			EndObject();
			EndDocument(jsonText);
		}


		void JsonEnvironmentModelWriter::WriteEnvironmentModelKnowledge(const std::shared_ptr<xp::EnvironmentModel> environmentModel, std::string& jsonText)
		{
			StartDocument();
			StartObject("EnvironmentModelKnowledge");
			WriteString("Name", environmentModel->GetName(), true);

			const std::vector< std::shared_ptr<const EnvironmentState> >& states = environmentModel->GetAllStates();
			if (!states.empty())
			{
				StartArray("States");
				for (size_t i = 0; i < states.size(); i++)
				{
					WriteEnvironmentState((int)i, states[i]);
				}
				EndArray();
			}

			EndObject();
			EndDocument(jsonText);
		}


		void JsonEnvironmentModelWriter::WriteEntityStateTypes(const std::shared_ptr<xp::EnvironmentModel> environmentModel, std::string& jsonText)
		{
			StartDocument();
			const std::vector< std::shared_ptr<EntityStateType> >& entityTypes = environmentModel->GetEntityStateTypes();
			StartArray("EntityStateTypes");
			if (!entityTypes.empty())
			{
				for (size_t i = 0; i < entityTypes.size(); i++)
				{
					WriteEntityStateType(nullptr, entityTypes[i]);
				}
			}
			EndArray();

			EndDocument(jsonText);
		}




		void JsonEnvironmentModelWriter::WriteEntityState(const char* memberName, const std::shared_ptr<const EntityState> entState)
		{
			StartObject(memberName);
			WriteString("Model", entState->GetModelName());
			WriteString("Type", entState->GetTypeName());
			StartArray("Properties");
			for (auto& prop : entState->PropertyValues)
			{
				StartObject();
				WriteString("Property", prop.first);
				WriteString("Value", prop.second);
				EndObject();
			}
			EndArray();

			StartArray("Relationships");
			for (auto& ref : entState->Relationships)
			{
				StartObject();
				WriteString("StartPoint", ref.first);
				WriteString("TargetEntity", ref.second.EntityId);
				WriteString("EndPoint", ref.second.LinkId);
				EndObject();
			}
			EndArray();

			EndObject();
		}


		void JsonEnvironmentModelWriter::WriteEnvironmentState(int stateIndex, const std::shared_ptr<const EnvironmentState> state)
		{
			StartObject();
			WriteInt("Index", stateIndex);

			StartArray("EntityStates");
			for (const auto& entState : state->EntityStates)
			{
				StartObject();
				WriteString("EntityId", entState.first);
				WriteEntityState("State", entState.second);
				EndObject();
			}
			EndArray();

			StartArray("Features");
			for (const auto& feature : state->Features)
			{
				StartArray();
				WriteString(nullptr, feature.first);
				WriteString(nullptr, feature.second);
				EndArray();
			}
			EndArray();

			EndObject();
		}

		void JsonEnvironmentModelWriter::WriteEntityStateType(const char* memberName, const std::shared_ptr<EntityStateType> entityStateType)
		{
			if (memberName)
			{
				Writer.Key(memberName);
			}
			StartObject();
			WriteString("TypeName", entityStateType->GetTypeName());
			WriteString("ModelName", entityStateType->GetModelName());
			WriteString("ParentTypeName", entityStateType->GetParentTypeName());

			StartArray("DefaultPropertyValues");
			for (const auto& defaultPropertyValue : entityStateType->GetDefaultPropertyValues())
			{
				StartObject();
				WriteString("PropertyId", defaultPropertyValue.first);
				WriteString("DefaultValue", defaultPropertyValue.second);
				EndObject();
			}
			EndArray();

			StartArray("PossiblePropertyValues");
			for (const auto& possiblePropertyValue : entityStateType->GetPossiblePropertyValues())
			{
				StartObject();
				WriteString("PropertyId", possiblePropertyValue.first);
				StartArray("PossibleValues");
				for (const auto& possibleValue : possiblePropertyValue.second)
				{
					WriteString(nullptr, possibleValue);
				}
				EndArray();
				EndObject();
			}
			EndArray();

			StartArray("Links");
			for (const std::string& link : entityStateType->GetLinks())
			{
				WriteString(nullptr, link);
			}
			EndArray();

			EndObject();
		}

	}
}
