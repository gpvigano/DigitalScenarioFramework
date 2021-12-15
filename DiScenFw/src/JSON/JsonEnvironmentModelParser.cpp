//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonEnvironmentModelParser.h"

using namespace rapidjson;
using namespace discenfw::xp;

namespace discenfw
{
	namespace json
	{

		JsonEnvironmentModelParser::JsonEnvironmentModelParser()
		{
		}


		std::shared_ptr<xp::EnvironmentModel> JsonEnvironmentModelParser::ParseEnvironmentModel(const std::string& jsonText, const std::string& knowlJsonText)
		{
			StartContext("EnvironmentModel");

			std::shared_ptr<EnvironmentModel> environmentModel;
			environmentModel = ParseEnvironmentModelDefinition(jsonText);
			if (!knowlJsonText.empty())
			{
				environmentModel = ParseEnvironmentModelKnowledge(knowlJsonText, environmentModel);
			}
			EndContext();
			return environmentModel;
		}


		std::shared_ptr<xp::EnvironmentModel> JsonEnvironmentModelParser::ParseEnvironmentModelDefinition(const std::string& jsonText)
		{
			StartContext("EnvironmentModelDefinition");

			Parse(jsonText);

			const Value& environmentModelValue = GetRootElement("EnvironmentModel");
			std::string modelName = GetAsString(environmentModelValue, "Name");
			std::shared_ptr<EnvironmentModel> environmentModelRef = GetModel(modelName);
			if (CheckHasArray(environmentModelValue, "EntityStateTypes"))
			{
				StartContext("EntityStateTypes");
				const Value& entityTypesValue = environmentModelValue["EntityStateTypes"];
				for (SizeType i = 0; i < entityTypesValue.Size(); i++)
				{
					StartContext(i);
					ParseEntityStateType(entityTypesValue[i]);
					EndContext();
				}
				EndContext();
			}

			if (CheckHasArray(environmentModelValue, "Roles"))
			{
				StartContext("Roles");
				const Value& rolesValue = environmentModelValue["Roles"];
				for (SizeType i = 0; i < rolesValue.Size(); i++)
				{
					StartContext(i);
					std::shared_ptr<RoleInfo> role = ParseRoleDefinition(rolesValue[i], modelName.c_str());
					EndContext();
					if (role)
					{
						environmentModelRef->StoreRole(role);
					}
				}
				EndContext();
			}

			environmentModelRef->ClearStoredStates();
			if (CheckHasArray(environmentModelValue, "States", true))
			{
				StartContext("States");
				const Value& states = environmentModelValue["States"];
				for (SizeType i = 0; i < states.Size(); i++)
				{
					StartContext(i);
					environmentModelRef->GetStoredState(ParseEnvironmentState(states[i]));
					EndContext();
				}
				EndContext();
			}

			EndContext();
			return environmentModelRef;
		}


		std::shared_ptr<xp::EnvironmentModel> JsonEnvironmentModelParser::ParseEnvironmentModelKnowledge(const std::string& jsonText, std::shared_ptr<xp::EnvironmentModel>)
		{
			StartContext("EnvironmentModelKnowledge");

			Parse(jsonText);

			const Value& environmentModelValue = GetRootElement("EnvironmentModelKnowledge");
			std::string modelName = GetAsString(environmentModelValue, "Name");
			std::shared_ptr<EnvironmentModel> model = GetModel(modelName);
			model->ClearStoredStates();

			if (CheckHasArray(environmentModelValue, "States", true))
			{
				StartContext("States");
				const Value& states = environmentModelValue["States"];
				for (SizeType i = 0; i < states.Size(); i++)
				{
					StartContext(i);
					model->GetStoredState(ParseEnvironmentState(states[i]));
					EndContext();
				}
				EndContext();
			}

			EndContext();
			return model;
		}


