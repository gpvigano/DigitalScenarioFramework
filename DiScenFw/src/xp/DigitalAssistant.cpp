//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/DigitalAssistant.h>
#include <discenfw/xp/EnvironmentModel.h>

#include "../JSON/JsonExperience.h"
#include "../JSON/JsonRoleInfo.h"
#include "../JSON/JsonEnvironmentModel.h"

#include <discenfw/util/MessageLog.h>

#include <gpvulc/text/text_util.h>
#include <gpvulc/path/PathInfo.h>
#include <gpvulc/json/RapidJsonInclude.h> // ParseException, FormatException

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <math.h>

namespace discenfw
{
	namespace xp
	{

		DigitalAssistant::DigitalAssistant(const std::string& role, const std::string& id)
			: CurrentRole(role), Id(id)
		{
		}


		DigitalAssistant::~DigitalAssistant()
		{
		}


		void DigitalAssistant::GetGoals(std::vector<std::string>& goalNames) const
		{
			goalNames.clear();
			for (const auto& item : WealthOfExperiences)
			{
				goalNames.push_back(item.first);
			}
		}


		const std::vector<std::string>& DigitalAssistant::GetGoalNames() const
		{
			CachedGoalNames.clear();
			for (const auto& item : WealthOfExperiences)
			{
				CachedGoalNames.push_back(item.first);
			}
			return CachedGoalNames;
		}


		bool DigitalAssistant::SetCurrentGoal(const std::string& goal)
		{
			if (CurrentGoal == goal)
			{
				return true;
			}

			CurrentEpisode = nullptr;
			CurrentGoal = goal;
			CurrentExperience()->Goal = goal;
			return true;
		}


		bool DigitalAssistant::RenameCurrentGoal(const std::string& newGoalName)
		{
			if (newGoalName == CurrentGoal || newGoalName.empty())
			{
				return false;
			}
			if (WealthOfExperiences.find(newGoalName) != WealthOfExperiences.end())
			{
				return false;
			}
			if (WealthOfExperiences.find(CurrentGoal) == WealthOfExperiences.end())
			{
				return false;
			}
			WealthOfExperiences[newGoalName] = WealthOfExperiences[CurrentGoal];
			WealthOfExperiences.erase(CurrentGoal);
			CurrentGoal = newGoalName;
			WealthOfExperiences[CurrentGoal]->Goal = CurrentGoal;
			return true;
		}


		bool DigitalAssistant::AddNewGoal(const std::string& goal)
		{
			if (WealthOfExperiences.find(goal) != WealthOfExperiences.end())
			{
				return false;
			}
			WealthOfExperiences[goal] = std::make_shared<Experience>();
			WealthOfExperiences[goal]->Goal = goal;
			WealthOfExperiences[goal]->Model = CurrentModel;
			WealthOfExperiences[goal]->Role = CurrentRole;
			return true;
		}


		bool DigitalAssistant::RemoveGoal(const std::string& goal)
		{
			if (WealthOfExperiences.find(goal) == WealthOfExperiences.end())
			{
				return false;
			}
			WealthOfExperiences.erase(goal);
			if (goal == CurrentGoal)
			{
				if (WealthOfExperiences.empty())
				{
					CurrentGoal = "";
				}
				else
				{
					CurrentGoal = WealthOfExperiences.begin()->first;
				}
			}
			return true;
		}


		const std::shared_ptr<RoleInfo> DigitalAssistant::GetRole() const
		{
			return GetCurrentExperience()->GetRole();
		}


		void DigitalAssistant::SetRole(const std::shared_ptr<RoleInfo> role, const std::string& goalName)
		{
			std::string goal = goalName.empty() ? CurrentGoal : goalName;
			bool existingGoal = WealthOfExperiences.find(goal) != WealthOfExperiences.end();
			if (!existingGoal)
			{
				LogMsg(LOG_WARNING, "Invalid goal: " + goal);
				return;
			}
			CurrentExperience()->GetModel()->StoreRole(role);
			WealthOfExperiences[goal]->Role = role->GetRoleName();
		}

