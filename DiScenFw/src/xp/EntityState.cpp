//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "discenfw/xp/EntityState.h"
#include <discenfw/xp/EnvironmentModel.h>

namespace discenfw
{
	namespace xp
	{
		const std::map<std::string, std::string>& EntityState::GetPropertyValues() const
		{
			return PropertyValues;
		}
		void EntityState::SetPropertyValue(const std::string& propertyName, const std::string& value)
		{
			PropertyValues[propertyName] = value;
		}

		const std::string& EntityState::GetPropertyValue(const std::string& propertyName) const
		{
			return PropertyValues.at(propertyName);
		}

		void EntityState::ClearPropertyValues()
		{
			PropertyValues.clear();
		}


		const std::map<std::string, RelationshipLink>& EntityState::GetRelationships() const
		{
			return Relationships;
		}

		void EntityState::SetRelationship(const std::string& entityId, const RelationshipLink& link)
		{
			Relationships[entityId] = link;
		}

		const RelationshipLink& EntityState::GetRelationship(const std::string& entityId) const
		{
			return Relationships.at(entityId);
		}

		void EntityState::RemoveRelationship(const std::string& entityId)
		{
			if (Relationships.find(entityId) != Relationships.end())
			{
				Relationships.erase(entityId);
			}
		}

		void EntityState::ClearRelationships()
		{
			Relationships.clear();
		}


		EntityState::EntityState()
		{
		}


		EntityState::~EntityState()
		{
		}


		std::shared_ptr<EntityState> EntityState::Make(const std::string& typeName, const std::string& modelName)
		{
			return std::make_shared<EntityState>(typeName, modelName);
		}


		/*!
		Construct an entity state with the given entity state type.
		*/
		EntityState::EntityState(const std::string& typeName, const std::string& modelName)
			: ModelName(modelName), TypeName(typeName)
		{
		}


		EntityState::EntityState(const std::string& typeName,
			const std::map<std::string, std::string>& propertyValues,
			const std::string& modelName)
		{
			SetType(typeName,modelName);
			PropertyValues.insert(propertyValues.cbegin(), propertyValues.cend());
		}


		EntityState::EntityState(const std::string& typeName,
			const std::map<std::string, std::string>& propertyValues,
			const std::map<std::string, RelationshipLink>& relationships,
			const std::string& modelName
			)
		{
			SetType(typeName,modelName);
			PropertyValues.insert(propertyValues.cbegin(), propertyValues.cend());
			Relationships.insert(relationships.cbegin(), relationships.cend());
		}


		void EntityState::SetDefaultValues()
		{
			if (Type)
			{
				Type->GetDefaultValues(PropertyValues);
			}
			else
			{
				PropertyValues.clear();
			}
			Relationships.clear();
		}


		std::shared_ptr<EntityStateType> EntityState::GetType() const
		{
			if (!Type)
			{
				Type = xp::GetModel(ModelName)->GetEntityStateType(TypeName);
			}
			return Type;
		}


		bool EntityState::SetType(const std::string& typeName, const std::string& modelName)
		{
			TypeName = typeName;
			ModelName = modelName;
			SetDefaultValues();
			return Type != nullptr;
		}


		EntityState& EntityState::operator =(const EntityState& entityState)
		{
			if (this == &entityState)
			{
				return *this;
			}
			SetType(entityState.GetTypeName(), entityState.GetModelName());
			PropertyValues = entityState.PropertyValues;
			Relationships = entityState.Relationships;
			return *this;
		}


		std::shared_ptr<EntityState> EntityState::Clone() const
		{
			std::shared_ptr<EntityState> entityState = std::make_shared<EntityState>(TypeName, ModelName);
			*entityState = *this;
			return entityState;
		}

	}
}
