//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/Entity.h>

#include <algorithm>


namespace discenfw
{
	Entity::Entity()
	{
	}


	Entity::Entity(const std::string& id, const std::string& entityType)
		: Identity(id, entityType)
	{
	}


	Entity::Entity(
		const std::string& id,
		const std::string& entityType,
		const AssetReference& assetRef
		) :
		Identity(id, entityType),
		Asset(assetRef)
	{
	}


	Entity::Entity(
		const std::string& id,
		const std::string& entityType,
		const std::string& category,
		const std::string& configuration,
		const std::string& description,
		const AssetReference& assetRef
		) :
		Identity(id, entityType, category, description),
		Configuration(configuration),
		Asset(assetRef)
	{
	}


	void Entity::SetIdentity(const EntityIdentity & entityIdentity)
	{
		Identity = entityIdentity;
	}


	void Entity::SetConfiguration(const std::string& entityConfig)
	{
		Configuration = entityConfig;
	}

	void Entity::SetAssetReference(const AssetReference& assetReference)
	{
		Asset = assetReference;
	}
}
