//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonExperienceWriter.h"


namespace discenfw
{
	namespace json
	{
		using namespace rapidjson;
		using namespace discenfw;
		using namespace discenfw::xp;


		JsonExperienceWriter::JsonExperienceWriter()
		{
		}

		void JsonExperienceWriter::WriteExperience(const std::shared_ptr<Experience> experience, std::string& jsonText)
		{
			StartDocument();
			StartObject("Experience");
			WriteString("Model", experience->GetModel()->GetName());
			WriteString("Goal", experience->Goal);
			WriteString("Role", experience->GetRole()->GetRoleName());
			WriteString("Agent", experience->Agent, true);
			WriteString("Level", ExperienceLevelToString(experience->Level));

			if (experience->SystemFailureIgnored)
			{
				WriteBool("SystemFailureIgnored", experience->SystemFailureIgnored);
			}

			if (experience->DiscountingConstant >= 0.0f)
			{
				WriteFloat("DiscountingConstant", experience->DiscountingConstant);
			}

			if (!experience->FailedTransitions.empty())
			{
				StartArray("FailedTransitions");
				for (unsigned i = 0; i < experience->FailedTransitions.size(); i++)
				{
					WriteTransition(experience->GetModel(), experience->FailedTransitions[i]);
				}
				EndArray();
			}

			if (!experience->Episodes.empty())
			{
				StartArray("Episodes");
				for (unsigned i = 0; i < experience->Episodes.size(); i++)
				{
					std::shared_ptr<Episode> episode = experience->Episodes[i];
					WriteEpisode(experience->GetModel(), episode);
				}
				EndArray();
			}

			if (!experience->StateActionValues.empty())
			{
				StartArray("StateActionValues");
				for (const auto& stateActionValue : experience->StateActionValues)
				{
					WriteStateActionValue(
						experience->GetModel(),
						stateActionValue.first,
						stateActionValue.second
						);
				}
				EndArray();
			}

			EndObject();
			EndDocument(jsonText);
		}



		void JsonExperienceWriter::WriteEntityState(const char* memberName, const std::shared_ptr<const EntityState> entState)
		{
			StartObject(memberName);
			WriteString("Type", entState->GetTypeName());
			StartArray("Properties");
			for (auto& prop : entState->PropertyValues)
			{
				StartObject();
				WriteString("Property", prop.first);
				WriteString("Value", prop.second);
				EndObject();
			}
			EndArray();

			StartArray("Relationships");
			for (auto& ref : entState->Relationships)
			{
				StartObject();
				WriteString("StartPoint", ref.first);
				WriteString("TargetEntity", ref.second.EntityId);
				WriteString("EndPoint", ref.second.LinkId);
				EndObject();
			}
			EndArray();

			EndObject();
		}


		void JsonExperienceWriter::WriteEnvironmentState(int stateIndex, const std::shared_ptr<const EnvironmentState> state)
		{
			StartObject();
			WriteInt("Index", stateIndex);

			StartArray("EntityStates");
			for (const auto& entState : state->EntityStates)
			{
				StartObject();
				WriteString("EntityId", entState.first);
				WriteEntityState("State", entState.second);
				EndObject();
			}
			EndArray();

			EndObject();
		}


		void JsonExperienceWriter::WriteEpisode(const std::shared_ptr<EnvironmentModel> model, const std::shared_ptr<const Episode> episode)
		{
			StartObject();
			WriteInt("InitialState", model->IndexOfState(episode->InitialState));
			StartArray("TransitionSequence");
			for (unsigned i = 0; i < episode->TransitionSequence.size(); i++)
			{
				WriteTransition(model, episode->TransitionSequence[i]);
			}
			EndArray();
			WriteInt("LastState", model->IndexOfState(episode->LastState));
			WriteInt("Performance", episode->Performance);
			WriteString("Result", ActionResultToString(episode->Result));
			WriteInt("RepetitionsCount", episode->RepetitionsCount);
			EndObject();
		}


	}
}
