//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFwConfig.h>
#include "discenfw/scen/Entity.h"
#include "discenfw/scen/Aggregate.h"
#include "discenfw/scen/Element.h"
#include "discenfw/scen/GroupElement.h"
#include "discenfw/scen/ConnectionElement.h"
#include "discenfw/scen/MutableElement.h"
#include "discenfw/scen/Scenario.h"

#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
#     pragma warning(disable : 4275) // disable warning C4275: non dll-interface class ... used as base for dll-interface class
#   endif

namespace discenfw
{

	/*!
	Create an entity with the proper type, given its class name.
	*/
	DISCENFW_API std::shared_ptr<Entity> MakeEntity(const std::string& entityClass);


	/*!
	Create an entity with the proper type, given its class name.
	*/
	DISCENFW_API std::shared_ptr<Entity> MakeEntity(const std::string& entityClass, const std::string& entityId, const std::string& entityType = std::string());

	/*!
	Clone an entity.
	*/
	DISCENFW_API std::shared_ptr<Entity> CloneEntity(const std::shared_ptr<Entity> originalEntity);



}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
