//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


#include "discenfw/scen/Catalog.h"

namespace discenfw
{
	Catalog::Catalog()
	{
	}


	std::shared_ptr<Catalog> Catalog::Make()
	{
		return std::make_shared<Catalog>();
	}


	void Catalog::SetName(const std::string& name)
	{
		Name = name;
	}


	void Catalog::SetItems(const std::vector<CatalogItem>& items)
	{
		Items = items;
	}


	void Catalog::AddItem(const CatalogItem& item)
	{
		Items.push_back(item);
	}


	void Catalog::ClearItems()
	{
		Items.clear();
	}


	void Catalog::SetCategoryAbbreviations(const std::map<std::string, std::string>& abbreviations)
	{
		CategoryAbbreviations = abbreviations;
	}
}
