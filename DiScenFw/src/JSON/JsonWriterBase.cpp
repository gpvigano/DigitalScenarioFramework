//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonWriterBase.h"

namespace discenfw
{
	namespace json
	{
		using namespace discenfw;
		using namespace rapidjson;

		JsonWriterBase::JsonWriterBase()
			: RapidJsonWriter()
		{
		}


		void JsonWriterBase::WriteVec3(const char* memberName, const Vector3D& vec)
		{
			StartObject(memberName);
			WriteFloat("Right", vec.Right);
			WriteFloat("Forward", vec.Forward);
			WriteFloat("Up", vec.Up);
			EndObject();
		}


		void JsonWriterBase::WriteLocation3D(const char* memberName, const Location3D& location)
		{
			StartObject(memberName);
			WriteVec3("Position", location.Position);
			WriteVec3("Rotation", location.Rotation);
			EndObject();
		}


		void JsonWriterBase::WriteCoordSys(const char* memberName, const CoordSys3D& coordSys)
		{
			StartObject(memberName);
			WriteVec3("RightAxis", coordSys.RightAxis);
			WriteVec3("ForwardAxis", coordSys.ForwardAxis);
			WriteVec3("UpAxis", coordSys.UpAxis);
			WriteVec3("Origin", coordSys.Origin);
			EndObject();
		}


		void JsonWriterBase::WriteTransformMembers(const Transform3D& transform)
		{
			WriteBool("UseCoordSys",transform.UseCoordSys);
			if (transform.UseCoordSys)
			{
				WriteCoordSys("CoordSys",transform.CoordSys);
			}
			else
			{
				WriteVec3("Location",transform.Location);
				WriteVec3("EulerAngles",transform.EulerAngles);
			}
			WriteVec3("Scale",transform.Scale);
		}


		void JsonWriterBase::WriteLocalTransform(const char* memberName, const LocalTransform& transform)
		{
			StartObject(memberName);
			WriteString("ParentId", transform.ParentId);
			WriteTransformMembers(transform);
			EndObject();
		}


		void JsonWriterBase::WriteTransform(const char* memberName, const Transform3D& transform)
		{
			StartObject(memberName);
			WriteTransformMembers(transform);
			EndObject();
		}


		void JsonWriterBase::WriteAssetDefinitionMembers(const AssetDefinition& assetDef)
		{
			WriteString("Uri", assetDef.Uri);
			WriteString("AssetType", assetDef.AssetType);
			WriteString("Source", assetDef.GetSourceString());
			if (!assetDef.PartId.empty())
			{
				WriteString("PartId", assetDef.PartId);
			}
		}


		void JsonWriterBase::WriteAssetDefinition(const char* memberName, const AssetDefinition& assetDef)
		{
			StartObject(memberName);
			WriteAssetDefinitionMembers(assetDef);
			EndObject();
		}


		void JsonWriterBase::WriteAssetReference(const char* memberName, const AssetReference& assetRef)
		{
			StartObject(memberName);
			WriteAssetDefinitionMembers(assetRef);
			WriteString("Catalog", assetRef.Catalog);
			EndObject();
		}


		void JsonWriterBase::WritePath3D(const std::vector<Vector3D>& path)
		{
			StartArray();
			for (unsigned i = 0; i < path.size(); i++)
			{
				WriteVec3(nullptr, path[i]);
			}
			EndArray();
		}


		void JsonWriterBase::WriteEntityIdentityMembers(const EntityIdentity& entityIdentity)
		{
			WriteString("Identifier", entityIdentity.Identifier);
			WriteString("Type", entityIdentity.Type);
			WriteString("Category", entityIdentity.Category);
			WriteString("Description", entityIdentity.Description);
		}
	}
}