		bool DigitalAssistant::SetRole(
			const std::string& roleName,
			const Condition& successCondition,
			const Condition& failureCondition,
			const Condition& deadlockCondition,
			const StateRewardRules& stateRewardRules,
			const std::string& goalName)
		{
			std::shared_ptr<EnvironmentModel> model;
			if(goalName.empty())
			{
				model = CurrentExperience()->GetModel();
			}
			else
			{
				auto expIt = WealthOfExperiences.find(goalName);
				if(expIt==WealthOfExperiences.end())
				{
					LogMessage(LOG_ERROR, "Assistant " + Id + " - Undefined goal: " + goalName, "DiScenFw");
					return false;
				}

				model = expIt->second->GetModel();
			}
			return model->SetRole(
				roleName,
				successCondition,
				failureCondition,
				deadlockCondition,
				stateRewardRules
				);
		}


		ExperienceLevel DigitalAssistant::SetLevel(ExperienceLevel level)
		{
			if (CurrentExperience()->Valid() && level != ExperienceLevel::NONE)
			{
				CurrentExperience()->Level = level;
			}
			return GetLevel();
		}


		void DigitalAssistant::ClearAllExperiences()
		{
			CurrentEpisode = nullptr;
			WealthOfExperiences.clear();
			LastEpisodePerformance = 0;
		}


		void DigitalAssistant::ClearCurrentExperience()
		{
			CurrentEpisode = nullptr;
			CurrentExperience()->Clear();
			LastEpisodePerformance = 0;
		}


		void DigitalAssistant::SetSuccessCondition(const Condition& successCondition)
		{
			if (!GetCurrentExperience()->Episodes.empty())
			{
				ClearCurrentExperience();
			}
			GetRole()->SetSuccessCondition(successCondition);
		}


		void DigitalAssistant::AddSuccessCondition(const Condition& successCondition)
		{
			if (!GetCurrentExperience()->Episodes.empty())
			{
				ClearCurrentExperience();
			}
			GetRole()->AddSuccessCondition(successCondition);
		}


		void DigitalAssistant::SetFailureCondition(const Condition& failureCondition)
		{
			if (!GetCurrentExperience()->Episodes.empty())
			{
				ClearCurrentExperience();
			}
			GetRole()->SetFailureCondition(failureCondition);
		}


		void DigitalAssistant::AddFailureCondition(const Condition& failureCondition)
		{
			if (!GetCurrentExperience()->Episodes.empty())
			{
				ClearCurrentExperience();
			}
			GetRole()->AddFailureCondition(failureCondition);
		}


		void DigitalAssistant::SetDeadlockCondition(const Condition& deadlockCondition)
		{
			if (!GetCurrentExperience()->Episodes.empty())
			{
				ClearCurrentExperience();
			}
			GetRole()->SetDeadlockCondition(deadlockCondition);
		}


		void DigitalAssistant::AddDeadlockCondition(const Condition& deadlockCondition)
		{
			if (!GetCurrentExperience()->Episodes.empty())
			{
				ClearCurrentExperience();
			}
			GetRole()->AddDeadlockCondition(deadlockCondition);
		}


		bool DigitalAssistant::StartEpisode(const StateRef initialState)
		{
			if (!GetCurrentExperience())
			{
				return false;
			}
			if (CurrentEpisode && GetCurrentExperience()->Level != ExperienceLevel::TRAINER)
			{
				if (LogEnabled && !CurrentEpisode->Completed() && !CurrentEpisode->Empty())
				{
					std::string msg = CurrentGoal + ": episode n." + std::to_string(GetCurrentExperience()->Episodes.size() + 1)
						+ " incomplete (discarded).\n";
					LogMsg(LOG_DEBUG, msg);
				}
			}

			CurrentEpisode = std::make_shared<Episode>();
			CurrentEpisode->InitialState = initialState;
			CurrentEpisode->LastState = initialState;
			if (LogEnabled)
			{
				if (GetCurrentExperience()->Level == ExperienceLevel::TRAINER)
				{
					LogMsg(LOG_DEBUG, CurrentGoal + ": new session started\n");
				}
				else
				{
					std::ostringstream oStr;
					oStr << "\n"
						<< CurrentGoal << ": new episode\n"
						<< "------------------------------------------\n"
						<< std::endl;
					LogMsg(LOG_DEBUG, oStr.str());
				}
			}

			return true;
		}


