//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonCatalogWriter.h"


namespace discenfw
{
	namespace json
	{
		using namespace rapidjson;
		using namespace discenfw;

		JsonCatalogWriter::JsonCatalogWriter()
			: JsonWriterBase()
		{
		}


		void JsonCatalogWriter::WriteEntityBadge(const EntityBadge& badge)
		{
			StartObject("Badge");
			WriteEntityIdentityMembers(badge);
			WriteString("DisplayName", badge.DisplayName);
			WriteString("Icon", badge.Icon);
			EndObject();
		}


		void JsonCatalogWriter::WriteCatalogItem(const CatalogItem& item)
		{
			StartObject();
			WriteEntityBadge(item.Badge);
			WriteAssetDefinition("Asset", item.Asset);
			WriteBool("FirstOne", item.FirstOne);
			if (item.AllowedInstances >= 0)
			{
				WriteInt("AllowedInstances", item.AllowedInstances);
			}
			WriteBool("Hidden", item.Hidden);
			EndObject();
		}


		void JsonCatalogWriter::WriteCatalog(const std::shared_ptr<Catalog>& catalog, std::string& jsonText)
		{
			StartDocument();
			StartObject("Catalog");

			WriteString("Name", catalog->Name.c_str());
			StartArray("Items");
			for (unsigned i = 0; i < catalog->Items.size(); i++)
			{
				WriteCatalogItem(catalog->Items[i]);
			}
			EndArray();
			if (!catalog->CategoryAbbreviations.empty())
			{
				StartArray("CategoryAbbreviations");
				for (const auto& abbreviation : catalog->CategoryAbbreviations)
				{
					StartArray();
					WriteString(nullptr, abbreviation.first.c_str());
					WriteString(nullptr, abbreviation.second.c_str());
					EndArray();
				}
				EndArray();
			}
			EndObject();
			EndDocument(jsonText);
		}
	}
}

