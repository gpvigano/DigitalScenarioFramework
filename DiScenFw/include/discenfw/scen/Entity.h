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
#include <discenfw/util/IBaseClass.h>
#include "discenfw/scen/CommonData.h"
#include "discenfw/scen/AssetReference.h"
#include "discenfw/scen/EntityIdentity.h"

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>

#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
#     pragma warning(disable : 4275) // disable warning C4275: non dll-interface class ... used as base for dll-interface class
#   endif

namespace discenfw
{
	/*!
	Basic entity definition.
	*/
	class DISCENFW_API Entity : protected IBaseClass
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		/*!
		Information about identity.
		*/
		EntityIdentity Identity;

		/*!
		Entity configuration text.
		*/
		std::string Configuration;

		/*!
		Asset reference (if any).
		*/
		AssetReference Asset;

		/// @name Constructors.
		///@{

		Entity();

		Entity(const std::string& id, const std::string& entityType);

		Entity(const std::string& id, const std::string& entityType, const AssetReference& assetRef);

		Entity(
			const std::string& id,
			const std::string& entityType,
			const std::string& category,
			const std::string& configuration,
			const std::string& description,
			const AssetReference& assetRef
			);
		///@}

		virtual ~Entity() {}

	public:

		/*!
		Get the real class name.
		*/
		virtual const char* GetClassName() const
		{
			return "Entity";
		}

		/*!
		Check if this class or one of its ancestors has the given class name.
		*/
		virtual bool IsA(const std::string& classType) const
		{
			return classType == Entity::GetClassName();
		}

		/*!
		Check if this instance has the given class name.
		*/
		bool IsExactlyOfType(const std::string& classType) const
		{
			return IBaseClass::IsExactlyOfType(classType);
		}


		///@name Getters/setters
		///@{

		const std::string& GetIdentifier() const
		{
			return Identity.Identifier;
		}


		const EntityIdentity& GetIdentity() const
		{
			return Identity;
		}

		void SetIdentity(const EntityIdentity& entityIdentity);


		const std::string& GetConfiguration() const
		{
			return Configuration;
		}

		void SetConfiguration(const std::string& entityConfig);


		const AssetReference& GetAssetReference() const
		{
			return Asset;
		}

		void SetAssetReference(const AssetReference& assetReference);

		///@}
	};


}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