		bool DigitalAssistant::NewEpisode()
		{
			if (!CurrentEpisode)
			{
				return false;
			}

			StartEpisode(CurrentEpisode->InitialState);
			return true;
		}



		ActionResult DigitalAssistant::ApplyChangesToEpisode(
			const EnvironmentState& scenarioChanges)
		{
			CurrentEpisode->LastState = GetCurrentExperience()->GetModel()->ChangeState(CurrentEpisode->LastState, scenarioChanges);
			ActionResult result = GetLastStateInfo().Result;
			CurrentEpisode->Result = result;

			EvaluateEpisode();

			return result;
		}


		void DigitalAssistant::SetLogEnabled(bool enabled)
		{
			LogEnabled = enabled;
		}



		//-------------------------------------------------------------
		// protected:



		const StateRef DigitalAssistant::GetStoredState(const StateRef environmentState)
		{
			return GetCurrentExperience()->GetModel()->GetStoredState(environmentState);
		}


		bool DigitalAssistant::GetLastActionsSequence(std::vector<Action>& actionsSequence) const
		{
			actionsSequence.clear();
			if (!GetCurrentExperience())
			{
				return false;
			}
			for (const Transition& actionInfo : CurrentEpisode->TransitionSequence)
			{
				actionsSequence.push_back(*actionInfo.ActionTaken);
			}
			return true;
		}

		const std::vector<Action>& DigitalAssistant::GetLastActionsSequence() const
		{
			if(ActionsSequenceChanged)
			{
				CachedActionsSequence.clear();
				GetLastActionsSequence(CachedActionsSequence);
				ActionsSequenceChanged = false;
			}
			return CachedActionsSequence;
		}


		bool DigitalAssistant::GetSuggestedActions(std::vector<Action>& suggestedActions) const
		{
			std::vector<Transition> transitions;
			std::vector<int> performances;
			suggestedActions.clear();
			std::shared_ptr<Experience> currExperience = GetCurrentExperience();
			if (!currExperience || currExperience->Level < ExperienceLevel::ASSISTANT)
			{
				return false;
			}
			std::shared_ptr<Episode> bestEpisode = currExperience->BestEpisode;
			if (bestEpisode)
			{
				int bestPerformance = bestEpisode->Performance;
				for (std::shared_ptr<Episode> episode : currExperience->BestEpisodes)
				{
					// search the scenario state in transitions sequence
					bool stateFound = false;
					for (unsigned int i = 0; !stateFound && i < episode->TransitionSequence.size(); i++)
					{
						const Transition& currTransition = episode->TransitionSequence[i];
						// check if each intermediate state is equal to the current state
						if (currTransition.StartState == CurrentEpisode->LastState)
						{
							stateFound = true;
							// store the action
							suggestedActions.push_back(*(currTransition.ActionTaken));
						}
					}
				}
			}

			return !suggestedActions.empty();
		}


		const std::vector<Action>& DigitalAssistant::GetSuggestedActions() const
		{
			CachedSuggestedActions.clear();
			GetSuggestedActions(CachedSuggestedActions);
			return CachedSuggestedActions;
		}


		bool DigitalAssistant::GetForbiddenActions(std::vector<Action>& forbiddenActions) const
		{
			forbiddenActions.clear();

			if (GetCurrentExperience()->FailedTransitions.empty())
			{
				return false;
			}
			for (const Transition& transition : GetCurrentExperience()->FailedTransitions)
			{
				if (transition.StartState == CurrentEpisode->LastState)
				{
					forbiddenActions.push_back(*(transition.ActionTaken));
				}
			}

			return !forbiddenActions.empty();
		}


