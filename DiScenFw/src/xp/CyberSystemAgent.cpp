//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/CyberSystemAgent.h>
#include <discenfw/RL/RLAgent.h>
#include <discenfw/util/Rand.h>
//#include <discenfw/util/MessageLog.h>

#include <iostream>



namespace discenfw
{
	namespace xp
	{

		CyberSystemAgent::CyberSystemAgent()
		{
		}


		CyberSystemAgent::CyberSystemAgent(std::shared_ptr<ICyberSystem> cyberSystem,
			const std::string& role, const std::string& id)
			: CyberSystemAssistant(cyberSystem, role, id)
		{
		}


		CyberSystemAgent::~CyberSystemAgent()
		{
		}


		void CyberSystemAgent::Initialize()
		{
			GetAgent();

			if (!IsInitialized())
			{
				CyberSystemAssistant::Initialize();
			}
			std::shared_ptr<EnvironmentState> initState = GetLastState();
			if (!initState)
			{
				CyberSystemAssistant::NewEpisode();
				initState = GetLastState();
				RegisterState(initState);
				NewEpisodeStarted = true;
			}
		}


		ActionResult CyberSystemAgent::Train(bool updateXp)
		{
			ProcessLastEpisode();
			ActionResult actionChosenResult = Act(updateXp);
			if(actionChosenResult==ActionResult::DENIED)
			{
				CyberSystemAssistant::NewEpisode();
				std::shared_ptr<EnvironmentState> initState = GetLastState();
				RegisterState(initState);
				NewEpisodeStarted = true;
			}
			LastActionResult = actionChosenResult;

			return LastActionResult;
		}


		ActionResult CyberSystemAgent::TakeAction(const Action& actionChosen, bool updateXp)
		{
			ProcessLastEpisode();

			const ActionRef actionChosenRef = GetActionRef(actionChosen);
			StateRef prevStateRef = RegisterState(GetLastState());
			ActionResult actionChosenResult = CyberSystemAssistant::TakeAction(actionChosen, updateXp);
			if(actionChosenResult==ActionResult::DENIED)
			{
				return actionChosenResult;
			}
			// process the action result

			LastActionResult = ProcessActionResult(prevStateRef, actionChosenRef, actionChosenResult, -1, updateXp);
			return LastActionResult;
		}


		void CyberSystemAgent::UpdateState()
		{
			Initialize();

			ActionResult result = LastEpisodeResult();
			StateRef nextStateRef = RegisterState(GetLastState());
			if (result != ActionResult::IN_PROGRESS)
			{
				std::shared_ptr<SharedArena> sharedArena = Arena.lock();
				if (sharedArena && sharedArena->MultiActor())
				{
					// update last result if affected by external actions


					std::shared_ptr<Experience> experience = GetCurrentExperience();

					EnvironmentStateInfo stateInfo = GetStateInfo(nextStateRef);
					int reward = stateInfo.Reward;
					ActionResult result = stateInfo.Result;
					UpdateStats(result);
					if (LearningEnabled)
					{
						GetAgent()->Learn(experience, CurrentEpisode->TransitionSequence, result);
					}
				}
			}
			LastActionResult = result;
		}



		/*!
		Get the reinforcement learning configuration parameters (see RLConfig).
		*/

		const std::shared_ptr<IAgentConfiguration> CyberSystemAgent::GetAgentConfiguration()
		{
			if (!AgentConfiguration)
			{
				AgentConfiguration = GetAgent()->GetConfiguration();
			}
			return AgentConfiguration;
		}


		void CyberSystemAgent::SetAgentConfiguration(const std::shared_ptr<IAgentConfiguration> config)
		{
			GetAgent()->SetConfiguration(config);
			AgentConfiguration = GetAgent()->GetConfiguration();
		}


		void CyberSystemAgent::ResetStats()
		{
			Statistics.EpisodesCount = 0;
			Statistics.FailedCount = 0;
			Statistics.SuccessCount = 0;
			Statistics.DeadlockCount = 0;
			Statistics.ActionChoiceCount = 0;
			Statistics.ExplorationActionCount = 0;
			GetAgent()->ResetStats();
		}


		void CyberSystemAgent::ResetAgentForCurrentGoal()
		{
			StateVisitCount.clear();
			StateSet.clear();
			DeadlockActions.clear();
			Statistics = AgentStats();
			LastTransition = Transition();
			GetAgent()->Reset();
		}


