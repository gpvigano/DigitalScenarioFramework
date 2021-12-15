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

#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
#     pragma warning(disable : 4275) // disable warning C4275: non dll-interface class ... used as base for dll-interface class
#   endif

namespace discenfw
{
	/*!
	Aggregate entity.
	*/
	class DISCENFW_API Aggregate : public Entity
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		/*!
		List of references to entities that compose the aggregate.
		*/
		std::vector< std::shared_ptr<Entity> > Components;

		/// @name Constructors.
		///@{

		Aggregate();

		Aggregate(const std::string& id, const std::string& entityType);

		Aggregate(
			const std::string& id,
			const std::string& entityType,
			const AssetReference& assetRef
			);

		///@}

		virtual ~Aggregate() {}

	public:

		virtual const char* GetClassName() const override
		{
			return "Aggregate";
		}

		virtual bool IsA(const std::string& classType) const override
		{
			return classType == Aggregate::GetClassName() || Entity::IsA(classType);
		}

		const std::vector< std::shared_ptr<Entity> >& GetComponents()
		{
			return Components;
		}

		void SetComponents(const std::vector< std::shared_ptr<Entity> >& components);
	};
}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
