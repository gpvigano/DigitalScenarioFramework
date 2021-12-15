//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/MutableElement.h>


namespace discenfw
{
	MutableElement::MutableElement()
	{
	}


	MutableElement::MutableElement(const std::string& id, const std::string& entityType)
		: Element(id, entityType)
	{
	}


	MutableElement::MutableElement(const std::string& id, const std::string category, const LocalTransform& transform, const std::vector<AssetReference>& repr, unsigned reprIndex)
		: Element(id, category, transform), Representations(repr), RepresentationIndex(reprIndex)
	{
	}
}
