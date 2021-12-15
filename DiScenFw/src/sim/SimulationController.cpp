//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/sim/SimulationController.h>

#include <discenfw/util/MessageLog.h>
#include <gpvulc/time/DateTimeUtil.h>


namespace discenfw
{
	namespace sim
	{
		SimulationController::SimulationController()
		{
			SimulationHistory = std::make_shared<ScenarioHistory>();
		}


		SimulationController::~SimulationController()
		{
		}


		const char* SimulationController::GetSimulationDateTimeString()
		{
			return gpvulc::DateTimeToCString(SimulationDateTime);
		}


		double SimulationController::ComputeSimulationProgress()
		{
			if (IsSimulationStarted && SimulationDuration > 0.0)
			{
				double	progress = SimulationTime / SimulationDuration;
				return progress;
			}
			return 0.0;
		}


		void SimulationController::SetSimulationProgress(double progress)
		{
			if (IsSimulationStarted)// && IsSimulationPaused)
			{
				SimulationTime = progress * SimulationDuration;
				SimulationStartTime = SimulationPauseTime - SimulationTime;
				UpdateSimulation();
			}
		}


		void SimulationController::SetSimulationTimeSpeed(double scale)
		{
			if (SimulationTimeSpeed != scale)
			{
				SimulationTimeSpeed = scale;
				if (IsSimulationStarted && IsSimulationPaused)
				{
					UpdateSimulation();
				}
			}
		}


		void SimulationController::SetSimulationTime(double time)
		{
			if (IsSimulationStarted)
			{
				SimulationTime = time;
				if (SimulationTime > SimulationDuration)
				{
					SimulationTime = SimulationDuration;
				}
				if (SimulationTime < 0)
				{
					SimulationTime = 0;
				}
				if (IsSimulationPaused)
				{
					SimulationStartTime = SimulationPauseTime - SimulationTime;
					UpdateSimulation();
				}
				else
				{
					SimulationStartTime = RunTime - SimulationTime;
				}
				if (SimulationTimeChanged)
				{
					SimulationTimeChanged((float)time);
				}
			}
		}

		void SimulationController::IncSimulationTime()
		{
			if (SimulationTime < SimulationDuration)
			{
				SetSimulationTime(SimulationTime + SimulationDuration * 0.05f);
			}
		}


		void SimulationController::DecSimulationTime()
		{
			if (SimulationTime > 0)
			{
				SetSimulationTime(SimulationTime - SimulationDuration * 0.05f);
			}
		}


		void SimulationController::ResetSimulationTime()
		{
			SetSimulationTime(0);
		}


		void SimulationController::CompleteSimulationTime()
		{
			SetSimulationTime(SimulationDuration);
		}

		bool SimulationController::InitSimulation()
		{
			if (!SimulationHistory)
			{
				return false;
			}
			SimulationUpdateTime = -1.0f;
			DateTime maxDateTime = { 9999,12,31,23,59,59,0,0,0 };
			DateTime minDateTime = { 1400,1,1,0,0,0,0,0,0 };

			SimulationHistory->StartDateTime = maxDateTime;
			SimulationHistory->EndDateTime = minDateTime;
			for (auto& historyEntry : SimulationHistory->EntityHistories)
			{
				std::string entityId = historyEntry.first;
				std::shared_ptr<EntityHistory>& history = historyEntry.second;
				history->StartDateTime = maxDateTime;
				history->EndDateTime = minDateTime;
				for (auto& state : history->States)
				{
					if (history->StartDateTime > state->StartDateTime)
					{
						history->StartDateTime = state->StartDateTime;
					}
					if (history->EndDateTime < state->EndDateTime)
					{
						history->EndDateTime = state->EndDateTime;
					}
				}
				if (SimulationHistory->StartDateTime > history->StartDateTime)
				{
					SimulationHistory->StartDateTime = history->StartDateTime;
				}
				if (SimulationHistory->EndDateTime < history->EndDateTime)
				{
					SimulationHistory->EndDateTime = history->EndDateTime;
				}
				auto& exec = Executors[entityId];
				if (!exec)
				{
					exec = std::make_shared<SimulationExecutor>();
				}
				exec->History = history;
				exec->TargetElement = Scenario->GetElementById(entityId);
				if (!exec->TargetElement)
				{
					LogMessage(LOG_WARNING, "Missing target element "+entityId, "DiScenFw|Sim", true, true);
				}
			}
			SimulationDuration = gpvulc::DateTimeDistanceSecD(SimulationHistory->StartDateTime, SimulationHistory->EndDateTime);
			LogMessage(LOG_DEBUG, "Simulation loaded.", "DiScenFw|Sim", true, true);
			if (SimulationLoaded)
			{
				SimulationLoaded();
			}
			return true;
		}


