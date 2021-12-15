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
#include "discenfw/scen/Anchor.h"

#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
#     pragma warning(disable : 4275) // disable warning C4275: non dll-interface class ... used as base for dll-interface class
#   endif

namespace discenfw
{
	/*!
	Element connecting other elements through their sockets, following a defined path.
	@remark Anchors can be attached also to the sockets of this elements.
	*/
	class DISCENFW_API ConnectionElement : public Element
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		/// @name Constructors.
		///@{

		ConnectionElement();

		ConnectionElement(const std::string& id, const std::string& entityType);

		ConnectionElement(const std::string& id, const std::string& entityType, const std::vector<Anchor>& path);

		ConnectionElement(const std::string& id, const std::string& entityType, const LocalTransform& transform, const std::vector<Anchor>& path);

		ConnectionElement(const std::string& id, const std::string& entityType, const std::vector<Anchor>& path, const AssetReference& assetRef);

		ConnectionElement(const std::string& id, const std::string& entityType, const LocalTransform& transform, const std::vector<Anchor>& path, const AssetReference& assetRef);

		ConnectionElement(const Element& elementData, const std::vector<Vector3D>& anchorPoints);

		///@}

		virtual ~ConnectionElement() {}

	public:

		virtual const char* GetClassName() const override
		{
			return "ConnectionElement";
		}

		virtual bool IsA(const std::string& classType) const override
		{
			return classType == ConnectionElement::GetClassName() || Element::IsA(classType);
		}

		//! Get the list of points to witch the connection path is anchored.
		const std::vector<Anchor>& GetConnectionPath() const
		{
			return ConnectionPath;
		}

		//! Set the list of points to witch the connection path is anchored.
		void SetConnectionPath(const std::vector<Anchor>& connectionPath);

		//! Get the number of points to witch the connection path is anchored.
		const size_t GetNumAnchors() const
		{
			return ConnectionPath.size();
		}

		//! Get the number of points to witch the connection path is anchored.
		bool IsConnectionPathEmpty() const
		{
			return ConnectionPath.empty();
		}

		//! Get a point to witch the connection path is anchored.
		const Anchor& GetAnchor(size_t index) const;

		//! Add a new point to witch the connection path is anchored.
		void AddAnchor(const Anchor& anchor);

		//! Set a point to witch the connection path is anchored.
		void SetAnchor(size_t index, const Anchor& anchor);

		//! Clear the list of points to witch the connection path is anchored.
		void ClearConnectionPath();

	protected:

		//! List of points to witch the connection path is anchored.
		std::vector<Anchor> ConnectionPath;
	};
}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
