//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonParserBase.h"


namespace discenfw
{
	namespace json
	{
		using namespace gpvulc::json;

		JsonParserBase::JsonParserBase()
		{
		}


		Vector3D JsonParserBase::ParseVec3(const rapidjson::Value& vecValue)
		{
			Vector3D vec;
			//vec.Right = vecValue["X"].GetFloat();
			//vec.Forward = vecValue["Y"].GetFloat();
			//vec.Up = vecValue["Z"].GetFloat();
			vec.Right = GetAsFloat(vecValue, "Right");
			vec.Forward = GetAsFloat(vecValue, "Forward");
			vec.Up = GetAsFloat(vecValue, "Up");
			return vec;
		}


		void JsonParserBase::GetVec3(const rapidjson::Value& vecValue, const char* name, Vector3D& outVec, bool optional)
		{
			if (vecValue.HasMember(name))
			{
				StartContext(name);
				outVec = ParseVec3(vecValue[name]);
				EndContext();
			}
			else
			{
				AddJsonError(ErrorType::MISSING_MEMBER, name);
			}
		}

		void JsonParserBase::ParseCoordSys(const rapidjson::Value& coordSysValue, CoordSys3D& coordSys)
		{
			if (CheckIsObject(coordSysValue))
			{
				GetVec3(coordSysValue, "RightAxis", coordSys.RightAxis);
				GetVec3(coordSysValue, "ForwardAxis", coordSys.ForwardAxis);
				GetVec3(coordSysValue, "UpAxis", coordSys.UpAxis);
				GetVec3(coordSysValue, "Origin", coordSys.Origin);
			}
		}

		void JsonParserBase::ParseLocation3D(const rapidjson::Value& locationValue, Location3D& coordSys)
		{
			if (CheckIsObject(locationValue))
			{
				GetVec3(locationValue, "Position", coordSys.Position);
				GetVec3(locationValue, "Rotation", coordSys.Rotation);
			}
		}

		void JsonParserBase::ParseTransform3D(const rapidjson::Value& transformValue, Transform3D& transform)
		{
			if (CheckIsObject(transformValue))
			{
				if (transformValue.HasMember("CoordSys"))
				{
					StartContext("CoordSys");
					const rapidjson::Value& coordSys = transformValue["CoordSys"];
					transform.UseCoordSys = true;
					ParseCoordSys(coordSys, transform.CoordSys);
					EndContext();
				}
				else
				{
					GetVec3(transformValue, "Location", transform.Location);
					GetVec3(transformValue, "EulerAngles", transform.EulerAngles);
				}
				GetVec3(transformValue, "Scale", transform.Scale, true);
			}
		}


		void JsonParserBase::ParseLocalTransform(const rapidjson::Value& transformValue, LocalTransform& transform)
		{
			if (CheckIsObject(transformValue))
			{
				transform.ParentId = GetAsString(transformValue, "ParentId");
				ParseTransform3D(transformValue, transform);
			}
		}


		void JsonParserBase::ParsePath3D(const rapidjson::Value& pathValue, std::vector<Vector3D>& path)
		{
			if (pathValue.IsArray())
			{
				path.clear();
				for (rapidjson::SizeType i = 0; i < pathValue.Size(); i++)
				{
					path.push_back(ParseVec3(pathValue[i]));
				}
			}
		}


		void JsonParserBase::ParseAssetDefinitionMembers(const rapidjson::Value& assetDefinitionValue, AssetDefinition& assetDefinition)
		{
			if (CheckIsObject(assetDefinitionValue))
			{
				//assetDefinition.SetSourceString( assetDefinitionValue["Source"].GetString() );
				//assetDefinition.Uri = assetDefinitionValue["Uri"].GetString();
				//if (assetDefinitionValue.HasMember("PartId"))
				//{
				//	assetDefinition.PartId = assetDefinitionValue["PartId"].GetString();
				//}
				//if (assetDefinitionValue.HasMember("AssetType"))
				//{
				//	assetDefinition.AssetType = assetDefinitionValue["AssetType"].GetString();
				//}
				assetDefinition.SetSourceString(GetAsString(assetDefinitionValue, "Source"));
				assetDefinition.Uri = GetAsString(assetDefinitionValue, "Uri");
				assetDefinition.PartId = GetAsString(assetDefinitionValue, "PartId", true);
				assetDefinition.AssetType = GetAsString(assetDefinitionValue, "AssetType", true);
			}
		}


		void JsonParserBase::ParseEntityIdentityMembers(const rapidjson::Value& entityIdentityValue, EntityIdentity& entityIdentity)
		{
			entityIdentity.Identifier = GetAsString(entityIdentityValue, "Identifier");
			entityIdentity.Type = GetAsString(entityIdentityValue, "Type");
			entityIdentity.Category = GetAsString(entityIdentityValue, "Category");
			entityIdentity.Description = GetAsString(entityIdentityValue, "Description", true);
		}


		AssetDefinition JsonParserBase::ParseAssetDefinition(const rapidjson::Value& assetDefinitionValue)
		{
			AssetDefinition assetDefinition;
			ParseAssetDefinitionMembers(assetDefinitionValue, assetDefinition);
			return assetDefinition;
		}


		AssetReference JsonParserBase::ParseAssetReference(const rapidjson::Value& assetReferenceValue)
		{
			AssetReference assetReference;
			ParseAssetDefinitionMembers(assetReferenceValue, assetReference);
			assetReference.Catalog = GetAsString(assetReferenceValue, "Catalog");
			return assetReference;
		}


		AssetDefinition JsonParserBase::GetAsAssetDefinition(const rapidjson::Value& ownerValue, const char* name)
		{
			if (!ownerValue.HasMember(name))
			{
				AddJsonError(ErrorType::MISSING_MEMBER, name);
				return AssetDefinition();
			}
			StartContext(name);
			AssetDefinition assetDef = ParseAssetDefinition(ownerValue[name]);
			EndContext();
			return assetDef;
		}


		AssetReference JsonParserBase::GetAsAssetReference(const rapidjson::Value& ownerValue, const char* name, bool optional)
		{
			if (!ownerValue.HasMember(name))
			{
				if (!optional)
				{
					AddJsonError(ErrorType::MISSING_MEMBER, name);
				}
				return AssetReference();
			}
			StartContext(name);
			AssetReference assetRef = ParseAssetReference(ownerValue[name]);
			EndContext();
			return assetRef;
		}
	}
}
