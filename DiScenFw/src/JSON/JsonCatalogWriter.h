//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonWriterBase.h"
#include <discenfw/scen/Catalog.h>

#include <memory>

namespace discenfw
{
	namespace json
	{

		class JsonCatalogWriter : public JsonWriterBase
		{
		public:
			JsonCatalogWriter();

			void WriteCatalog(const std::shared_ptr<Catalog>& catalog, std::string& jsonText);
		protected:

			void WriteEntityBadge(const EntityBadge& badge);
			void WriteCatalogItem(const CatalogItem& item);
		};

	}
}
