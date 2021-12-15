//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <gpvulc/json/RapidJsonParser.h>
#include <discenfw/scen/CommonData.h>
#include <discenfw/scen/AssetReference.h>
#include <discenfw/scen/EntityIdentity.h>


namespace discenfw
{
	namespace json
	{

		class JsonParserBase : protected gpvulc::json::RapidJsonParser
		{
		public:

			JsonParserBase();

			void CheckJsonErrors() { gpvulc::json::RapidJsonParser::CheckJsonErrors(); }


		protected:

			Vector3D ParseVec3(const rapidjson::Value& vecValue);


			void GetVec3(const rapidjson::Value& vecValue, const char* name, Vector3D& outVec, bool optional = false);


			void ParseCoordSys(const rapidjson::Value& coordSysValue, CoordSys3D& coordSys);


			void ParseLocation3D(const rapidjson::Value& locationValue, Location3D& coordSys);


			void ParseTransform3D(const rapidjson::Value& transformValue, Transform3D& transform);


			void ParseLocalTransform(const rapidjson::Value& transformValue, LocalTransform& transform);


			void ParsePath3D(const rapidjson::Value& pathValue, std::vector<Vector3D>& path);


			void ParseAssetDefinitionMembers(const rapidjson::Value& assetDefinitionValue, AssetDefinition& assetDefinition);


			void ParseEntityIdentityMembers(const rapidjson::Value& entityIdentityValue, EntityIdentity& entityIdentity);


			AssetDefinition ParseAssetDefinition(const rapidjson::Value& assetDefinitionValue);


			AssetReference ParseAssetReference(const rapidjson::Value& assetReferenceValue);


			AssetDefinition GetAsAssetDefinition(const rapidjson::Value& ownerValue, const char* name);


			AssetReference GetAsAssetReference(const rapidjson::Value& ownerValue, const char* name, bool optional = false);

		};

	}
}
