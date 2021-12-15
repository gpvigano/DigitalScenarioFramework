//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonCommonParser.h"

using namespace rapidjson;
using namespace discenfw::xp;

namespace discenfw
{
	namespace json
	{

		JsonCommonParser::JsonCommonParser()
		{
		}


		Action JsonCommonParser::ParseAction(const rapidjson::Value& actionValue)
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


		void JsonCommonParser::ParseTransition(std::shared_ptr<EnvironmentModel> model, const rapidjson::Value& transitionValue, xp::Transition& transition)
		{
			int prevStateIdx = GetAsInt(transitionValue, "StartState", false, -1);
			transition.StartState = prevStateIdx >= 0 ? model->GetStoredState(prevStateIdx) : nullptr;
			StartContext("ActionTaken");
			transition.ActionTaken = model->DecodeAction(GetAsString(transitionValue,"ActionTaken"));
			EndContext();
			int nextStateIdx = GetAsInt(transitionValue, "EndState", false, -1);
			transition.EndState = nextStateIdx >= 0 ? model->GetStoredState(nextStateIdx) : nullptr;
		}


		void JsonCommonParser::ParseStateActionValue(
			std::shared_ptr<xp::EnvironmentModel> model,
			const rapidjson::Value& stateActionValue,
			xp::StateActionRef& stateAction,
			float& actionValue
			)
		{
			int prevStateIdx = GetAsInt(stateActionValue, "State", false, -1);
			stateAction.State = prevStateIdx >= 0 ? model->GetStoredState(prevStateIdx) : nullptr;
			StartContext("Action");
			stateAction.Action = model->DecodeAction(GetAsString(stateActionValue,"Action"));
			EndContext();
			actionValue = GetAsFloat(stateActionValue, "Value", false, 0.0f);
		}

	}
}
