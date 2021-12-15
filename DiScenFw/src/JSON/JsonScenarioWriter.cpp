//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonScenarioWriter.h"


namespace discenfw
{
	namespace json
	{
		using namespace rapidjson;
		using namespace discenfw;


		JsonScenarioWriter::JsonScenarioWriter()
			: JsonWriterBase()
		{
		}


		void JsonScenarioWriter::WriteSocket(const SocketInfo& socketInfo)
		{
			StartObject();
			WriteString("Id", socketInfo.Id);
			if (!socketInfo.Type.empty())
			{
				WriteString("Type", socketInfo.Type);
			}
			if (!socketInfo.Compatibility.empty())
			{
				StartArray("Compatibility");
				for (size_t i = 0; i < socketInfo.Compatibility.size(); i++)
				{
					WriteString(nullptr, socketInfo.Compatibility[i]);
				}
				EndArray();
			}
			WriteCoordSys("Location", socketInfo.Location);
			EndObject();
		}


		void JsonScenarioWriter::WriteAnchor(const Anchor& anchor)
		{
			StartObject();
			if (!anchor.ElementId.empty())
			{
				WriteString("ElementId", anchor.ElementId);
			}
			if (!anchor.SocketId.empty())
			{
				WriteString("SocketId", anchor.SocketId);
			}
			WriteVec3("Offset", anchor.Offset);
			EndObject();
		}


		void JsonScenarioWriter::WriteConnection(const std::shared_ptr<ConnectionElement>& connection)
		{
			if (!connection->IsConnectionPathEmpty())
			{
				StartArray("ConnectionPath");
				for (size_t i = 0; i < connection->GetNumAnchors(); i++)
				{
					WriteAnchor(connection->GetAnchor(i));
				}
				EndArray();
			}
		}


		void JsonScenarioWriter::WriteMutableElement(const std::shared_ptr<MutableElement>& mutableElement)
		{
			StartArray("Representations");
			for (size_t i = 0; i < mutableElement->Representations.size(); i++)
			{
				WriteAssetReference(nullptr, mutableElement->Representations[i]);
			}
			EndArray();
			WriteInt("RepresentationIndex", mutableElement->RepresentationIndex);
		}


		void JsonScenarioWriter::WriteElement(const std::shared_ptr<Element>& element)
		{
			WriteLocalTransform("LocalTransform", element->Transform);
			if (!element->GetSockets().empty())
			{
				StartArray("Sockets");
				for (const auto& socket : element->GetSockets())
				{
					WriteSocket(socket);
				}
				EndArray();
			}
			if (element->IsA("ConnectionElement"))
			{
				std::shared_ptr<ConnectionElement> connection = std::static_pointer_cast<ConnectionElement>(element);
				WriteConnection(connection);
			}
			if (element->IsA("MutableElement"))
			{
				std::shared_ptr<MutableElement> mutableElement = std::static_pointer_cast<MutableElement>(element);
				WriteMutableElement(mutableElement);
			}
		}


		void JsonScenarioWriter::WriteAggregate(const std::shared_ptr<Aggregate>& sys)
		{
			const std::vector<std::shared_ptr<Entity>>& components = sys->Components;
			StartArray("Components");
			for (unsigned i = 0; i < components.size(); i++)
			{
				WriteEntity(components[i]);
			}
			EndArray();
		}


		void JsonScenarioWriter::WriteGroupElement(const std::shared_ptr<GroupElement>& groupElement)
		{
			const std::vector<std::shared_ptr<Element>>& parts = groupElement->Parts;
			StartArray("Parts");
			for (unsigned i = 0; i < parts.size(); i++)
			{
				WriteElement(parts[i]);
			}
			EndArray();
		}


		void JsonScenarioWriter::WriteEntityIdentity(const EntityIdentity& entityIdentity)
		{
			WriteString("Identifier", entityIdentity.Identifier);
			WriteString("Type", entityIdentity.Type);
			WriteString("Category", entityIdentity.Category);
			WriteString("Description", entityIdentity.Description);
		}


		void JsonScenarioWriter::WriteEntity(const std::shared_ptr<Entity>& entity)
		{
			StartObject();
			WriteString("Class", entity->GetClassName());
			WriteEntityIdentity(entity->Identity);
			//WriteString("Identifier", entity->Identifier);
			//WriteString("Type", entity->Type);
			//WriteString("Category", entity->Category);
			//WriteString("Description", entity->Description);
			WriteString("Configuration", entity->Configuration);
			WriteAssetReference("Asset", entity->Asset);
			if (entity->IsA("Element"))
			{
				std::shared_ptr<Element> element = std::static_pointer_cast<Element>(entity);
				WriteElement(element);
			}
			if (entity->IsA("Aggregate"))
			{
				std::shared_ptr<Aggregate> sys = std::static_pointer_cast<Aggregate>(entity);
				WriteAggregate(sys);
			}
			EndObject();
		}


		void JsonScenarioWriter::WriteScenario(const Scenario& scenarioData, std::string& jsonText)
		{
			StartDocument();
			StartObject("Scenario");
			SetDecimalPrecision(5);

			WriteString("Name",scenarioData.Name.c_str());
			StartArray("Entities");
			for (unsigned i = 0; i < scenarioData.Entities.size(); i++)
			{
				std::shared_ptr<Entity> entity = scenarioData.Entities[i];
				WriteEntity(entity);
			}
			EndArray();

			EndObject();
			EndDocument(jsonText);
		}
	}
}