		void CyberSystemAgent::ResetAgent()
		{
			for (auto& agentPair : Agents)
			{
				agentPair.second->Reset();
			}
		}




		std::shared_ptr<IAgent> CyberSystemAgent::MakeAgent()
		{
			std::shared_ptr<IAgent> agent;
			if (CustomAgentMaker)
			{
				agent = CustomAgentMaker();
			}
			else
			{
				// make a default RL agent
				agent = std::make_shared<RLAgent>();
			}
			if (AgentConfiguration)
			{
				agent->SetConfiguration(AgentConfiguration);
			}
			return agent;
		}


		void CyberSystemAgent::SetCustomAgentMaker(std::function<std::shared_ptr<IAgent>()> makeAgentCallback)
		{
			Agents.clear();
			CustomAgentMaker = makeAgentCallback;
		}


		std::shared_ptr<IAgent> CyberSystemAgent::GetAgent()
		{
			std::string goal = GetCurrentGoal();
			if (!Agents[goal])
			{
				Agents[goal] = MakeAgent();
			}
			return Agents[goal];
		}


		void CyberSystemAgent::ProcessLastEpisode()
		{
			Initialize();

			std::shared_ptr<SharedArena> sharedArena = Arena.lock();
			if (sharedArena && sharedArena->MultiActor())
			{
				ActionResult result = LastEpisodeResult();
				NewEpisodeStarted = result != ActionResult::IN_PROGRESS;
				LastActionResult = result;
			}
			else
			{
				NewEpisodeStarted = LastActionResult != ActionResult::IN_PROGRESS;
			}

			if (NewEpisodeStarted)
			{
				if (sharedArena)
				{
					sharedArena->NewEpisode(shared_from_this());
				}
				else
				{
					NewEpisode();
				}
				std::shared_ptr<EnvironmentState> initState = GetLastState();
				RegisterState(initState);
			}
		}




		ActionResult CyberSystemAgent::Act(bool updateXp)
		{
			std::shared_ptr<Experience> experience = GetCurrentExperience();
			ActionResult result;

			StateRef prevStateRef = RegisterState(GetLastState());
			std::vector<ActionRef> possibleActions;

			// remove actions leading to a deadlock (if loop detection is enabled)

			bool storedDeadlockActions = DeadlockActions.find(prevStateRef) != DeadlockActions.end()
				&& !DeadlockActions.at(prevStateRef).empty();
			if (LoopDetectionEnabled && storedDeadlockActions)
			{
				const std::vector<ActionRef>& availableActions = GetAvailableActions(true);
				GetPossibleActions(prevStateRef, availableActions, possibleActions);
			}
			else
			{
				possibleActions = GetAvailableActions(true);
			}

			// choose an action

			int actionChosenIndex = -1;

			if (!possibleActions.empty())
			{
				if (LearningEnabled)
				{
					actionChosenIndex = GetAgent()->ChooseAction(experience, possibleActions, prevStateRef, true);
					Statistics.ActionChoiceCount = GetAgent()->GetActionChoiceCount();
					Statistics.ExplorationActionCount = GetAgent()->GetExplorationActionCount();
				}
				else
				{
					actionChosenIndex = GetAgent()->ChooseAction(experience, possibleActions, prevStateRef, false);
				}
			}

			bool actionAvailable = (actionChosenIndex >= 0);

			ActionResult actionChosenResult = ActionResult::IN_PROGRESS;

			if (actionAvailable)
			{
				// do the action

				const ActionRef& chosenAction = possibleActions[actionChosenIndex];
				actionChosenResult = CyberSystemAssistant::TakeAction(*chosenAction, updateXp);
				if (actionChosenResult == ActionResult::DENIED)
				{
					return actionChosenResult;
				}
				result = ProcessActionResult(prevStateRef, chosenAction, actionChosenResult, (int)possibleActions.size() - 1, updateXp);
			}
			else
			{
				result = LastEpisodeResult();
				// if no action available set the result to "DEADLOCK"
				if (result == ActionResult::IN_PROGRESS)
				{
					result = ActionResult::DEADLOCK;
					GetRole()->OverrideStateResult(prevStateRef, result);
					if (LastTransition.ActionTaken)
					{
						DeadlockActions[prevStateRef].push_back(LastTransition.ActionTaken);
					}
					UpdateStats(result);
				}
			}

			return result;
		}


