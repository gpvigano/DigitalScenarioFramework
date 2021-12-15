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

#include "discenfw/xp/Episode.h"
#include "discenfw/xp/Condition.h"
#include "discenfw/xp/EnvironmentState.h"
#include "discenfw/xp/EnvironmentStateInfo.h"
#include "discenfw/xp/StateRewardRules.h"
#include "discenfw/xp/ExperienceLevel.h"
#include "discenfw/xp/RoleInfo.h"
#include "discenfw/xp/EnvironmentModel.h"

namespace discenfw
{
	namespace xp
	{

		/*!
		Data structure for storing experience.
		*/
		class DISCENFW_API Experience
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED
			/*!
			Environment model name (optional, using default model if empty).
			*/
			std::string Model = "";

			//! Goal name.
			std::string Goal = "";

			/*!
			Identifier of the agent's role (optional, used in multi-agent environments).
			*/
			std::string Role;

			/*!
			Identifier of the agent (optional, needed in multi-agent environments).
			*/
			std::string Agent;

			/*!
			Level of experience.
			*/
			ExperienceLevel Level = ExperienceLevel::NONE;

			/*!
			Ignore system failures (normally they are considered before any other condition)
			*/
			bool SystemFailureIgnored = false;

			/*!
			Discounting constant (0..1) for calculating discounted returns in performance calculation.
			If set to a negative value (default) it is automatically computed
			using the reward given by an intermediate step (if defined, see StateRewardRules::ResultReward).
			*/
			float DiscountingConstant = -1.0f;

			//! History of completed episodes.
			std::vector< std::shared_ptr<Episode> > Episodes;

			//! Successful episodes with the best performance.
			std::vector< std::shared_ptr<Episode> > BestEpisodes;

			//! Last successful episode with the best performance.
			std::shared_ptr<Episode> BestEpisode;

			//! List of failed transitions.
			std::vector<Transition> FailedTransitions;

			/*!
			State-action values, computed by an agent while learning.
			*/
			std::map<StateActionRef, float> StateActionValues;

		public:

			/*!
			Get a pointer to the role, if not defined a null pointer is returned.
			*/
			const std::shared_ptr<RoleInfo> GetRole() const;

			/*!
			Get the name of the role (even if not yet created).
			*/
			const std::string& GetRoleName() const;

			/*!
			Get a shared pointer to the model, if not defined it is created.
			*/
			const std::shared_ptr<EnvironmentModel> GetModel() const;

			/*!
			Get the name of the model, if not defined it is created.
			*/
			const std::string& GetModelName() const;

			/*!
			Level of experience.
			*/
			ExperienceLevel GetLevel() const
			{
				return Level;
			}


			/*!
			Get the index of the given state or -1 if not found if not found.
			*/
			int IndexOfState(const std::shared_ptr<EnvironmentState> state) const;


			/*!
			Find a stored state equal to the given one.
			Return nullptr if not found.
			*/
			std::shared_ptr<EnvironmentState> FindState(const std::shared_ptr<EnvironmentState> state) const;


			/*!
			Find or create a stored state equal to the given one.
			*/
			std::shared_ptr<EnvironmentState> GetStoredState(const std::shared_ptr<EnvironmentState> state) const;


			/*!
			Find or create a stored state equal to the given one.
			*/
			std::shared_ptr<EnvironmentState> GetStoredState(const EnvironmentState& state) const;


			/*!
			Get the state stored at the given position or null if the position is not valid.
			*/
			std::shared_ptr<EnvironmentState> GetStoredState(int stateIndex) const;


			/*!
			Update or store a state equal to the given one.
			*/
			void StoreState(const std::shared_ptr<EnvironmentState> state);

			/*!
			Evaluate a state, aacording to the success, deadlock and failure conditions defined.
			*/
			EnvironmentStateInfo GetStateInfo(const std::shared_ptr<EnvironmentState> scenarioState);


			/*!
			Check if an episode equal to the current one was already stored in experience.
			*/
			bool CheckDuplicateEpisode(std::shared_ptr<Episode> episode);

			/*!
			Store the given episode updating FailedTransitions and Episodes.
			@param episode reference to the episode to store.
			@param checkDuplicate check if the episode is duplicated.
			@return false if the episode is duplicate, false otherwise.
			*/
			bool StoreEpisode(std::shared_ptr<Episode> episode, bool checkDuplicate = true);



			/*!
			Check if the value is defined for the given state-action.
			*/
			bool StateActionValueDefined(const StateActionRef& stateAction) const;


			/*!
			Get the value of a state-action.
			*/
			float GetStateActionValue(const StateActionRef& stateAction) const;


			/*!
			Override the value of a state-action for the given state.
			*/
			void SetStateActionValue(const StateActionRef& stateAction, float actionValue);


			/*!
			Get the values of known state-actions.
			*/
			const std::map<StateActionRef, float>& GetStateActionValues() const;


			/*!
			Clear the values of known state-actions.
			*/
			void ClearStateActionValues();






			/*!
			Check if this is a valid experience (at least one episode completed).
			*/
			bool Valid();

			/*!
			Clear all episodes and reset experience.
			*/
			void Clear();
		};


		inline bool Experience::Valid()
		{
			return !Episodes.empty();
		}


		inline std::string ExperienceLevelToString(ExperienceLevel experienceLevel)
		{
			switch (experienceLevel)
			{
			case ExperienceLevel::NONE:
				break;
			case ExperienceLevel::TRAINEE:
				return "Trainee";
			case ExperienceLevel::ASSISTANT:
				return "Assistant";
			case ExperienceLevel::TRAINER:
				return "Trainer";
			}
			return "";
		}


		inline ExperienceLevel ExperienceLevelFromString(const std::string& experienceLevel)
		{
			if (experienceLevel == "Trainer") return ExperienceLevel::TRAINER;
			if (experienceLevel == "Assistant") return ExperienceLevel::ASSISTANT;
			if (experienceLevel == "Trainee") return ExperienceLevel::TRAINEE;
			return ExperienceLevel::NONE;
		}


		/*!
		Compute the actual value of discounting constant, automatically calculated according to the given parameters.
		@param singleStepReward reward of a single step
		@param episodeReward reward of a successfulEpisode
		@return The calculated discounting constant (it is a gain constant if greater than 1).
		*/
		inline float ComputeDiscountingOrGainConstant(int singleStepReward, int episodeReward)
		{
			float discountingOrGainConstant = 1.0f;

			// ratio between a single step reward and a successful episode reward
			float rewardRatio = (float)singleStepReward / (float)episodeReward;

			// if the single step reward decreases the episode reward
			// derive the discounting constant from single step reward
			if (rewardRatio < 0.0f)
			{
				rewardRatio = -rewardRatio;

				// clamp the value between 0 and 1
				if (rewardRatio > 1.0f) rewardRatio = 1.0f;

				// adjust the effect of single step reward
				// (if the discount grows the discounting constant decreases)
				discountingOrGainConstant = 1.0f - rewardRatio;
			}
			else
			{
				// limit the gain at maximum 100 per cent
				if (rewardRatio > 1.0f) rewardRatio = 1.0f;

				// adjust the effect of single step reward
				// (if the discount grows the gain constant increases)
				discountingOrGainConstant = 1.0f + rewardRatio;
			}
			return discountingOrGainConstant;
		}
	}
}