		const std::vector<Action>& DigitalAssistant::GetForbiddenActions() const
		{
			CachedForbiddenActions.clear();
			GetForbiddenActions(CachedForbiddenActions);
			return CachedForbiddenActions;
		}


		EnvironmentStateInfo DigitalAssistant::GetStateInfo(const StateRef environmentState) const
		{
			EnvironmentStateInfo result = GetRole()->GetStateInfo(environmentState);
			return result;
		}


		int DigitalAssistant::GetLastStateReward() const
		{
			return CurrentEpisode ? GetLastStateInfo().Reward : 0;
		}


		void DigitalAssistant::AcquireTransition(
			const Transition& newTransition,
			bool updateXp)
		{
			const StateRef& newState = newTransition.EndState;
			if (CurrentEpisode->LastState == newState)
			{
				return;
			}
			CurrentEpisode->TransitionSequence.push_back(newTransition);
			ActionsSequenceChanged = true;
			CurrentEpisode->LastState = newState;
			EvaluateEpisode();
			UpdateState();
			if (updateXp)
			{
				ProcessCurrentEpisode();
			}
		}


		void DigitalAssistant::UpdateState()
		{
			// Do nothing here, this method is overridden in derived classes
			// to get up to date with the new state before processing the episode.
		}


		std::shared_ptr<Experience> DigitalAssistant::ParseExperience(const std::string& jsonText, std::shared_ptr<Experience> experience, bool attribOnly)
		{
			std::string errMsg;
			try
			{
				if (attribOnly)
				{
					experience = ExperienceAttributesFromJson(jsonText);
				}
				else
				{
					experience = ExperienceFromJson(jsonText, experience);
				}
			}
			catch (gpvulc::json::ParseException parseException)
			{
				errMsg = GetParseExceptionErrorMessage(parseException);
			}
			catch (gpvulc::json::FormatException formatException)
			{
				errMsg = std::string("JSON assert failed: ") + formatException.what();
			}
			catch (gpvulc::json::ContentException contentException)
			{
				errMsg = contentException.what();
			}

			if (!experience)
			{
				if (!errMsg.empty())
				{
					LogMsg(LOG_ERROR, "Error reading JSON:\n" + errMsg, "DiScenFw");
				}
				return nullptr;
			}
			if (attribOnly)
			{
				return experience;
			}
			std::string goal = experience->Goal;
			if (CurrentGoal.empty())
			{
				CurrentGoal = goal;
			}
			if (goal != CurrentGoal)
			{
				LogMsg(LOG_WARNING, "Experience loaded is not for the current goal.");
			}
			WealthOfExperiences[goal] = experience;
			return experience;
		}


