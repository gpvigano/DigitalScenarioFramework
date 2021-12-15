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
	Group of elements.
	*/
	struct GroupElement : public Element
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		/*!
		List of references to elements that compose the group.
		*/
		std::vector< std::shared_ptr<Element> > Parts;

		/// @name Constructors.
		///@{

		GroupElement();

		GroupElement(const std::string& id, const std::string& entityType);

		GroupElement(const std::string& id, const std::string& entityType,
			const LocalTransform& transform, const std::vector< std::shared_ptr<Element> >& parts);

		///@}

		virtual ~GroupElement() {}

	public:

		virtual const char* GetClassName() const override
		{
			return "GroupElement";
		}

		virtual bool IsA(const std::string& classType) const override
		{
			return classType == GroupElement::GetClassName() || Entity::IsA(classType);
		}

		const std::vector< std::shared_ptr<Element> >& GetParts() const
		{
			return Parts;
		}

		void SetParts(const std::vector< std::shared_ptr<Element> >& parts);

		void AddPart(std::shared_ptr<Element> part);

		void RemovePart(std::shared_ptr<Element> part);
	};
}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
