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
#include "discenfw/xp/EntityStateType.h"
#include "discenfw/xp/RelationshipLink.h"

#include <string>
#include <map>
#include <set>


namespace discenfw
{
	namespace xp
	{
		/*!
		State of a generic entity, with properties and relationships.
		*/
		class DISCENFW_API EntityState
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED

			/*!
			Generic properties represented as strings.
			*/
			std::map<std::string, std::string> PropertyValues;

			/*!
			Generic relationships represented by entity identifiers and relationship identifiers.
			*/
			std::map<std::string, RelationshipLink> Relationships;


			/*!
			Default empty constructor.
			*/
			EntityState();

			/*!
			Construct an entity state with the given entity state type in the given module (the last defined if omitted).
			*/
			EntityState(const std::string& typeName, const std::string& modelName = "");

			/*!
			Construct an entity state with the given entity state type and a set of properties.
			*/
			EntityState(const std::string& typeName,
				const std::map<std::string, std::string>& propertyValues,
				const std::string& modelName = "");

			/*!
			Construct an entity state with the given entity state type,
			a set of properties and a set of relationships.
			*/
			EntityState(const std::string& typeName,
				const std::map<std::string, std::string>& propertyValues,
				const std::map<std::string, RelationshipLink>& relationships,
				const std::string& modelName = ""
				);


			/*!
			Destructor.
			*/
			~EntityState();

		public:

			/*!
			Make a new entity state type (allocated in the library module), return a shared pointer to it.
			*/
			static std::shared_ptr<EntityState> Make(const std::string& typeName, const std::string& modelName);

			///@name PropertyValues
			///@{

			const std::map<std::string, std::string>& GetPropertyValues() const;

			void SetPropertyValue(const std::string& propertyName, const std::string& value);

			const std::string& GetPropertyValue(const std::string& propertyName) const;

			void ClearPropertyValues();

			///@}

			///@name Relationships
			///@{

			const std::map<std::string, RelationshipLink>& GetRelationships() const;

			void SetRelationship(const std::string& entityId, const RelationshipLink& link);

			const RelationshipLink& GetRelationship(const std::string& entityId) const;

			void RemoveRelationship(const std::string& entityId);

			void ClearRelationships();

			///@}


			/*!
			Set properties to their default values.
			*/
			void SetDefaultValues();


			/*!
			Get the type name.
			*/
			const std::string& GetModelName() const
			{
				return ModelName;
			}


			/*!
			Get the type name.
			*/
			const std::string& GetTypeName() const
			{
				return TypeName;
			}


			/*!
			Get the entity state type name.
			*/
			std::shared_ptr<EntityStateType> GetType() const;


			/*!
			Check if the entity has the given type name.
			*/
			bool IsOfType(const std::string& typeName) const
			{
				return GetType() ? GetType()->IsA(typeName) : typeName.empty();
			}


			/*!
			Get the entity state type to the one with the given name (only if it exists).
			*/
			bool SetType(const std::string& typeName, const std::string& modelName = "");


			/*!
			Check if the related entity has the given property set.
			*/
			bool HasProperty(const std::string& propName) const
			{
				return (PropertyValues.find(propName) != PropertyValues.cend());
			}

			/*!
			Check if the related entity has the given property set to the given value.
			*/
			bool HasPropertySet(const std::string& propName, const std::string& propValue) const
			{
				if (!HasProperty(propName))
				{
					return false;
				}
				return PropertyValues.at(propName) == propValue;
			}


			bool operator ==(const EntityState& entityState) const
			{
				return entityState.PropertyValues == PropertyValues
					&& entityState.Relationships == Relationships;
			}


			EntityState& operator =(const EntityState& entityState);

			/*!
			Compare the given referenced entity state to this one.
			*/
			bool EqualTo(const std::shared_ptr<EntityState>& entityState) const
			{
				return *this == *entityState;
			}

			/*!
			Clone this instance and return a shared pointer to it.
			*/
			std::shared_ptr<EntityState> Clone() const;

		protected:

			mutable std::shared_ptr<EntityStateType> Type;

			std::string ModelName;

			std::string TypeName;
		};

	}
}

