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
#include "discenfw/scen/Element.h"


#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
#     pragma warning(disable : 4275) // disable warning C4275: non dll-interface class ... used as base for dll-interface class
#   endif

namespace discenfw
{
	/*!
	Scenario data, container of all the entities of the scenario.
	*/
	class Scenario
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		std::string Name;

		std::vector< std::shared_ptr<Entity> > Entities;

		/// @name Constructors.
		///@{

		Scenario();

		Scenario(const std::string& name);

		///@}

	public:

		/*!
		Get a shared pointer to an entity in the scenario given its identifier
		(or nullptr if not found).
		*/
		std::shared_ptr<Entity> GetEntityById(const std::string& id);


		/*!
		Get a shared pointer to an element in the scenario given its identifier
		(or nullptr if not found).
		*/
		std::shared_ptr<Element> GetElementById(const std::string& id);

		void AddEntity(std::shared_ptr<Entity> entity);

		bool RemoveEntity(std::shared_ptr<Entity> entity);

		void ClearEntities();

		const std::string& GetName()
		{
			return Name;
		}

		const std::vector< std::shared_ptr<Entity> >& GetEntities() const
		{
			return Entities;
		}
	};
}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
