//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/ConnectionElement.h>

#include <algorithm>


namespace discenfw
{
	ConnectionElement::ConnectionElement()
	{
	}


	ConnectionElement::ConnectionElement(
		const std::string& id,
		const std::string& entityType
		) :
		Element(id, entityType),
		ConnectionPath()
	{
	}


	ConnectionElement::ConnectionElement(
		const std::string& id,
		const std::string& entityType,
		const std::vector<Anchor>& path
		) :
		Element(id, entityType),
		ConnectionPath(path)
	{
	}


	ConnectionElement::ConnectionElement(
		const std::string& id,
		const std::string& entityType,
		const LocalTransform& transform,
		const std::vector<Anchor>& path
		) :
		Element(id, entityType, transform),
		ConnectionPath(path)
	{
	}


	ConnectionElement::ConnectionElement(
		const std::string& id,
		const std::string& entityType,
		const std::vector<Anchor>& path,
		const AssetReference& assetRef
		) :
		Element(id, entityType, assetRef),
		ConnectionPath(path)
	{
	}


	ConnectionElement::ConnectionElement(
		const std::string& id,
		const std::string& entityType,
		const LocalTransform& transform,
		const std::vector<Anchor>& path,
		const AssetReference& assetRef
		) :
		Element(id, entityType, transform, assetRef),
		ConnectionPath(path)
	{
	}


	ConnectionElement::ConnectionElement(
		const Element& elementData,
		const std::vector<Vector3D>& anchorPoints
		) :
		Element(elementData)
	{
		for (size_t i = 0; i< anchorPoints.size(); i++)
		{
			AddAnchor({ anchorPoints[i] });
		}
	}

	void ConnectionElement::SetConnectionPath(const std::vector<Anchor>& connectionPath)
	{
		ConnectionPath = connectionPath;
	}


	const Anchor& ConnectionElement::GetAnchor(size_t index) const
	{
		if (index<ConnectionPath.size())
		{
			return ConnectionPath.at(index);
		}
		static Anchor undefined;
		return undefined;
	}

	void ConnectionElement::AddAnchor(const Anchor& anchor)
	{
		ConnectionPath.push_back(anchor);
	}

	void ConnectionElement::SetAnchor(size_t index, const Anchor& anchor)
	{
		if (index<ConnectionPath.size())
		{
			ConnectionPath[index]=anchor;
		}
	}

	void ConnectionElement::ClearConnectionPath()
	{
		ConnectionPath.clear();
	}
}