		void CyberSystemAgent::UpdateLastAction()
		{
			StateRef nextStateRef = RegisterState(GetLastState());

			std::shared_ptr<Experience> experience = GetCurrentExperience();

			EnvironmentStateInfo stateInfo = GetStateInfo(nextStateRef);
			int reward = stateInfo.Reward;
			ActionResult result = stateInfo.Result;
			UpdateStats(result);
			if (LearningEnabled)
			{
				GetAgent()->Learn(experience, GetCurrentEpisode()->TransitionSequence, result);
			}
		}



		bool CyberSystemAgent::GetPossibleActions(const StateRef prevStateRef,
			const std::vector<ActionRef>& availableActions, std::vector<ActionRef>& possibleActions)
		{
			if (availableActions.empty())
			{
				return false;
			}

			const std::vector<ActionRef>& deadlockActions = DeadlockActions[prevStateRef];
			for (const ActionRef action : availableActions)
			{
				bool isDeadlockAction = (std::find(deadlockActions.cbegin(), deadlockActions.cend(), action) != deadlockActions.cend());
				if (!isDeadlockAction)
				{
					possibleActions.push_back(action);
				}
			}

			if (possibleActions.empty())
			{
				return false;
			}

			return true;
		}


		bool CyberSystemAgent::DetectLoop(const std::vector<Transition>& transitionSequence, const Transition& transition)
		{
			if (!transitionSequence.empty())
			{
				bool alternativePathFound = false;
				for (int i = (int)transitionSequence.size() - 1; i >= 0; i--)
				{
					if (transitionSequence[i].StartState == transition.EndState)
					{
						// reached a previous state with no escape path
						return true;
					}
				}
			}
			return false;
		}


		ActionResult CyberSystemAgent::ProcessActionResult(
			StateRef prevStateRef,
			const ActionRef actionChosen,
			ActionResult actionChosenResult,
			int alternativeActionCount,
			bool updateXp
			)
		{
			std::shared_ptr<Experience> experience = CurrentExperience();

			StateRef nextStateRef = RegisterState(GetLastState());
			//Transition chosenTransition(prevStateRef, actionChosen, nextStateRef);
			const std::vector<Transition>& transitionSequence = GetCurrentEpisode()->TransitionSequence;
			const Transition& chosenTransition = transitionSequence.back();
			LastTransition = chosenTransition;
			if (actionChosenResult == ActionResult::DEADLOCK)
			{
				DeadlockActions[prevStateRef].push_back(actionChosen);
			}
			if (LoopDetectionEnabled && actionChosenResult == ActionResult::IN_PROGRESS)
			{
				// manage (presumed) deadlock
				bool loop = DetectLoop(transitionSequence, chosenTransition);
				if (loop)
				{
					// a loop does not necessarily mean a deadlock if possible alternative actions are available
					if (alternativeActionCount == 0)
					{
						actionChosenResult = ActionResult::DEADLOCK;
						GetRole()->OverrideStateResult(nextStateRef, actionChosenResult);
						CurrentEpisode->Result = actionChosenResult;
					}
					DeadlockActions[prevStateRef].push_back(actionChosen);
				}
			}

			// Update the optimal value function approximator

			if (LearningEnabled)
			{
				GetAgent()->Learn(experience, transitionSequence, actionChosenResult);
			}
			//Statistics.ActionChoiceCount++;

			// update statistics
			UpdateStats(actionChosenResult);

			std::shared_ptr<SharedArena> sharedArena = Arena.lock();
			if (sharedArena)
			{
				sharedArena->NotifyTransition(shared_from_this(), chosenTransition, updateXp);
			}

			return actionChosenResult;
		}


		void CyberSystemAgent::UpdateStats(ActionResult result)
		{
			switch (result)
			{
			case ActionResult::IN_PROGRESS:
				break;
			case ActionResult::SUCCEEDED:
				Statistics.SuccessCount++;
				Statistics.EpisodesCount++;
				break;
			case ActionResult::FAILED:
				Statistics.FailedCount++;
				Statistics.EpisodesCount++;
				break;
			case ActionResult::DEADLOCK:
				Statistics.DeadlockCount++;
				Statistics.EpisodesCount++;
				break;
			}
		}


		StateRef CyberSystemAgent::RegisterState(std::shared_ptr<EnvironmentState> state)
		{
			StateSet.insert(state);
			Statistics.StatesCount = (int)StateSet.size();
			return state;
		}



	} // namespace xp
} // namespace discenfw