		bool SimulationController::SimulationDefined()
		{
			return SimulationHistory && !SimulationHistory->EntityHistories.empty();
		}


		void SimulationController::PlaySimulation()
		{
			if (!IsSimulationStarted || IsSimulationPaused)
			{
				LogMessage(LOG_DEBUG, "PLAY", "DiScenFw|Sim",true,true);
				if (SimulationPlay)
				{
					SimulationPlay();
				}
			}
			if (IsSimulationStarted)
			{
				if (IsSimulationPaused)
				{
					IsSimulationPaused = false;
					SimulationStartTime = RunTime - (SimulationPauseTime - SimulationStartTime);
					SimulationUpdateTime = -1.0f;
				}
			}
			else
			{
				SimulationStartTime = RunTime;
				IsSimulationStarted = true;
				for (auto& executorPair : Executors)
				{
					auto& executor = executorPair.second;
					executor->Run();
				}
			}
		}


		void SimulationController::StopSimulation()
		{
			ResetSimulationTime();
			UpdateSimulation();
			SimulationUpdateTime = -1.0f;
			IsSimulationStarted = false;
			IsSimulationPaused = false;
			for (auto& executorPair : Executors)
			{
				auto& executor = executorPair.second;
				executor->UpdateTarget(SimulationDateTime);
				executor->Stop();
			}
			LogMessage(LOG_DEBUG, "STOP", "DiScenFw|Sim", true, true);
			if (SimulationStop)
			{
				SimulationStop();
			}
		}


		void SimulationController::PauseSimulation()
		{
			if (IsSimulationStarted && !IsSimulationPaused)
			{
				LogMessage(LOG_DEBUG, "PAUSE", "DiScenFw|Sim", true, true);
				if (SimulationPause)
				{
					SimulationPause();
				}
				SimulationPauseTime = RunTime;
				IsSimulationPaused = true;
				UpdateSimulation();
			}
		}


		void SimulationController::UpdateSimulation()
		{
			if (!SimulationDefined())
			{
				return;
			}

			bool pauseSim = false;

			if (IsSimulationPaused)
			{
				SimulationUpdateTime = -1.0f;
			}
			else
			{
				double t = RunTime;
				double deltaUpdateTime = SimulationUpdateTime < 0 ? 0 : RunTime - SimulationUpdateTime;
				SimulationUpdateTime = t;
				SimulationTime += deltaUpdateTime * SimulationTimeSpeed;
			}
			if (SimulationTime > SimulationDuration)
			{
				SimulationTime = SimulationDuration;
				// if passed the end then pause
				pauseSim = SimulationTimeSpeed > 0;
			}
			if (SimulationTime < 0)
			{
				SimulationTime = 0;
				// if passed the beginning then pause
				pauseSim = SimulationTimeSpeed < 0;
			}
			SimulationDateTime = gpvulc::DateTimeAddMs(SimulationHistory->StartDateTime, (long)(SimulationTime*1000.0));

			for (auto& executorPair : Executors)
			{
				executorPair.second->Update(SimulationDateTime);
			}
			if (SimulationUpdated)
			{
				SimulationUpdated();
			}
			if (pauseSim)
			{
				PauseSimulation();
			}
		}


		void SimulationController::Update(double runTime)
		{
			RunTime = runTime;
			if (IsSimulationStarted && !IsSimulationPaused)
			{
				UpdateSimulation();
			}
		}


	} // namespace sim
}


