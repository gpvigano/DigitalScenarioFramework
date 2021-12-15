//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/ScenarioData.h>

#include <algorithm>


namespace discenfw
{
	Element::Element()
	{
	}


	Element::Element(const std::string& id, const std::string& entityType) :
		Entity(id, entityType)
	{
	}


	Element::Element(
		const std::string& id,
		const std::string& entityType,
		const AssetReference& assetRef
		) :
		Entity(id, entityType, assetRef)
	{
	}


	Element::Element(
		const std::string& id,
		const std::string& entityType,
		const LocalTransform& transform
		) :
		Entity(id, entityType),
		Transform(transform)
	{
	}


	Element::Element(
		const std::string& id,
		const std::string& entityType,
		const LocalTransform& transform,
		const AssetReference& assetRef
		) :
		Entity(id, entityType, assetRef),
		Transform(transform)
	{
	}


	Element::Element(
		const Entity& entityData,
		const LocalTransform& transform,
		const std::vector<SocketInfo>& sockets
		) :
		Entity(entityData),
		Transform(transform),
		Sockets(sockets)
	{
	}


	void Element::SetTransform3D(const Transform3D& transform)
	{
		(Transform3D&)Transform = transform;
	}


	void Element::SetParentId(const std::string& parentId)
	{
		Transform.ParentId = parentId;
	}


	const SocketInfo& Element::GetSocket(const std::string& id) const
	{
		const auto& socketItr = std::find_if(Sockets.begin(), Sockets.end(),
			[&id](const SocketInfo& info) { return info.Id == id; });
		if (socketItr != Sockets.end())
		{
			return *socketItr;
		}
		static SocketInfo invalidSocket;
		return invalidSocket;
	}


	void Element::ClearSockets()
	{
		Sockets.clear();
	}


	bool Element::SetSocket(const SocketInfo& socketInfo)
	{
		if (socketInfo.Id.empty())
		{
			return false;
		}
		const auto& socketItr = std::find_if(Sockets.begin(), Sockets.end(),
			[&socketInfo](const SocketInfo& info) { return info.Id == socketInfo.Id; });
		if (socketItr != Sockets.end())
		{
			*socketItr = socketInfo;
			return true;
		}
		Sockets.push_back(socketInfo);
		return true;
	}
}
