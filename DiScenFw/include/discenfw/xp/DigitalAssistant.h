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
#include "discenfw/xp/ref.h"
#include "discenfw/xp/Experience.h"
#include "discenfw/xp/Condition.h"
#include "discenfw/xp/RoleInfo.h"
#include "discenfw/xp/ICyberSystem.h"

#include <map>
#include <vector>
#include <memory>


namespace discenfw
{
	namespace xp
	{
		/*!
		Aid for computer-assisted tasks.

		It tracks activity and, after some tasks are completed,
		it can suggest a sequence of successful actions
		or it can warn about forbidden actions.
		*/
		class DISCENFW_API DigitalAssistant
		{
		public:

			/*!
			Default empty constructor.
			*/
			DigitalAssistant(const std::string& role = "", const std::string& id = "");


			virtual ~DigitalAssistant();


			/*!
			Get the goal name of the active experience.
			*/
			void GetGoals(std::vector<std::string>& goalNames) const;


			/*!
			Get the goal name of the active experience.
			*/
			const std::vector<std::string>& GetGoalNames() const;


			/*!
			Get the goal name of the active experience.
			*/
			const std::string& GetCurrentGoal() const { return CurrentGoal; }


			/*!
			Set the current goal: experience is stored separately for each goal.
			*/
			virtual bool SetCurrentGoal(const std::string& goal);


			/*!
			Rename the current goals (internal mappings are updated).
			*/
			bool RenameCurrentGoal(const std::string& newGoalName);


			/*!
			Add a new goal: experience is stored separately for each goal.
			*/
			bool AddNewGoal(const std::string& goal);


			/*!
			Remove a goal: experience is stored separately for each goal.
			*/
			bool RemoveGoal(const std::string& goal);



			/*!
			Get the role name of this assistant.
			*/
			const std::string& GetRoleName()
			{
				return GetCurrentExperience()->GetRoleName();
			}

			/*!
			Get the identifier of this assistant.
			*/
			const std::string& GetId() { return Id; }


			/*!
			Get the role of this assistant.
			*/
			const std::shared_ptr<RoleInfo> GetRole() const;


			void SetRole(const std::shared_ptr<RoleInfo> role, const std::string& goalName = "");


			bool SetRole(
				const std::string& roleName,
				const Condition& successCondition,
				const Condition& failureCondition,
				const Condition& deadlockCondition,
				const StateRewardRules& stateRewardRules,
				const std::string& goalName = ""
				);


			/*!
			Get the assistant experience level for the current goal.
			*/
			ExperienceLevel GetLevel() const { return GetCurrentExperience()->GetLevel(); }


			/*!
			Set the assistant experience level for the current goal.
			@note The experience level is automatically promoted from NONE to TRAINEE
			when an episode is completed for the current goal.
			@return the actual level (e.g. no way to change the level with no experience)
			*/
			ExperienceLevel SetLevel(ExperienceLevel level);


			/*!
			Completely erase all the episodes for all the goals.
			*/
			void ClearAllExperiences();


			/*!
			Completely erase all the episodes for the current the goal.
			*/
			void ClearCurrentExperience();


			/*!
			Set the success condition.
			*/
			void SetSuccessCondition(const Condition& successCondition);


			/*!
			Add a further success condition.
			*/
			void AddSuccessCondition(const Condition& successCondition);


			/*!
			Set the failure condition.
			*/
			void SetFailureCondition(const Condition& failureCondition);


			/*!
			Add a further failure condition.
			*/
			void AddFailureCondition(const Condition& failureCondition);


			/*!
			Set the deadlock condition.
			*/
			void SetDeadlockCondition(const Condition& failureCondition);


			/*!
			Add a further deadlock condition.
			*/
			void AddDeadlockCondition(const Condition& failureCondition);


			/*!
			Start a new episode with the given initial state.
			*/
			virtual bool StartEpisode(const StateRef initialState);


			/*!
			Start a new episode with the initial state of the previous one (if any).
			*/
			virtual bool NewEpisode();


			/*!
			Apply the given environment state changes to the current state.
			@see EnvironmentModel::ChangeState()
			@return the final result of the action
			*/
			ActionResult ApplyChangesToEpisode(const EnvironmentState& scenarioChanges);


			/*!
			Get the list of actions until the current state (consulting current experience).
			*/
			bool GetLastActionsSequence(std::vector<Action>& actionsSequence) const;


			/*!
			Get the list of actions until the current state (consulting current experience).
			*/
			const std::vector<Action>& GetLastActionsSequence() const;


			/*!
			Get a list of successful actions for the current state (consulting current experience).
			*/
			bool GetSuggestedActions(std::vector<Action>& suggestedActions) const;

			/*!
			Get a list of successful actions for the current state (consulting current experience).
			*/
			const std::vector<Action>& GetSuggestedActions() const;


			/*!
			Get a list of failing actions for the current state (consulting current experience).
			*/
			bool GetForbiddenActions(std::vector<Action>& forbiddenActions) const;


			/*!
			Get a list of failing actions for the current state (consulting current experience).
			*/
			const std::vector<Action>& GetForbiddenActions() const;


			/*!
			Get the last stored scenario state for the current episode.
			*/
			const StateRef GetLastState() const { return CurrentEpisode ? (CurrentEpisode->LastState) : nullptr; }


			/*!
			Get information about the given state.
			*/
			virtual EnvironmentStateInfo GetStateInfo(const StateRef environmentState) const;


			/*!
			Get the reward for the last stored state for the current episode (0 if no episode is defined).
			*/
			int GetLastStateReward() const;


