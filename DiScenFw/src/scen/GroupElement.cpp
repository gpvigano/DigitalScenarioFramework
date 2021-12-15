//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/GroupElement.h>

#include <algorithm>


namespace discenfw
{
	GroupElement::GroupElement()
	{
	}


	GroupElement::GroupElement(const std::string& id, const std::string& entityType)
		: Element(id, entityType)
	{
	}


	GroupElement::GroupElement(const std::string& id, const std::string& entityType, const LocalTransform& transform, const std::vector<std::shared_ptr<Element>>& parts)
		: Element(id, entityType, transform), Parts(parts)
	{
	}


	void GroupElement::SetParts(const std::vector<std::shared_ptr<Element>>& parts)
	{
		Parts = parts;
	}


	void GroupElement::AddPart(std::shared_ptr<Element> part)
	{
		auto partIt = std::find(Parts.begin(), Parts.end(), part);
		if (partIt == Parts.end())
		{
			Parts.push_back(part);
		}
	}

	void GroupElement::RemovePart(std::shared_ptr<Element> part)
	{
		auto partIt = std::find(Parts.begin(), Parts.end(), part);
		if(partIt != Parts.end())
		{
			Parts.erase(partIt);
		}
	}
}