		bool DigitalAssistant::SaveExperience(const std::string& fileName, const std::string& goalName, bool saveAll)
		{
			if (fileName.empty())
			{
				LogMsg(LOG_ERROR, "Saving experience: empty file name.");
				return false;
			}
			gpvulc::PathInfo xpPath(fileName);
			const std::string& filePath= xpPath.GetFullPath();
			std::string goal = goalName.empty() ? GetCurrentGoal() : goalName;
			LogMsg(LOG_DEBUG, "Saving experience for " + goal);
			std::string jsonText;
			if (!SerializeExperience(jsonText, goal))
			{
				return false;
			}
			jsonText += "\n"; // add a newline at the end of file
			if (!gpvulc::SaveText(filePath, jsonText))
			{
				LogMsg(LOG_ERROR, " Failed to save " + filePath);
				return false;
			}
			if (saveAll)
			{
				std::string modelFileName = xpPath.GetName() + "_" + gpvulc::GetCidStr(GetCurrentExperience()->GetModelName()) + "_model";

				// save the model in the same path as the experience

				gpvulc::PathInfo modelFile(xpPath.GetPath(), modelFileName, "json");
				gpvulc::PathInfo modelFileKnowl(xpPath.GetPath(), modelFileName+"_knowl", "json");

				SaveModel(modelFile.GetFullPath(), modelFileKnowl.GetFullPath());

				//gpvulc::PathInfo modelFile(xpPath.GetPath(), modelFileName, "json");

				//LogMsg(LOG_DEBUG, " Saving model to " + modelFile.GetFullPath());
				//bool modelSaved = SaveModel(modelFile.GetFullPath());
				//if (!modelSaved)
				//{
				//	LogMsg(LOG_ERROR, "Failed to save " + modelFile.GetFullPath());
				//}

				//const std::vector< std::string >& roleNames = GetCurrentExperience()->GetModel()->GetRoleNames();
				//bool roleSaved = roleNames.empty();
				//for (size_t i = 0; i < roleNames.size(); i++)
				//{
				//	// save the role in the same path as the experience

				//	std::string roleFileName = xpPath.GetName();
				//	std::string roleName = roleNames[i];
				//	if (!roleName.empty())
				//	{
				//		roleFileName += "_" + gpvulc::GetCidStr(roleName);
				//	}
				//	roleFileName += "_role";
				//	gpvulc::PathInfo roleFile(xpPath.GetPath(), roleFileName, "json");

				//	LogMsg(LOG_DEBUG, " Saving role " + roleName + " to " + roleFile.GetFullPath());
				//	roleSaved = SaveRole(GetCurrentExperience()->GetModel()->GetRole(roleName), roleFile.GetFullPath(), goal);
				//	if (!roleSaved)
				//	{
				//		LogMsg(LOG_ERROR, "Failed to save " + roleFile.GetFullPath());
				//	}
				//}

				//if(!roleSaved && !modelSaved)
				//{
				//	return false;
				//}
			}
			LogMsg(LOG_DEBUG, "Experience saved to " + filePath);
			return true;
		}


		bool DigitalAssistant::SerializeExperience(std::string& jsonText, const std::string& goalName)
		{
			std::string goal = goalName;
			if (goal.empty())
			{
				goal = CurrentGoal;
			}
			bool existingGoal = WealthOfExperiences.find(goal) != WealthOfExperiences.end();
			if (!existingGoal)
			{
				LogMsg(LOG_ERROR, " Goal " + goal + " not found.");
				return false;
			}
			ExperienceToJson(WealthOfExperiences[goal], jsonText);
			return true;
		}


		bool DigitalAssistant::LoadExperience(const std::string& fileName, bool loadAll)
		{
			LogMsg(LOG_DEBUG, "Loading experience from: " + fileName);
			if (fileName.empty())
			{
				LogMsg(LOG_ERROR, " Failed to load experience: empty file name.");
				return false;
			}
			gpvulc::PathInfo xpPath(fileName);
			const std::string& filePath = xpPath.GetFullPath();
			std::string jsonText;
			if (!gpvulc::LoadText(filePath, jsonText))
			{
				LogMsg(LOG_ERROR, " Failed to load " + filePath);
				return false;
			}
			std::shared_ptr<Experience> experience;
			if (loadAll)
			{
				experience = ParseExperience(jsonText, experience, true);
				if (!experience)
				{
					LogMsg(LOG_ERROR, " Failed to parse the experience attributes.");
					return false;
				}


				// load the model in the same path as the experience

				std::string modelFileName = xpPath.GetName() + "_" + gpvulc::GetCidStr(experience->Model) + "_model";

				gpvulc::PathInfo modelFileDef(xpPath.GetPath(), modelFileName, "json");
				gpvulc::PathInfo modelFileKnowl(xpPath.GetPath(), modelFileName+"_knowl", "json");

				LogMsg(LOG_DEBUG, " Loading model " + experience->Model + "...");
				bool modelLoaded = LoadModel(modelFileDef.GetFullPath(), modelFileKnowl.GetFullPath());
				if (!modelLoaded)
				{
					return false;
				}
			}
			if (!ParseExperience(jsonText, experience, false))
			{
				LogMsg(LOG_ERROR, " Failed to parse the experience.");
				return false;
			}
			LogMsg(LOG_DEBUG, "Experience loaded for " + experience->Goal + ".");
			return true;
		}


