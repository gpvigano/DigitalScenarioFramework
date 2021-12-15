//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/scen/Catalog.h>

#include <string>
#include <memory>

namespace discenfw
{
	namespace scen
	{

		/*!
		Parse a Catalog from a JSON text.
		*/
		std::shared_ptr<Catalog> CatalogFromJson(const std::string& jsonText);


		/*!
		Serialize a Catalog to a JSON text.
		*/
		void CatalogToJson(const std::shared_ptr<Catalog>& catalog, std::string& jsonText);

	}
}
