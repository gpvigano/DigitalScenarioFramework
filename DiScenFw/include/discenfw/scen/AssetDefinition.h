//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <string>

namespace discenfw
{
	//! Definition of an asset.
	struct AssetDefinition
	{
		//! Type of source containing the asset.
		enum SourceType
		{
			SCENE = 0,
			PROJECT,
			EXTERNAL,
			UNDEFINED
		} Source = UNDEFINED;

		//! Uniform resource identifier of this asset.
		std::string Uri;

		//! Identifier of the asset part.
		std::string PartId;

		//! Type of this asset.
		std::string AssetType;


		/// @name Constructors.
		///@{

		AssetDefinition() {}

		AssetDefinition(const std::string& uri, const std::string& assetType, SourceType assetSource)
			: Source(assetSource), Uri(uri), AssetType(assetType)
		{}

		AssetDefinition(const std::string& uri, const std::string& assetType, const std::string& partId, SourceType assetSource)
			: Source(assetSource), Uri(uri), PartId(partId), AssetType(assetType)
		{}

		///@}


		//! Get the asset source as string.
		std::string GetSourceString() const;

		//! Set the asset source from a string.
		void SetSourceString(const std::string& src);

		bool IsDefined()
		{
			return Source != UNDEFINED;
		}
	};


	//! Convert the asset source to a string.
	inline std::string AssetDefinition::GetSourceString() const
	{
		switch (Source)
		{
		case AssetDefinition::SCENE:
			return "SCENE";
		case AssetDefinition::PROJECT:
			return "PROJECT";
		case AssetDefinition::EXTERNAL:
			return "EXTERNAL";
		default:
			return "";
		}
	}

	//! Convert a string to asset source and set it to this asset.
	inline void AssetDefinition::SetSourceString(const std::string& src)
	{
		Source = UNDEFINED;
		if (src == "SCENE") Source = AssetDefinition::SCENE;
		if (src == "PROJECT") Source = AssetDefinition::PROJECT;
		if (src == "EXTERNAL") Source = AssetDefinition::EXTERNAL;
	}
}