		bool DigitalAssistant::ParseModel(const std::string& jsonText, const std::string& knowlJsonText)
		{
			std::shared_ptr<EnvironmentModel> model;
			std::string errMsg;
			try
			{
				model = EnvironmentModelFromJson(jsonText,knowlJsonText);
			}
			catch (gpvulc::json::ParseException parseException)
			{
				errMsg = GetParseExceptionErrorMessage(parseException);
			}
			catch (gpvulc::json::FormatException formatException)
			{
				errMsg = std::string("JSON assert failed: ") + formatException.what();
			}
			catch (gpvulc::json::ContentException contentException)
			{
				errMsg = contentException.what();
			}

			if (!model)
			{
				if (!errMsg.empty())
				{
					LogMsg(LOG_ERROR, "Error reading JSON:\n" + errMsg);
				}
				return false;
			}
			CurrentExperience()->Model = model->GetName();
			return true;
		}


		bool DigitalAssistant::SaveModel(const std::string& fileName, const std::string& knowlFileName)
		{
			if (fileName.empty())
			{
				return false;
			}
			LogMsg(LOG_DEBUG, " Saving model definition to " + fileName);
			std::string jsonText;
			std::string knowlJsonText;
			if (!knowlFileName.empty())
			{
				EnvironmentModelToJson(GetCurrentExperience()->GetModel(), jsonText, knowlJsonText);
				jsonText += "\n"; // add a newline at the end of file
				knowlJsonText += "\n"; // add a newline at the end of file
			}
			else
			{
				EnvironmentModelDefinitionToJson(GetCurrentExperience()->GetModel(), jsonText);
				jsonText += "\n"; // add a newline at the end of file
			}
			if (!gpvulc::SaveText(fileName, jsonText))
			{
				LogMsg(LOG_ERROR, "Failed to save model definition to " + fileName);
				return false;
			}
			if (!knowlFileName.empty())
			{
				LogMsg(LOG_DEBUG, " Saving model knowledge to " + knowlFileName);
				if (!gpvulc::SaveText(knowlFileName, knowlJsonText))
				{
					LogMsg(LOG_ERROR, "Failed to save model knowledge to " + knowlFileName);
					return false;
				}
			}
			return true;
		}


		bool DigitalAssistant::LoadModel(const std::string& fileName, const std::string& knowlFileName)
		{
			if (fileName.empty())
			{
				return false;
			}
			std::string jsonText;
			std::string knowlJsonText;
			if (!gpvulc::LoadText(fileName, jsonText))
			{
				LogMsg(LOG_ERROR, "Failed to load model from " + fileName);
				return false;
			}
			if (!knowlFileName.empty() && !gpvulc::LoadText(knowlFileName, knowlJsonText))
			{
				LogMsg(LOG_WARNING, "Failed to load model knowledge from " + knowlFileName);

				// Parse the model definition anyway (do not return false),
				// its knowledge can be rebuilt.
			}
			return ParseModel(jsonText, knowlJsonText);
		}


		bool DigitalAssistant::OptimizeForAssistance()
		{
			if (GetCurrentExperience()->Level < ExperienceLevel::ASSISTANT)
			{
				return false;
			}

			// keep only best episodes (with higher performance)
			GetCurrentExperience()->Episodes.clear();

			return true;
		}


