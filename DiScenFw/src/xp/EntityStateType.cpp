//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/EntityStateType.h>
#include <discenfw/xp/EnvironmentModel.h>

#include <discenfw/util/MessageLog.h>


namespace discenfw
{
	namespace xp
	{

		EntityStateType::~EntityStateType()
		{
		}


		EntityStateType::EntityStateType(
			const std::string& typeName,
			std::map<std::string, std::string> defaultPropertyValues,
				const std::string& modelName
			) :
			TypeName(typeName),
			DefaultPropertyValues(defaultPropertyValues),
			ModelName(modelName)
		{
		}

		EntityStateType::EntityStateType(
			const std::string& typeName,
			const std::map<std::string, std::string>& defaultPropertyValues,
			const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
			const std::vector<std::string>& links,
				const std::string& modelName
			) :
			TypeName(typeName),
			DefaultPropertyValues(defaultPropertyValues),
			PossiblePropertyValues(possiblePropertyValues),
			Links(links),
			ModelName(modelName)
		{
		}

		EntityStateType::EntityStateType(
			const std::string& parentTypeName,
			const std::string& typeName,
			const std::map<std::string, std::string>& defaultPropertyValues,
			const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
			const std::vector<std::string>& links,
				const std::string& modelName
			) :
			ParentTypeName(parentTypeName),
			TypeName(typeName),
			DefaultPropertyValues(defaultPropertyValues),
			PossiblePropertyValues(possiblePropertyValues),
			Links(links),
			ModelName(modelName)
		{
		}


		void EntityStateType::GetDefaultValues(std::map<std::string, std::string>& propertyValues) const
		{
			propertyValues = DefaultPropertyValues;
		}

		void EntityStateType::SetDefaultValues(const std::map<std::string, std::string>& propertyValues)
		{
			DefaultPropertyValues = propertyValues;
		}


		void EntityStateType::GetPossiblePropertyValues(std::map<std::string, std::vector<std::string>>& propertyValues) const
		{
			propertyValues = PossiblePropertyValues;
		}

		void EntityStateType::SetPossiblePropertyValues(const std::map<std::string, std::vector<std::string>>& propertyValues)
		{
			PossiblePropertyValues = propertyValues;
		}


		bool EntityStateType::HasDefaultValues(const std::map<std::string, std::string>& propertyValues) const
		{
			return propertyValues == DefaultPropertyValues;
		}


		bool EntityStateType::HasPossiblePropertyValues(const std::map<std::string, std::vector<std::string>>& propertyValues) const
		{
			return propertyValues == PossiblePropertyValues;
		}


		bool EntityStateType::HasLinks(const std::vector<std::string>& links) const
		{
			return Links == links;
		}

		/*!
		Set the link identifiers of this EntityStateType.
		*/

		void EntityStateType::SetLinks(const std::vector<std::string>& links)
		{
			Links = links;
		}


		bool EntityStateType::DerivesFrom(const std::string& parentTypeName) const
		{
			if (ParentTypeName.empty())
			{
				return false;
			}
			if (ParentTypeName == parentTypeName)
			{
				return true;
			}
			if (TypeName == parentTypeName)
			{
				return false;
			}

			std::shared_ptr<EntityStateType> parentType = xp::GetModel(ModelName)->GetEntityStateType(parentTypeName);
			if (!parentType)
			{
				return false;
			}
			if (parentType->DerivesFrom(parentTypeName))
			{
				return true;
			}

			return false;
		}


		bool EntityStateType::IsA(const std::string& typeName) const
		{
			if (TypeName == typeName)
			{
				return true;
			}
			return DerivesFrom(typeName);
		}


		int EntityStateType::CountPossibleProperties() const
		{
			return (int)DefaultPropertyValues.size();
		}


		void EntityStateType::GetPossibleProperties(std::vector<std::string>& propertyIds) const
		{
			propertyIds.clear();
			for (const auto& item : DefaultPropertyValues)
			{
				propertyIds.push_back(item.first);
			}
		}


		int EntityStateType::CountPossiblePropertyValues(const std::string& propertyId) const
		{
			const auto& it = PossiblePropertyValues.find(propertyId);
			if (it == PossiblePropertyValues.cend())
			{
				return 0;
			}
			int count = (int)it->second.size();
			return count;
		}


		bool EntityStateType::GetPossiblePropertyValues(
			const std::string& propertyId,
			std::vector<std::string>& values) const
		{
			const auto& it = PossiblePropertyValues.find(propertyId);
			if (it == PossiblePropertyValues.cend())
			{
				return false;
			}
			values = it->second;
			return true;
		}


	} // namespace xp
} // namespace discenfw
