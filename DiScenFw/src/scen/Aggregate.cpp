//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/Aggregate.h>

#include <algorithm>


namespace discenfw
{
	Aggregate::Aggregate()
	{
	}


	Aggregate::Aggregate(const std::string& id, const std::string& entityType)
		: Entity(id, entityType)
	{
	}


	Aggregate::Aggregate(
		const std::string& id,
		const std::string& entityType,
		const AssetReference& assetRef
		) :
		Entity(id, entityType, assetRef)
	{
	}


	void Aggregate::SetComponents(const std::vector<std::shared_ptr<Entity>>& components)
	{
		Components = components;
	}
}
