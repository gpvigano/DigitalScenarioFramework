//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonExperienceParser.h"

using namespace rapidjson;
using namespace discenfw::xp;

namespace discenfw
{
	namespace json
	{

		JsonExperienceParser::JsonExperienceParser()
		{
		}


		std::shared_ptr<xp::Experience> JsonExperienceParser::ParseExperienceAttributes(const std::string& jsonText)
		{
			StartContext("Experience");
			std::shared_ptr<Experience> experience = std::make_shared<Experience>();

			Parse(jsonText);

			const Value& experienceValue = GetRootElement("Experience");
			experience->Model = GetAsString(experienceValue, "Model");
			experience->Goal = GetAsString(experienceValue, "Goal");
			experience->Role = GetAsString(experienceValue, "Role");
			experience->Agent = GetAsString(experienceValue, "Agent", true);
			experience->SystemFailureIgnored = GetAsBool(experienceValue, "SystemFailureIgnored", true, false);
			experience->DiscountingConstant = GetAsFloat(experienceValue, "DiscountingConstant", true, -1.0f);
			experience->Level = ExperienceLevelFromString(GetAsString(experienceValue, "Level"));
			EndContext();
			return experience;
		}

		std::shared_ptr<xp::Experience> JsonExperienceParser::ParseExperience(
			const std::string& jsonText,
			std::shared_ptr<xp::Experience> experience
			)
		{
			experience = ParseExperienceAttributes(jsonText);
			return ParseExperienceDefinition("",experience);
		}


		std::shared_ptr<xp::Experience> JsonExperienceParser::ParseExperienceDefinition(
			const std::string& jsonText,
			std::shared_ptr<xp::Experience> experience
			)
		{
			StartContext("Experience");
			if (!experience)
			{
				experience = std::make_shared<Experience>();
			}

			if(!jsonText.empty()) Parse(jsonText);

			const Value& experienceValue = GetRootElement("Experience");

			if (CheckHasArray(experienceValue, "FailedTransitions", true))
			{
				StartContext("FailedTransitions");
				const Value& failedTransitions = experienceValue["FailedTransitions"];
				experience->FailedTransitions.clear();
				for (SizeType i = 0; i < failedTransitions.Size(); i++)
				{
					StartContext(i);
					Transition transition;
					ParseTransition(experience->GetModel(), failedTransitions[i], transition);
					experience->FailedTransitions.push_back(transition);
					EndContext();
				}
				EndContext();
			}

			if (CheckHasArray(experienceValue, "Episodes", true))
			{
				StartContext("Episodes");
				const Value& episodes = experienceValue["Episodes"];
				experience->BestEpisode = nullptr;
				experience->BestEpisodes.clear();
				experience->FailedTransitions.clear();
				experience->Episodes.clear();
				for (SizeType i = 0; i < episodes.Size(); i++)
				{
					StartContext(i);
					experience->StoreEpisode(ParseEpisode(*experience, episodes[i]), false);
					EndContext();
				}
				EndContext();
			}

			if (CheckHasArray(experienceValue, "StateActionValues", true))
			{
				StartContext("StateActionValues");
				const Value& stateActionValues = experienceValue["StateActionValues"];
				experience->StateActionValues.clear();
				for (SizeType i = 0; i < stateActionValues.Size(); i++)
				{
					StartContext(i);
					StateActionRef stateAction;
					float value = 0.0f;
					ParseStateActionValue(
						experience->GetModel(),
						stateActionValues[i],
						stateAction,
						value
						);
					experience->StateActionValues[stateAction] = value;
					EndContext();
				}
				EndContext();
			}

			EndContext();
			return experience;
		}


		Action JsonExperienceParser::ParseAction(const rapidjson::Value& actionValue)
		{
			Action action;
			action.TypeId = GetAsString(actionValue, "TypeId");
			if (CheckHasArray(actionValue, "Params", true))
			{
				StartContext("Params");
				const Value& actParamsValue = actionValue["Params"];
				for (SizeType i = 0; i < actParamsValue.Size(); i++)
				{
					StartContext(i);
					action.Params.push_back(actParamsValue[i].GetString());
					EndContext();
				}
				EndContext();
			}
			return action;
		}


		std::shared_ptr<Episode> JsonExperienceParser::ParseEpisode(const Experience& experience, const rapidjson::Value& episodeValue)
		{
			std::shared_ptr<Episode> episode = std::make_shared<Episode>();
			int initStateIdx = GetAsInt(episodeValue, "InitialState", false, -1);
			episode->InitialState = initStateIdx >= 0 ? experience.GetStoredState(initStateIdx) : nullptr;
			if (CheckHasArray(episodeValue, "TransitionSequence", true))
			{
				const Value& actionsValue = episodeValue["TransitionSequence"];
				episode->TransitionSequence.clear();
				for (SizeType i = 0; i < actionsValue.Size(); i++)
				{
					StartContext(i);
					Transition transition;
					ParseTransition(experience.GetModel(), actionsValue[i], transition);
					episode->TransitionSequence.push_back(transition);
					EndContext();
				}
			}
			int finalStateIdx = GetAsInt(episodeValue, "LastState", false, -1);
			episode->LastState = finalStateIdx >= 0 ? experience.GetStoredState(finalStateIdx) : nullptr;
			episode->Performance = GetAsInt(episodeValue, "Performance", true);
			episode->Result = ActionResultFromString(GetAsString(episodeValue, "Result", true));
			episode->RepetitionsCount = GetAsInt(episodeValue, "RepetitionsCount", true);

			return episode;
		}

	}
}
