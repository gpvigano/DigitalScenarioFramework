//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonCatalogParser.h"

namespace discenfw
{
	namespace json
	{

		JsonCatalogParser::JsonCatalogParser()
		{
		}


		std::shared_ptr<Catalog> JsonCatalogParser::ParseCatalog(const std::string& jsonText)
		{
			using namespace rapidjson;

			StartContext("Catalog");
			std::shared_ptr<Catalog> catalog = std::make_shared<Catalog>();

			Parse(jsonText.c_str());

			const Value& catalogValue = GetRootElement("Catalog");

			catalog->Name = GetAsString(catalogValue, "Name");
			if (catalogValue.HasMember("Items"))
			{
				const Value& itemsValue = catalogValue["Items"];
				if (CheckIsArray(itemsValue))
				{
					StartContext("Items");
					for (SizeType i = 0; i < itemsValue.Size(); i++)
					{
						StartContext(std::to_string(i));
						catalog->Items.push_back(ParseCatalogItem(itemsValue[i]));
						EndContext();
					}
					EndContext();
				}
			}
			if (catalogValue.HasMember("CategoryAbbreviations"))
			{
				const Value& abbreviationValue = catalogValue["CategoryAbbreviations"];
				if (CheckIsArray(abbreviationValue))
				{
					StartContext("CategoryAbbreviations");
					for (SizeType i = 0; i < abbreviationValue.Size(); i++)
					{
						StartContext(std::to_string(i));
						if (CheckIsArray(abbreviationValue[i]))
						{
							if (abbreviationValue[i].Size()<2)
							{
								AddJsonError(gpvulc::json::ErrorType::MISSING_MEMBER, "(type tag)");
							}
							else
							{
								std::string typeName = abbreviationValue[i][0].GetString();
								std::string tagName = abbreviationValue[i][1].GetString();
								catalog->CategoryAbbreviations[typeName] = tagName;
							}
						}
						//catalog->CategoryAbbreviations[abbreviationValue[i]["Type"].GetString()]
						//	= abbreviationValue[i]["Tag"].GetString();
						EndContext();
					}
					EndContext();
				}
			}
			EndContext();

			return catalog;
		}


		EntityBadge JsonCatalogParser::ParseEntityBadge(const rapidjson::Value& itemValue)
		{
			EntityBadge badge;
			ParseEntityIdentityMembers(itemValue, badge);

			badge.DisplayName = GetAsString(itemValue, "DisplayName", true);
			badge.Icon = GetAsString(itemValue, "Icon", true);

			return badge;
		}


		CatalogItem JsonCatalogParser::ParseCatalogItem(const rapidjson::Value& itemValue)
		{
			CatalogItem item;

			StartContext("Badge");
			item.Badge = ParseEntityBadge(itemValue["Badge"]);
			EndContext();

			item.Asset = GetAsAssetDefinition(itemValue, "Asset");
			item.FirstOne = GetAsBool(itemValue, "FirstOne", true, false);
			item.AllowedInstances = GetAsInt(itemValue, "AllowedInstances", true, -1);
			item.Hidden = GetAsBool(itemValue, "Hidden", true, false);

			return item;
		}

	}
}
