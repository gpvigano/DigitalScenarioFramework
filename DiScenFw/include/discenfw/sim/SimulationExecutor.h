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
#include "discenfw/sim/ScenarioHistoryData.h"
#include <discenfw/scen/ScenarioData.h>
#include <discenfw/util/DateTimeUtil.h>
#include <ctime>
#include <memory>
#include <map>

namespace discenfw
{
	namespace sim
	{
		/*!
		Update a target element according to its simulated evolution.
		*/
		class DISCENFW_API SimulationExecutor
		{
		public:

			/*!
			Simulation history for the target element.
			*/
			std::shared_ptr<EntityHistory> History;

			/*!
			Controlled target element.
			*/
			std::shared_ptr<Element> TargetElement;

			/*!
			Default empty constructor.
			*/
			SimulationExecutor()
			{}

			~SimulationExecutor()
			{}

			/*!
			Call UpdateTarget(dateTime) if the simulation is running.
			*/
			void Update(const DateTime& dateTime);

			/*!
			Update the target element according to its simulated evolution at the given time.
			*/
			bool UpdateTarget(const DateTime& dateTime);

			/*!
			Check if the simulation is running.
			*/
			bool Running() { return IsRunning; }

			/*!
			Ensure the simulation is running.
			*/
			void Run() { IsRunning = true; }

			/*!
			Stop the simulation running.
			*/
			void Stop() { IsRunning = false; }

		private:

			bool IsRunning = false;

			std::shared_ptr<ElementState> GetElementStateAt(unsigned index);
			std::shared_ptr<ElementState> FindStateAt(const DateTime& time, std::shared_ptr<ElementState>& prevState, std::shared_ptr<ElementState>& nextState);
		};

	}
}