		void DigitalAssistant::PrintHints(bool toConsole, bool toScreen)
		{
			if (!GetCurrentExperience() || !GetCurrentExperience()->Valid())
			{
				return;
			}
			// print suggested actions
			std::vector<Action> suggestedActions;
			if (GetSuggestedActions(suggestedActions))
			{
				std::ostringstream oStr;
				oStr << "Suggested: " << std::endl;
				for (unsigned i = 0; i < suggestedActions.size(); i++)
				{
					const Action& action = suggestedActions[i];
					oStr << "  You can " << action.TypeId << " (";
					bool sep = false;
					for (const auto& param : action.Params)
					{
						if (sep) oStr << ", ";
						sep = true;
						oStr << param;
					}
					oStr << ")" << std::endl;
				}
				LogMessage(LOG, oStr.str(), "DiScenFw(xp)", toConsole, toScreen);
			}

			// print forbidden actions
			std::vector<Action> forbiddenActions;
			if (GetForbiddenActions(forbiddenActions))
			{
				std::ostringstream oStr;
				oStr << "Forbidden: " << std::endl;
				for (unsigned i = 0; i < forbiddenActions.size(); i++)
				{
					const Action& action = forbiddenActions[i];
					oStr << "  DO NOT " << action.TypeId << " (";
					bool sep = false;
					for (const auto& param : action.Params)
					{
						if (sep) oStr << ", ";
						sep = true;
						oStr << param;
					}
					oStr << ")" << std::endl;
				}
				LogMessage(LOG, oStr.str(), "DiScenFw(xp)", toConsole, toScreen);
			}

		}


		EnvironmentStateInfo DigitalAssistant::GetLastStateInfo() const
		{
			EnvironmentStateInfo result = GetStateInfo(CurrentEpisode->LastState);
			return result;
		}


		std::shared_ptr<Experience> DigitalAssistant::CurrentExperience()
		{
			if (WealthOfExperiences.empty() || WealthOfExperiences.find(CurrentGoal) == WealthOfExperiences.end())
			{
				WealthOfExperiences[CurrentGoal] = std::make_shared<Experience>();
				WealthOfExperiences[CurrentGoal]->Goal = CurrentGoal;
				WealthOfExperiences[CurrentGoal]->Model = CurrentModel;
				WealthOfExperiences[CurrentGoal]->Role = CurrentRole;
			}
			CurrentModel = WealthOfExperiences[CurrentGoal]->Model;
			CurrentRole = WealthOfExperiences[CurrentGoal]->Role;
			return WealthOfExperiences[CurrentGoal];
		}


