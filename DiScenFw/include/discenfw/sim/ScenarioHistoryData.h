//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/util/IBaseClass.h>
#include <discenfw/util/DateTimeUtil.h>
#include <discenfw/scen/ScenarioData.h>

#include <memory>
#include <vector>
#include <map>
#include <ctime>


namespace discenfw
{
	namespace sim
	{
		/*!
		State of an entity in an interval of time.
		*/
		struct TemporalState : public IBaseClass
		{
			/*!
			Start time (interval definition).
			*/
			DateTime StartDateTime;

			/*!
			End time (interval definition).
			*/
			DateTime EndDateTime;

			/*!
			Data origin.
			*/
			std::string Origin;

			virtual const char* GetClassName() const override { return "TemporalState"; }
			virtual bool IsA(const std::string& classType) const override
			{
				return classType == TemporalState::GetClassName();
			}
		};


		/*!
		State of an element in an interval of time, including spatial information and representation.
		*/
		struct ElementState : public TemporalState
		{
			/*!
			Transformation at the beginning of the interval.
			*/
			LocalTransform Transform;

			/*!
			Representation to be used in the interval.
			*/
			std::string Representation;

			virtual const char* GetClassName() const override { return "ElementState"; }
			virtual bool IsA(const std::string& classType) const override
			{
				return classType == ElementState::GetClassName() || TemporalState::IsA(classType);
			}
		};


		/*!
		Event occurred at a given time.
		*/
		struct HistoryEvent
		{
			std::string Uri;
			std::string Category;
			DateTime DateTime;
		};


		/*!
		History of states for an entity (the interval of time is built to include all the entity states).
		*/
		struct EntityHistory
		{
			std::string LogUri;

			DateTime StartDateTime;
			DateTime EndDateTime;

			std::vector< std::shared_ptr<TemporalState> > States;
		};


		/*!
		History of states of all the entities (the interval of time is built to include all the entity states).
		*/
		struct ScenarioHistory
		{
			std::string Uri;
			std::string Name;
			std::string Description;
			std::string Details;

			DateTime StartDateTime;
			DateTime EndDateTime;

			std::map< std::string, std::shared_ptr<EntityHistory> > EntityHistories;
			std::vector< std::shared_ptr<HistoryEvent> > Events;
		};


		/*!
		Createa shared pointer fo the proper type given the entity state class name.
		*/
		inline std::shared_ptr<TemporalState> MakeEntityState(const std::string& stateClass)
		{
			if (stateClass == "TemporalState") return std::make_shared<TemporalState>();
			if (stateClass == "ElementState") return std::make_shared<ElementState>();
			return nullptr;
		}

	}
}

