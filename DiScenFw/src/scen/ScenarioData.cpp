//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/ScenarioData.h>
#include <discenfw/scen/Entity.h>
#include <discenfw/scen/Aggregate.h>
#include <discenfw/scen/Element.h>
#include <discenfw/scen/GroupElement.h>
#include <discenfw/scen/ConnectionElement.h>
#include <discenfw/scen/MutableElement.h>

#include <memory>


namespace discenfw
{
	std::shared_ptr<Entity> MakeEntity(const std::string& entityClass)
	{
		if (entityClass == "Entity") return std::make_shared<Entity>();
		if (entityClass == "Aggregate") return std::make_shared<Aggregate>();
		if (entityClass == "Element") return std::make_shared<Element>();
		if (entityClass == "GroupElement") return std::make_shared<GroupElement>();
		if (entityClass == "ConnectionElement") return std::make_shared<ConnectionElement>();
		if (entityClass == "MutableElement") return std::make_shared<MutableElement>();
		return nullptr;
	}


	std::shared_ptr<Entity> MakeEntity(const std::string& entityClass, const std::string& entityId, const std::string& entityType)
	{
		if (entityClass == "Entity") return std::make_shared<Entity>(entityId, entityType);
		if (entityClass == "Aggregate") return std::make_shared<Aggregate>(entityId, entityType);
		if (entityClass == "Element") return std::make_shared<Element>(entityId, entityType);
		if (entityClass == "GroupElement") return std::make_shared<GroupElement>(entityId, entityType);
		if (entityClass == "ConnectionElement") return std::make_shared<ConnectionElement>(entityId, entityType);
		if (entityClass == "MutableElement") return std::make_shared<MutableElement>(entityId, entityType);
		return nullptr;
	}


	std::shared_ptr<Entity> CloneEntity(const std::shared_ptr<Entity> originalEntity)
	{
		if (!originalEntity)
		{
			return nullptr;
		}
		const std::string& entityClass = originalEntity->GetClassName();
		if (originalEntity->IsExactlyOfType("Entity"))
		{
			std::shared_ptr<Entity> clonedEntity;
			clonedEntity = std::make_shared<Entity>();
			*clonedEntity = *originalEntity;
			return clonedEntity;
		}
#define CHECK_CLONE_ENTITY(className) \
		if (originalEntity->IsExactlyOfType(#className)) \
		{ \
			std::shared_ptr<className> cloned = std::make_shared<className>(); \
			*cloned = *(std::static_pointer_cast<className>(originalEntity)); \
			return cloned; \
		}

		CHECK_CLONE_ENTITY(Aggregate)
		CHECK_CLONE_ENTITY(Element)
		CHECK_CLONE_ENTITY(GroupElement)
		CHECK_CLONE_ENTITY(ConnectionElement)
		CHECK_CLONE_ENTITY(MutableElement)
#undef CHECK_CLONE_ENTITY

		return nullptr;
	}

}