		std::shared_ptr<xp::EntityStateType> JsonEnvironmentModelParser::ParseEntityStateType(const rapidjson::Value& entityStateTypeValue)
		{
			std::string modelName = GetAsString(entityStateTypeValue, "ModelName");
			std::string typeName = GetAsString(entityStateTypeValue, "TypeName");
			std::string parentTypeName = GetAsString(entityStateTypeValue, "ParentTypeName", true);
			std::map<std::string, std::string> defaultPropertyValues;
			if (CheckHasArray(entityStateTypeValue, "DefaultPropertyValues"))
			{
				StartContext("DefaultPropertyValues");
				const Value& propsValue = entityStateTypeValue["DefaultPropertyValues"];
				for (SizeType i = 0; i < propsValue.Size(); i++)
				{
					StartContext(i);
					std::string id = GetAsString(propsValue[i], "PropertyId");
					defaultPropertyValues[id] = GetAsString(propsValue[i], "DefaultValue");
					EndContext();
				}
				EndContext();
			}
			std::map< std::string, std::vector<std::string> > possiblePropertyValues;
			if (CheckHasArray(entityStateTypeValue, "PossiblePropertyValues"))
			{
				StartContext("PossiblePropertyValues");
				const Value& propsValue = entityStateTypeValue["PossiblePropertyValues"];
				for (SizeType i = 0; i < propsValue.Size(); i++)
				{
					StartContext(i);
					std::string id = GetAsString(propsValue[i], "PropertyId");
					if (CheckHasArray(propsValue[i], "PossibleValues"))
					{
						StartContext("PossibleValues");
						const Value& possibleValuesValue = propsValue[i]["PossibleValues"];
						for (SizeType i = 0; i < possibleValuesValue.Size(); i++)
						{
							StartContext(i);
							if (possibleValuesValue[i].IsString())
							{
								possiblePropertyValues[id].push_back(possibleValuesValue[i].GetString());
							}
							else
							{
								AddJsonError(gpvulc::json::ErrorType::WRONG_TYPE, "possible value");
							}
							EndContext();
						}
						EndContext();
					}
					EndContext();
				}
				EndContext();
			}
			std::vector<std::string> links;
			if (CheckHasArray(entityStateTypeValue, "Links", true))
			{
				StartContext("Links");
				const Value& linksValue = entityStateTypeValue["Links"];
				for (SizeType i = 0; i < linksValue.Size(); i++)
				{
					StartContext(i);
					if (linksValue[i].IsString())
					{
						links.push_back(linksValue[i].GetString());
					}
					else
					{
						AddJsonError(gpvulc::json::ErrorType::WRONG_TYPE, "link");
					}
					EndContext();
				}
				EndContext();
			}

			std::shared_ptr<EnvironmentModel> model = xp::GetModel(modelName);
			std::shared_ptr<EntityStateType> entityStateType = model->CreateEntityStateType(
				parentTypeName,
				typeName,
				defaultPropertyValues,
				possiblePropertyValues,
				links
				);

			return entityStateType;
		}


		std::shared_ptr<EntityState> JsonEnvironmentModelParser::ParseEntityState(const rapidjson::Value& stateValue)
		{
			std::string modelName = stateValue["Model"].GetString();
			std::string typeId = stateValue["Type"].GetString();
			std::shared_ptr<EntityState> entState = std::make_shared<EntityState>(typeId, modelName);

			if (CheckHasArray(stateValue, "Properties", true))
			{
				StartContext("Properties");
				const Value& propsValue = stateValue["Properties"];
				for (SizeType i = 0; i < propsValue.Size(); i++)
				{
					StartContext(i);
					std::string id = GetAsString(propsValue[i], "Property");
					entState->PropertyValues[id] = GetAsString(propsValue[i], "Value");
					EndContext();
				}
				EndContext();
			}

			if (CheckHasArray(stateValue, "Relationships", true))
			{
				StartContext("Relationships");
				const Value& refsValue = stateValue["Relationships"];
				for (SizeType i = 0; i < refsValue.Size(); i++)
				{
					StartContext(i);
					std::string startPoint = GetAsString(refsValue[i], "StartPoint");
					if (!startPoint.empty())
					{
						entState->Relationships[startPoint] =
						{
							GetAsString(refsValue[i],"TargetEntity"),
							GetAsString(refsValue[i],"EndPoint")
						};
					}
					EndContext();
				}
				EndContext();
			}

			return entState;
		}


		std::shared_ptr<EnvironmentState> JsonEnvironmentModelParser::ParseEnvironmentState(const rapidjson::Value& stateValue)
		{
			std::shared_ptr<EnvironmentState> state = EnvironmentState::Make();
			int stateIndex = GetAsInt(stateValue, "Index");

			if (CheckHasArray(stateValue, "EntityStates", true))
			{
				StartContext("EntityStates");
				const Value& entStatesValue = stateValue["EntityStates"];
				for (SizeType i = 0; i < entStatesValue.Size(); i++)
				{
					StartContext(i);
					std::string id = GetAsString(entStatesValue[i], "EntityId");
					if (CheckHasMember(entStatesValue[i], "State"))
					{
						StartContext("State");
						state->EntityStates[id] = ParseEntityState(entStatesValue[i]["State"]);
						EndContext();
					}
					EndContext();
				}
				EndContext();
			}
			if (CheckHasArray(stateValue, "Features", true))
			{
				StartContext("Features");
				const Value& featuresValue = stateValue["Features"];
				for (SizeType i = 0; i < featuresValue.Size(); i++)
				{
					StartContext(i);
					if (CheckIsArray(featuresValue[i]))
					{
						std::string featureName = GetString(featuresValue[i][0],"feature name");
						std::string feature = GetString(featuresValue[i][1], "feature");
						if (!featureName.empty())
						{
							state->Features[featureName] = feature;
						}
					}
					EndContext();
				}
				EndContext();
			}
			return state;
		}

	}
}
