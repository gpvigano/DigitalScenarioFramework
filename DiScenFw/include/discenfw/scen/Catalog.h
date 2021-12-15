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
#include "discenfw/scen/CatalogItem.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace discenfw
{
	/*!
	Catalog data.
	*/
	class DISCENFW_API Catalog
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		/// Members protected from public access from DLL clients
		/// to prevent allocation/deallocation in different modules
		/// (see @ref dynamic_linkage "dynamic linkage section").
		/// @name Protected (or public for static linking) members.
		/// @{

		/*!
		Name of the catalog.
		*/
		std::string Name;

		/*!
		List of items included in the catalog.
		*/
		std::vector<CatalogItem> Items;

		/*!
		Catalog categories with their abbreviations, used to group items.
		*/
		std::map<std::string,std::string> CategoryAbbreviations;

		Catalog();

		///@}

	public:
		/*!
		Make a new catalog (allocated in the library module), return a shared pointer to it.
		*/
		static std::shared_ptr<Catalog> Make();

		/*!
		Get the name of the catalog.
		*/
		const std::string& GetName() const
		{
			return Name;
		}

		/*!
		Set the name of the catalog.
		*/
		void SetName(const std::string& name);


		/*!
		Get the list of items included in the catalog.
		*/
		const std::vector<CatalogItem>& GetItems()
		{
			return Items;
		}

		/*!
		Set the list of items included in the catalog.
		*/
		void SetItems(const std::vector<CatalogItem>& items);

		/*!
		Add an item to the catalog.
		*/
		void AddItem(const CatalogItem& item);

		/*!
		Clear all the items in the catalog.
		*/
		void ClearItems();


		/*!
		Get the catalog categories with their abbreviations.
		*/
		const std::map<std::string, std::string>& GetCategoryAbbreviations()
		{
			return CategoryAbbreviations;
		}

		/*!
		Set the catalog categories with their abbreviations.
		*/
		void SetCategoryAbbreviations(const std::map<std::string, std::string>& abbreviations);
	};
}
