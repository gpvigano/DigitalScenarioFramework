//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonScenarioParser.h"

using namespace rapidjson;

namespace discenfw
{
	namespace json
	{
		using namespace gpvulc::json;

		JsonScenarioParser::JsonScenarioParser()
		{
		}

		void JsonScenarioParser::ParseScenario(const std::string& jsonText, Scenario& scenarioData)
		{
			using namespace rapidjson;

			StartContext("Scenario");

			Parse(jsonText);

			const Value& scenario = GetRootElement("Scenario");

			scenarioData.Name = GetAsString(scenario, "Name");
			if (CheckHasArray(scenario, "Entities"))
			{
				StartContext("Entities");
				const Value& entities = scenario["Entities"];
				scenarioData.Entities.clear();
				for (SizeType i = 0; i < entities.Size(); i++)
				{
					StartContext(i);
					scenarioData.Entities.push_back(ParseEntity(entities[i]));
					EndContext();
				}
				EndContext();
			}
			EndContext();
		}






		void JsonScenarioParser::ParseAggregate(const rapidjson::Value& entityValue, std::shared_ptr<Aggregate>& aggregate)
		{
			const Value& componentsValue = entityValue["Components"];
			if (componentsValue.IsArray())
			{
				aggregate->Components.clear();
				for (SizeType i = 0; i < componentsValue.Size(); i++)
				{
					aggregate->Components.push_back(ParseEntity(componentsValue[i]));
				}
			}
		}


		void JsonScenarioParser::ParseGroupElement(const rapidjson::Value& entityValue, std::shared_ptr<GroupElement>& groupElement)
		{
			const Value& partsValue = entityValue["Parts"];
			if (partsValue.IsArray())
			{
				groupElement->Parts.clear();
				for (SizeType i = 0; i < partsValue.Size(); i++)
				{
					std::shared_ptr <Element> element = std::static_pointer_cast<Element>(ParseEntity(partsValue[i]));
					groupElement->Parts.push_back(element);
				}
			}
		}


		void JsonScenarioParser::ParseSocketInfo(const Value& socketInfoValue, SocketInfo& socketInfo)
		{
			socketInfo.Id = GetAsString(socketInfoValue, "Id");
			socketInfo.Type = GetAsString(socketInfoValue, "Type", true);
			if (CheckHasArray(socketInfoValue, "Compatibility", true))
			{
				StartContext("Compatibility");
				const Value& compatibilityValue = socketInfoValue["Compatibility"];
				socketInfo.Compatibility.clear();
				for (SizeType i = 0; i < compatibilityValue.Size(); i++)
				{
					if (compatibilityValue[i].IsString())
					{
						socketInfo.Compatibility.push_back(compatibilityValue[i].GetString());
					}
					else
					{
						AddJsonError(ErrorType::WRONG_TYPE, std::to_string(i));
					}
				}
				EndContext();
			}
			if (CheckHasMember(socketInfoValue, "Location", true))
			{
				const Value& locValue = socketInfoValue["Location"];
				ParseCoordSys(locValue, socketInfo.Location);
			}
		}


		Anchor JsonScenarioParser::ParseAnchor(const Value& anchorValue)
		{
			Anchor anchor;
			anchor.ElementId = GetAsString(anchorValue, "ElementId", true);
			anchor.SocketId = GetAsString(anchorValue, "SocketId", true);
			GetVec3(anchorValue, "Offset", anchor.Offset);
			return anchor;
		}


		void JsonScenarioParser::ParseConnection(const rapidjson::Value& entityValue, std::shared_ptr <ConnectionElement>& connection)
		{
			if (CheckHasArray(entityValue, "ConnectionPath"))
			{
				const Value& pathValue = entityValue["ConnectionPath"];
				connection->ClearConnectionPath();
				for (SizeType i = 0; i < pathValue.Size(); i++)
				{
					connection->AddAnchor(ParseAnchor(pathValue[i]));
				}
			}
		}


		void JsonScenarioParser::ParseMutableElement(const rapidjson::Value& entityValue, std::shared_ptr <MutableElement>& mutableElement)
		{
			const Value& reprValue = entityValue["Representations"];
			if (reprValue.IsArray())
			{
				mutableElement->Representations.clear();
				for (SizeType i = 0; i < reprValue.Size(); i++)
				{
					mutableElement->Representations.push_back(ParseAssetReference(reprValue[i]));
				}
			}
			mutableElement->RepresentationIndex = entityValue["RepresentationIndex"].GetInt();
		}

		void JsonScenarioParser::ParseElement(const rapidjson::Value& entityValue, std::shared_ptr<Element>& element)
		{
			const Value& transform = entityValue["LocalTransform"];
			StartContext("LocalTransform");
			ParseLocalTransform(transform, element->Transform);
			element->ClearSockets();
			if (CheckHasArray(entityValue, "Sockets", true))
			{
				StartContext("Sockets");
				const Value& socketValues = entityValue["Sockets"];
				for (SizeType i = 0; i < socketValues.Size(); i++)
				{
					StartContext(i);
					const Value& csValue = socketValues[i];
					SocketInfo socketInfo;
					ParseSocketInfo(csValue, socketInfo);
					element->SetSocket(socketInfo);
					EndContext();
				}
				EndContext();
			}
			EndContext();
			if (element->IsA("ConnectionElement"))
			{
				std::shared_ptr <ConnectionElement> connection = std::static_pointer_cast<ConnectionElement>(element);
				ParseConnection(entityValue, connection);
			}
			if (element->IsA("GroupElement"))
			{
				std::shared_ptr <GroupElement> groupElement = std::static_pointer_cast<GroupElement>(element);
				ParseGroupElement(entityValue, groupElement);
			}
			if (element->IsA("MutableElement"))
			{
				std::shared_ptr <MutableElement> mutableElement = std::static_pointer_cast<MutableElement>(element);
				ParseMutableElement(entityValue, mutableElement);
			}
		}


		EntityIdentity JsonScenarioParser::ParseEntityIdentity(const rapidjson::Value& entityIdentityValue)
		{
			EntityIdentity identity;
			identity.Identifier = GetAsString(entityIdentityValue, "Identifier");
			identity.Type = GetAsString(entityIdentityValue, "Type", true);
			identity.Category = GetAsString(entityIdentityValue, "Category", true);
			identity.Description = GetAsString(entityIdentityValue, "Description", true);
			return identity;
		}


		std::shared_ptr<Entity> JsonScenarioParser::ParseEntity(const rapidjson::Value& entityValue)
		{
			std::string entityClass = GetAsString(entityValue, "Class");
			std::shared_ptr<Entity> entityData = MakeEntity(entityClass);
			EntityIdentity identity;
			//identity.Identifier = GetAsString(entityValue, "Identifier");
			//identity.Type = GetAsString(entityValue, "Type", true);
			//identity.Category = GetAsString(entityValue, "Category", true);
			//identity.Description = GetAsString(entityValue, "Description", true);
			entityData->Identity = ParseEntityIdentity(entityValue);
			entityData->Configuration = GetAsString(entityValue, "Configuration", true);
			entityData->Asset = GetAsAssetReference(entityValue, "Asset", true);
			if (entityData->IsA("Element"))
			{
				std::shared_ptr <Element> element = std::static_pointer_cast<Element>(entityData);
				ParseElement(entityValue, element);
			}
			if (entityData->IsA("Aggregate"))
			{
				std::shared_ptr <Aggregate> aggregate = std::static_pointer_cast<Aggregate>(entityData);
				ParseAggregate(entityValue, aggregate);
			}

			return entityData;
		}


	}
}
