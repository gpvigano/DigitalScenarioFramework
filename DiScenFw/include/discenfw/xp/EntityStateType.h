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
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>


namespace discenfw
{
	namespace xp
	{
		/*!
		Entity state type, used to define default values for the properties of a specific type of entity state.
		@note Use EnvironmentModel methods to create instances of EntityStateType.
		*/
		class DISCENFW_API EntityStateType
		{
		public:

			~EntityStateType();


			/*!
			Create a new entity state type with the given name,
			the given default property values.
			*/
			EntityStateType(
				const std::string& typeName,
				std::map<std::string, std::string> defaultPropertyValues,
				const std::string& modelName = ""
				);

			/*!
			Create a new entity state type, inheriting from the given entity state type,
			with the given name, the given default property values, the given possible property values
			and the given links.
			*/
			EntityStateType(
				const std::string& typeName,
				const std::map<std::string, std::string>& defaultPropertyValues,
				const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
				const std::vector<std::string>& links,
				const std::string& modelName = ""
				);

			/*!
			Create a new entity state type, inheriting from the given entity state type,
			with the given name, the given default property values, the given possible property values
			and the given links.
			*/
			EntityStateType(
				const std::string& parentTypeName,
				const std::string& typeName,
				const std::map<std::string, std::string>& defaultPropertyValues,
				const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
				const std::vector<std::string>& links,
				const std::string& modelName = ""
				);


			/*!
			Get the given default property values.
			*/
			void GetDefaultValues(std::map<std::string, std::string>& propertyValues) const;


			/*!
			Set the given default property values.
			*/
			void SetDefaultValues(const std::map<std::string, std::string>& propertyValues);


			/*!
			Get the given possible property values.
			*/
			void GetPossiblePropertyValues(std::map<std::string, std::vector<std::string> >& propertyValues) const;


			/*!
			Set the given possible property values.
			*/
			void SetPossiblePropertyValues(const std::map<std::string, std::vector<std::string> >& propertyValues);


			/*!
			Check if this entity type has the given default property values.
			*/
			bool HasDefaultValues(const std::map<std::string, std::string>& propertyValues) const;


			/*!
			Check if this entity type has the given possible property values.
			*/
			bool HasPossiblePropertyValues(const std::map<std::string, std::vector<std::string> >& propertyValues) const;


			/*!
			Check if this entity type has possible property values defined.
			*/
			bool HasPossiblePropertyValuesDefined()
			{
				return !PossiblePropertyValues.empty();
			}


			/*!
			Get the entity state type model name.
			*/
			const std::string& GetModelName() const { return ModelName; }


			/*!
			Get the entity state type name.
			*/
			const std::string& GetTypeName() const { return TypeName; }

			/*!
			Get the name of the parent entity state type.
			*/
			const std::string& GetParentTypeName() const { return ParentTypeName; }

			/*!
			Check if this entity state type derive from the named parent entity state type.
			*/
			bool DerivesFrom(const std::string& parentTypeName) const;
			/*!
			Check if this entity state type is or derive from the named parent entity state type.
			*/
			bool IsA(const std::string& typeName) const;

			/*!
			Get the number of possible properties for this EntityStateType.
			*/
			int CountPossibleProperties() const;

			/*!
			Get the possible properties for this EntityStateType.
			*/
			void GetPossibleProperties(std::vector<std::string>& propertyIds) const;

			/*!
			Get the number of possible values for a property of this EntityStateType.
			*/
			int CountPossiblePropertyValues(const std::string& propertyId) const;

			/*!
			Get the possible values for a property of this EntityStateType.
			*/
			bool GetPossiblePropertyValues(
				const std::string& propertyId,
				std::vector<std::string>& values) const;

			/*!
			Get the possible properties values of this EntityStateType.
			*/
			const std::map<std::string, std::vector<std::string>>& GetPossiblePropertyValues() const
			{
				return PossiblePropertyValues;
			}

			/*!
			Get the default properties values of this EntityStateType.
			*/
			const std::map<std::string, std::string>& GetDefaultPropertyValues() const
			{
				return DefaultPropertyValues;
			}

			/*!
			Check if this entity type has the given links.
			*/
			bool HasLinks(const std::vector<std::string>& links) const;

			/*!
			Get the link identifiers of this EntityStateType.
			*/
			const std::vector<std::string>& GetLinks() const
			{
				return Links;
			}

			/*!
			Set the link identifiers of this EntityStateType.
			*/
			void SetLinks(const std::vector<std::string>& links);

		private:

			//static std::map< std::string, std::shared_ptr<EntityStateType> >
			//	TypeMap;

			std::string TypeName = "undefined";
			std::string ModelName = "";
			std::string ParentTypeName;

			std::map<std::string, std::string> DefaultPropertyValues;
			std::map<std::string, std::vector<std::string>> PossiblePropertyValues;
			std::vector<std::string> Links;
		};
	}
}

