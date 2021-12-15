//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonCommonWriter.h"


namespace discenfw
{
	namespace json
	{
		using namespace rapidjson;
		using namespace discenfw;
		using namespace discenfw::xp;


		JsonCommonWriter::JsonCommonWriter()
			: JsonWriterBase()
		{
		}


		void JsonCommonWriter::WriteAction(const char* memberName, const Action& action)
		{
			StartObject(memberName);
			WriteString("TypeId", action.TypeId);

			StartArray("Params");
			for (unsigned i = 0; i < action.Params.size(); i++)
			{
				WriteString(nullptr, action.Params[i]);
			}
			EndArray();
			EndObject();
		}


		void JsonCommonWriter::WriteTransition(
			std::shared_ptr<EnvironmentModel> model,
			const Transition& transition,
			const char* name
			)
		{
			StartObject(name);
			WriteInt("StartState", model->IndexOfState(transition.StartState));
			WriteString("ActionTaken", transition.ActionTaken->ToString());
			WriteInt("EndState", model->IndexOfState(transition.EndState));
			EndObject();
		}

		void JsonCommonWriter::WriteStateActionValue(
			std::shared_ptr<xp::EnvironmentModel> model,
			const xp::StateActionRef& stateAction,
			float value,
			const char* name
			)
		{
			StartObject(name);
			WriteInt("State", model->IndexOfState(stateAction.State));
			WriteString("Action", stateAction.Action->ToString());
			WriteFloat("Value", value);
			EndObject();
		}

	}
}
