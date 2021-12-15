//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonParserBase.h"
#include <discenfw/scen/Catalog.h>


namespace discenfw
{
	namespace json
	{
		class JsonCatalogParser : public JsonParserBase
		{
		public:

			JsonCatalogParser();

			std::shared_ptr<Catalog> ParseCatalog(const std::string& jsonText);

		protected:

			EntityBadge ParseEntityBadge(const rapidjson::Value& itemValue);

			CatalogItem ParseCatalogItem(const rapidjson::Value& itemValue);
		};

	}
}

