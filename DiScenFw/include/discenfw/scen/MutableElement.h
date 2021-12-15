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
#include "discenfw/scen/AssetReference.h"

#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
#     pragma warning(disable : 4275) // disable warning C4275: non dll-interface class ... used as base for dll-interface class
#   endif

namespace discenfw
{
	/*!
	Element with a mutable representation.
	*/
	struct MutableElement : public Element
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		//! List of asset references to alternative representations.
		std::vector<AssetReference> Representations;

		unsigned RepresentationIndex = 0;

		MutableElement();

		/// @name Constructors.
		///@{

		MutableElement(const std::string& id, const std::string& entityType);


		MutableElement(const std::string& id, const std::string category,
			const LocalTransform& transform, const std::vector<AssetReference>& repr, unsigned reprIndex = 0);

		///@}

		virtual ~MutableElement() {}

	public:

		virtual const char* GetClassName() const override
		{
			return "MutableElement";
		}

		virtual bool IsA(const std::string& classType) const override
		{
			return classType == MutableElement::GetClassName() || Element::IsA(classType);
		}

		int GetRepresentationIndex() const
		{
			return RepresentationIndex;
		}

		const std::vector<AssetReference>& GetRepresentations() const
		{
			return Representations;
		}
	};
}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
