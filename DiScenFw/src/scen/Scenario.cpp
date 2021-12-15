//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/Scenario.h>

#include <algorithm>


namespace discenfw
{
	Scenario::Scenario()
	{
	}


	Scenario::Scenario(const std::string& name)
		: Name(name)
	{
	}


	std::shared_ptr<Entity> Scenario::GetEntityById(const std::string& id)
	{
		// TODO: to be optimized e.g. using maps

		auto entItr = std::find_if(Entities.begin(), Entities.end(),
			[&id](const std::shared_ptr<Entity>& entity)
		{
			return entity->GetIdentifier() == id;
		});
		if (entItr == Entities.end())
		{
			return nullptr;
		}
		return *entItr;
	}


	std::shared_ptr<Element> Scenario::GetElementById(const std::string& id)
	{
		std::shared_ptr<Entity> entity = GetEntityById(id);
		if (entity && entity->IsA("Element"))
		{
			std::shared_ptr<Element> elem = std::static_pointer_cast<Element>(entity);
			return elem;
		}
		return nullptr;
	}


	void Scenario::AddEntity(std::shared_ptr<Entity> entity)
	{
		if (entity)
		{
			Entities.push_back(entity);
		}
	}


	bool Scenario::RemoveEntity(std::shared_ptr<Entity> entity)
	{
		auto entityIt = std::find(Entities.begin(), Entities.end(), entity);
		if (entityIt != Entities.end())
		{
			Entities.erase(entityIt);
			return true;
		}
		return false;
	}


	void Scenario::ClearEntities()
	{
		Entities.clear();
	}
}
