//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/scen/AssetDefinition.h"

#include <string>

namespace discenfw
{
	//! Reference to an asset defined out of the framework.
	struct AssetReference : public AssetDefinition
	{

		//! Catalog of assets containing this asset.
		std::string Catalog;


		/// @name Constructors.
		///@{

		AssetReference() {}

		AssetReference(const std::string& catalogId, const std::string& assetType, const std::string& uri, SourceType assetSource)
			: AssetDefinition(uri, assetType, assetSource), Catalog(catalogId)
		{
		}

		AssetReference(const std::string& catalogId, const std::string& assetType, const std::string& uri, const std::string& partId, SourceType assetSource)
			: AssetDefinition(uri, assetType, partId, assetSource), Catalog(catalogId)
		{
		}

		///@}

	};
}