		void DigitalAssistant::EvaluateEpisode()
		{
			EnvironmentStateInfo stateInfo = GetLastStateInfo();
			ActionResult result = stateInfo.Result;
			CurrentEpisode->Result = result;
			if (result != ActionResult::IN_PROGRESS)
			{

				int episodeReward = stateInfo.Reward;

				// Compute the performance of the given episode according to the final state reward only
				CurrentEpisode->Performance = episodeReward;

				int actionsCount = (int)CurrentEpisode->TransitionSequence.size();

				// Discounted results are considered only for successful (not trivial) episodes.
				// Episode performance is useful to compare only successful episodes, to suggest
				// the best action sequence.
				if (actionsCount > 1 && result == ActionResult::SUCCEEDED)
				{
					// Discounting constant (0..1) for calculating discounted returns in performance calculation.
					// if negative it is calculated according to single step reward defined in scenario reward.
					float gamma = GetCurrentExperience()->DiscountingConstant;

					// single step reward (if defined in scenario reward)
					int singleStepReward = 0;
					const std::shared_ptr<RoleInfo> role = GetRole();
					const StateRewardRules& stateReward = role->GetStateReward();
					bool singleStepRewardDefined =
						stateReward.ResultReward.find(ActionResult::IN_PROGRESS)
						!= stateReward.ResultReward.cend();
					if (singleStepRewardDefined)
					{
						singleStepReward = stateReward.ResultReward.at(ActionResult::IN_PROGRESS);

						// if the discounting constant is set to be automatically calculataed
						// (and it can be calculated)
						if (gamma < 0.0f && singleStepReward != 0 && episodeReward != 0)
						{
							gamma = ComputeDiscountingOrGainConstant(singleStepReward, episodeReward);
						}
					}
					if (gamma > 0.0f)
					{
						// Using discounted returns for performance calculation.
						// See p.335 of
						// Kubat M. (2017) “An Introduction To Machine Learning”, 2nd edition,
						// Springer International Publishing

						// floating point intermediate calculation
						float performance = episodeReward * powf(gamma, (float)actionsCount);
						CurrentEpisode->Performance = (int)performance;

						// note: in addition to the above method by Kubat,
						// here also gamma values greater than 1 are considered:
						// in this case the performance can be increased over the episode reward.
					}
					// else let performance be the episode reward

				}
				LastEpisodePerformance = CurrentEpisode->Performance;

				// promote to TRAINEE after the first completed episode
				if (CurrentEpisode->Completed() && GetCurrentExperience()->Level == ExperienceLevel::NONE)
				{
					GetCurrentExperience()->Level = ExperienceLevel::TRAINEE;
				}

				// promote to ASSISTANT after the first successful episode
				if (CurrentEpisode->Succeded() && GetCurrentExperience()->Level == ExperienceLevel::TRAINEE)
				{
					GetCurrentExperience()->Level = ExperienceLevel::ASSISTANT;
				}

				if (LogEnabled)
				{
					std::ostringstream oStr;
					if (GetCurrentExperience()->Level == ExperienceLevel::TRAINER)
					{
						oStr << CurrentGoal << ": "
							<< ActionResultToString(result);
					}
					else
					{
						size_t episodeNum = GetCurrentExperience()->Episodes.size() + 1;
						oStr << CurrentGoal << ": Episode n." << episodeNum
							<< " " << (CurrentEpisode->Succeded() ? "completed" : "failed");
						if (CurrentEpisode->Succeded())
						{
							oStr << ", performance = " << CurrentEpisode->Performance;
						}
					}
					oStr << "\n" << std::endl;
					LogMsg(LOG_DEBUG, oStr.str());
				}
			}
		}


		ActionResult DigitalAssistant::ProcessAction(
			const Action& action,
			const StateRef newState,
			bool updateXp
			)
		{
			ActionResult result = ActionResult::IN_PROGRESS;

			const StateRef prevState = GetCurrentExperience()->GetStoredState(CurrentEpisode->LastState);
			EnvironmentStateInfo stateInfo = GetStateInfo(newState);
			std::shared_ptr<Action> actionRef = GetCurrentExperience()->GetModel()->EncodeAction(action);
			Transition actionInfo;

			actionInfo.StartState = prevState;
			actionInfo.ActionTaken = actionRef;
			actionInfo.EndState = newState;

			result = stateInfo.Result;
			CurrentEpisode->LastState = newState;
			CurrentEpisode->Result = result;
			CurrentEpisode->TransitionSequence.push_back(actionInfo);
			ActionsSequenceChanged = true;
			EvaluateEpisode();
			if (updateXp)
			{
				ProcessCurrentEpisode();

				// note: the episode performance does not have to affect the state reward,
				// on the contrary, the performance depends on the reward
				// and often on the number of actions in the episode.
			}
			return result;
		}


		void DigitalAssistant::ProcessCurrentEpisode()
		{
			if (CurrentEpisode->Completed() && GetCurrentExperience()->Level != ExperienceLevel::TRAINER)
			{
				//if (CurrentEpisode->Succeded())
				if (!GetCurrentExperience()->StoreEpisode(CurrentEpisode))
				{
					if (LogEnabled)
					{
						std::string msg = CurrentGoal + ": episode n." + std::to_string(GetCurrentExperience()->Episodes.size() + 1)
							+ " duplicate (already stored).\n";
						LogMsg(LOG_DEBUG, msg);
					}
				}
			}
		}

		void DigitalAssistant::LogMsg(int severity, const std::string& message, const std::string& msgRef)
		{
			LogMessage(severity, message, "DiScenFw(xp)", ConsoleLogEnabled, ScreenLogEnabled, msgRef);
		}

	} // namespace xp
} // namespace discenfw