			/*!
			Acquire the given transition (e.g. after it is modified by external agents).
			*/
			virtual void AcquireTransition(
				const Transition& newTransition,
				bool updateXp);


			/*!
			Manage a state change, if it is a consequence of external actions.
			*/
			virtual void UpdateState();


			/*!
			Get the last stored result for the current episode.
			*/
			const ActionResult LastEpisodeResult() const { return CurrentEpisode ? (CurrentEpisode->Result) : ActionResult::IN_PROGRESS; }


			/*!
			Get the last stored performance for the current episode (0 if not available).
			*/
			int GetLastEpisodePerformance() const { return LastEpisodePerformance; }


			/*!
			Get information about the given state for the role of this assistant.
			*/
			virtual EnvironmentStateInfo GetLastStateInfo() const;


			/*!
			Check if the current episode has at least an action.
			*/
			const bool EpisodeStarted() const { return CurrentEpisode && !CurrentEpisode->TransitionSequence.empty(); }


			/*!
			Check if the current episode was completed.
			*/
			const bool EpisodeCompleted() const { return CurrentEpisode && !CurrentEpisode->Completed(); }


			/*!
			Shorcut to the current experience, create it if not found for the current goal.
			*/
			virtual std::shared_ptr<Experience> CurrentExperience();


			/*!
			Shorcut to the current experience (const version).
			*/
			const std::shared_ptr<Experience> GetCurrentExperience() const
			{
				return WealthOfExperiences.at(CurrentGoal);
			}


			/*!
			Get a read-only reference to the current episode.
			*/
			const std::shared_ptr<const Episode> GetCurrentEpisode() const
			{
				return std::const_pointer_cast<const Episode>(CurrentEpisode);
			}


			/*!
			Deserialize the experience from a JSON text (previous experience is lost).
			*/
			std::shared_ptr<Experience> ParseExperience(
				const std::string& jsonText,
				std::shared_ptr<Experience> experience = nullptr,
				bool attribOnly = false
				);

			/*!
			Serialize and save the experience to a JSON text file.
			*/
			bool SaveExperience(const std::string& fileName, const std::string& goalName = "", bool saveAll = true);


			/*!
			Serialize the model to a JSON text.
			*/
			bool SerializeExperience(std::string& jsonText, const std::string& goalName = "");


			/*!
			Load and deserialize the experience from a JSON text file (previous experience is lost).
			*/
			bool LoadExperience(const std::string& fileName, bool loadAll = true);


			/*!
			Deserialize the model from a JSON text (previous model is overridden).
			*/
			bool ParseModel(const std::string& jsonText, const std::string& knowlJsonText = "");


			/*!
			Serialize and save the model to a JSON text file.
			*/
			bool SaveModel(const std::string& fileName, const std::string& knowlFileName = "");


			/*!
			Load and deserialize the model from a JSON text file (previous model is overridden).
			*/
			bool LoadModel(const std::string& fileName, const std::string& knowlFileName = "");


			/*!
			Optimize episodes for assistance (keep only best successful episodes and failures).
			*/
			bool OptimizeForAssistance();


			/*!
			Print a summary of suggested and forbidden actions to standard output (for testing).
			*/
			void PrintHints(bool toConsole = true, bool toScreen = false);


			/*!
			Enable logging, used for debugging.
			*/
			virtual void SetLogEnabled(bool enabled);


			/*!
			Check if logging is enabled, used for debugging.
			*/
			bool IsLogEnabled() const { return LogEnabled; }

		protected:

			/*!
			Identifier of the role.
			*/
			std::string CurrentModel;

			/*!
			Identifier of the role.
			*/
			std::string CurrentRole;

			/*!
			Identifier of the specific assistant (optional, used in multi-agent environments).
			*/
			std::string Id;


			// Internal flag for message logging.
			bool LogEnabled = false;

			// This can be internally enabled/disabled to help debugging class methods.
			bool ConsoleLogEnabled = true;

			// This can be internally enabled/disabled to help debugging class methods.
			bool ScreenLogEnabled = false;

			/*!
			Store experiences mapped by their defined goals.
			*/
			std::map< std::string, std::shared_ptr<Experience> > WealthOfExperiences;

			/*!
			Current episode, it will be stored in the current experience (or discarded).
			*/
			std::shared_ptr<Episode> CurrentEpisode;

			/*!
			Goal for the current experience.
			*/
			std::string CurrentGoal = "";

			/*!
			Last computed episode performance.
			*/
			int LastEpisodePerformance = 0;

			/*!
			Get a reference in the model for the given state.
			*/
			const StateRef GetStoredState(const StateRef environmentState);


			/*!
			Detect success or failure for the current episode and compute performance.
			@see GetStateInfo()
			*/
			void EvaluateEpisode();


			/*!
			Process an action and store it, return the result of the evaluation of the new state.
			*/
			virtual ActionResult ProcessAction(
				const Action& action,
				const StateRef newState,
				bool updateXp
				);


			/*!
			Process the current episode, store it if completed.
			*/
			void ProcessCurrentEpisode();

			/*!
			Log a message using the global MessageLogger and the current logging settings.
			@param severity Severity level (see LogLevel).
			@param message Text of the message to be displayed.
			@param msgRef Reference identifier used to update existing messages instead of creating a new one.
			*/
			void LogMsg(
				int severity,
				const std::string& message,
				const std::string& msgRef = "");


		private:
			mutable std::vector<std::string> CachedGoalNames;
			mutable std::vector<xp::Action> CachedSuggestedActions;
			mutable std::vector<xp::Action> CachedForbiddenActions;
			mutable std::vector<xp::Action> CachedActionsSequence;
			mutable bool ActionsSequenceChanged = false;
		};
	}
}

