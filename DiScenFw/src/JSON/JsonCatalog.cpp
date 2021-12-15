//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonCatalogParser.h"
#include "JsonCatalogWriter.h"

#include <memory>

using namespace discenfw::json;

namespace discenfw
{
	namespace scen
	{
		std::shared_ptr<Catalog> CatalogFromJson(const std::string& jsonText)
		{
			JsonCatalogParser parser;
			//parser.Reset();
			std::shared_ptr<Catalog> catalog = parser.ParseCatalog(jsonText);
			parser.CheckJsonErrors();
			return catalog;
		}

		void CatalogToJson(const std::shared_ptr<Catalog>& catalog, std::string& jsonText)
		{
			JsonCatalogWriter writer;
			writer.WriteCatalog(catalog, jsonText);
		}
	}
}

