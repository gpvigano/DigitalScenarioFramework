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
#include "discenfw/scen/SocketInfo.h"

#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
#     pragma warning(disable : 4275) // disable warning C4275: non dll-interface class ... used as base for dll-interface class
#   endif

namespace discenfw
{
	/*!
	Basic element with a local 3D transformation.
	@see ConnectionElement
	*/
	class DISCENFW_API Element : public Entity
	{
	// Prevent public access from DLL clients
	DISCENFW_DLL_PROTECTED

		//! Local transfomation for this element
		LocalTransform Transform;

		/// @name Constructors.
		///@{

		Element();

		Element(const std::string& id, const std::string& entityType);

		Element(const std::string& id, const std::string& entityType, const AssetReference& assetRef);

		Element(const std::string& id, const std::string& entityType, const LocalTransform& transform);

		Element(const std::string& id, const std::string& entityType, const LocalTransform& transform, const AssetReference& assetRef);

		Element(
			const Entity& entityData,
			const LocalTransform& transform,
			const std::vector<SocketInfo>& sockets
			);

		///@}

		virtual ~Element() {}

	public:

		virtual const char* GetClassName() const override
		{
			return "Element";
		}

		virtual bool IsA(const std::string& classType) const override
		{
			return classType == Element::GetClassName() || Entity::IsA(classType);
		}

		//! Allow read-only access to the Transform data.
		const LocalTransform& GetTransform() const
		{
			return Transform;
		}

		//! Allow read/write access to the Transform3D data only.
		Transform3D& GetTransform3D()
		{
			return Transform;
		}

		//! Set the Transform3D data only.
		void SetTransform3D(const Transform3D& transform);

		//! Get the transform parent element identifier.
		const std::string& GetParentId() const
		{
			return Transform.ParentId;
		}

		//! Set the transform parent element identifier.
		void SetParentId(const std::string& parentId);


		/*!
		Get the sockets used for connections.
		*/
		const std::vector<SocketInfo>& GetSockets() const
		{
			return Sockets;
		}

		/*!
		Get the named socket used for connections.
		*/
		const SocketInfo& GetSocket(const std::string& id) const;

		/*!
		Clear all the sockets used for connections.
		*/
		void ClearSockets();

		/*!
		Configure the socket with the id included in the given data, create it if not found.
		*/
		bool SetSocket(const SocketInfo& socketInfo);

	protected:

		/*!
		Sockets used for connections.
		*/
		std::vector<SocketInfo> Sockets;
	};
}

#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif
