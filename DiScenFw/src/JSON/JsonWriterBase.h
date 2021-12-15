//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <gpvulc/json/RapidJsonWriter.h>
#include <discenfw/scen/CommonData.h>
#include <discenfw/scen/AssetReference.h>
#include <discenfw/scen/EntityIdentity.h>

namespace discenfw
{
	namespace json
	{

		class JsonWriterBase : protected gpvulc::json::RapidJsonWriter
		{
		public:
			JsonWriterBase();

		protected:

			void WriteVec3(const char* memberName, const Vector3D& vec);

			void WriteLocation3D(const char* memberName, const Location3D& location);

			void WriteCoordSys(const char* memberName, const CoordSys3D& coordSys);

			void WriteTransformMembers(const Transform3D& transform);

			void WriteLocalTransform(const char* memberName, const LocalTransform& transform);

			void WriteTransform(const char* memberName, const Transform3D& transform);

			void WriteAssetDefinitionMembers(const AssetDefinition& assetDef);

			void WriteAssetDefinition(const char* memberName, const AssetDefinition& assetDef);

			void WriteAssetReference(const char* memberName, const AssetReference& assetRef);

			void WritePath3D(const std::vector<Vector3D>& path);

			void WriteEntityIdentityMembers(const EntityIdentity& entityIdentity);
		};

	}
}
