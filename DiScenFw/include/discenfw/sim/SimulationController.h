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
#include "discenfw/sim/SimulationExecutor.h"
#include <discenfw/ve/VirtualEnvironmentAPI.h>

#include <memory>
#include <map>
#include <functional>

namespace discenfw
{
	namespace sim
	{
		/*!
		Controller for simulation playback.
		*/
		class DISCENFW_API SimulationController
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED
			/*!
			Shared pointer to Scenario data.
			*/
			std::shared_ptr<Scenario> Scenario;

			/*!
			Simulation history data.
			*/
			std::shared_ptr<ScenarioHistory> SimulationHistory;

			/*!
			A void callback function used to trigger events.
			*/
			using EventAction = std::function<void()>;

			/*!
			Event triggered after the simulation has been loaded.
			*/
			EventAction SimulationLoaded;

			/*!
			Event triggered each time the simulation has been updated.
			*/
			EventAction SimulationUpdated;

			/*!
			Event triggered before playing the simulation.
			*/
			EventAction SimulationPlay;

			/*!
			Event triggered before pausing the simulation.
			*/
			EventAction SimulationPause;

			/*!
			Event triggered after the simulation has been stopped.
			*/
			EventAction SimulationStop;

			/*!
			Event triggered after the simulation time is changed.
			*/
			std::function<void(float)> SimulationTimeChanged;

			/*!
			Default constructor: create an empty simulation history.
			*/
			SimulationController();

			~SimulationController();

		public:

			/*!
			Check if a valid not empty simulation is defined.
			*/
			bool ValidSimulation()
			{
				return SimulationHistory && !SimulationHistory->EntityHistories.empty();
			}

			/*!
			Get the total simulation duration in seconds.
			*/
			double GetSimulationDuration() { return SimulationDuration; }

			/*!
			Get the current simulation date and time.
			*/
			DateTime GetSimulationDateTime() { return SimulationDateTime; }

			/*!
			Get the current simulation date and time encoded to a string.
			*/
			const char* GetSimulationDateTimeString();

			/*!
			This is true if the simulation was started.
			*/
			bool SimulationStarted() { return IsSimulationStarted; }

			/*!
			This is true if the simulation was paused.
			*/
			bool SimulationPaused() { return IsSimulationPaused; }

			/*!
			Calculate the simulation progress.
			@return Progress between 0 (start) and 1 (end).
			*/
			double ComputeSimulationProgress();

			/*!
			Force the simulation progress to the given percentage (0..1).
			@param progress Progress between 0 (start) and 1 (end).
			*/
			void SetSimulationProgress(double progress);

			//! Simulation time speed scale (1=normal, 0=frozen, negative=reverse).
			double GetSimulationTimeSpeed() { return SimulationTimeSpeed; }

			/*!
			Change the time scale to control the speed of the simulation.
			@param scale Time scale, multiplied by the real time.
			@note Negative values make the time move backwards.
			*/
			void SetSimulationTimeSpeed(double scale);

			/*!
			Current simulation time (automatically computed if playing).
			*/
			double GetSimulationTime() { return SimulationTime; }

			/*!
			Force the simulation time to the given value (seconds since the beginning).
			@param  time Simulation time in seconds since the beginning.
			*/
			void SetSimulationTime(double time);

			/*!
			Increase the simulation tim by 5%.
			*/
			void IncSimulationTime();

			/*!
			Decrease the simulation tim by 5%.
			*/
			void DecSimulationTime();


			/*!
			Reset the simulation to the start time.
			*/
			void ResetSimulationTime();


			/*!
			Forward the simulation to its end time.
			*/
			void CompleteSimulationTime();

			/*!
			Prepare the simulation loading each entity history and computing the overall data.
			*/
			bool InitSimulation();

			/*!
			Prepare the simulation loading each entity history and computing the overall data.
			*/
			bool SimulationDefined();


			/*!
			Play the simulation or resume it if paused.
			*/
			void PlaySimulation();


			/*!
			Reset and stop the simulation.
			*/
			void StopSimulation();


			/*!
			Pause the simulation (if started).
			*/
			void PauseSimulation();


			/*!
			Update the simulation time based on the elapsed time and update the simulation.
			*/
			void UpdateSimulation();


			/*!
			Update internal time and call UpdateSimulation() if simulation is running.
			*/
			void Update(double runTime);


		private:

			double SimulationTimeSpeed = 1.0f;
			double SimulationTime = 0;
			DateTime SimulationDateTime;
			double RunTime = 0.0;

			double SimulationStartTime = 0;
			double SimulationPauseTime = 0;
			double SimulationUpdateTime = -1.0f;
			double SimulationDuration = 0;
			bool IsSimulationStarted = false;
			bool IsSimulationPaused = false;
			std::map< std::string, std::shared_ptr<SimulationExecutor> > Executors;
		};

	}
}

