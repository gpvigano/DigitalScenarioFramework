//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2019-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "DiScenFw/xp/DigitalAssistant.h"
#include "DiScenFw/xp/Condition.h"
#include "string_util.h"

#include <sstream>
#include <iostream>

using namespace discenfw::xp;

namespace
{

	inline bool IsOk(const std::shared_ptr<EntityState>& entState)
	{
		return entState->PropertyValues["Ok"] == "true";
	}


	inline std::string IsOkString(const std::shared_ptr<EntityState>& entState)
	{
		return entState->PropertyValues["Ok"];
	}


	inline std::shared_ptr<EntityState> NewState(bool ok)
	{
		auto entState = std::make_shared<EntityState>("MyEntityState");
		entState->PropertyValues["Ok"] = ok ? "true" : "false";
		return entState;
	}
}


void TestBasicXp()
{
	////	std::map<std::string, std::string> tmpmap = {
	////		{"","zzz"}
	////	};
	////	std::cout << tmpmap[""] << std::endl;
	////}
	////void test_discenaid_bak()
	////{

	EntityStateType::CreateEntityStateType(
		"MyEntityState",
		{
			{"Ok","false"},
		}
	);

	DigitalAssistant collaborator;

	auto ask = [&]()
	{
		collaborator.PrintHints();
	};

	auto addAction = [&](const std::string entityId, bool val)
	{
		std::cout << "+ " << entityId << " -> " << val << std::endl;
		Action action;
		action.TypeId = "Set";
		action.Params = { entityId, val ? "true" : "false" };
		action.ScenarioChanges.EntityStates[entityId] = NewState(val);

		////action.EntityId = entityId;
		////action.StateChange = NewState(val);

		collaborator.ProcessAction(action);
	};

	PropertyCondition okCond({ "Ok","true" });
	PropertyCondition notOkCond({ "Ok","false" });
	Condition successCond({ {"aaa",okCond}, {"bbb",okCond}, {"ccc",okCond} });
	Condition failCond({ {"",notOkCond} }, LogicOp::AND, { {"ccc",okCond} });
	//collaborator.FailureCondition = failCond;
	//collaborator.SuccessCondition = successCond;
	collaborator.SetFailureCondition(failCond);
	collaborator.SetSuccessCondition(successCond);


	ScenarioState initScenarioState =
	{
		{
			{"aaa",NewState(false)},
			{"bbb",NewState(false)},
			{"ccc",NewState(false)},
		}
	};

	collaborator.StartEpisode(initScenarioState);
	addAction("aaa", true);
	addAction("bbb", true);
	addAction("ccc", true);
	collaborator.StartEpisode(initScenarioState);
	ask();
	addAction("ccc", true);
	collaborator.StartEpisode(initScenarioState);
	ask();
	addAction("aaa", true);
	ask();
	addAction("bbb", true);
	ask();

	/*
	addAction("aaa", true);
	addAction("bbb", true);
	addAction("bbb", false);
	addAction("bbb", false);
	addAction("bbb", true);
	addAction("ccc", true);
	collaborator.StartEpisode(initScenarioState);
	ask();
	addAction("ccc", true);
	addAction("aaa", true);
	addAction("bbb", true);
	collaborator.StartEpisode(initScenarioState);
	addAction("aaa", true);
	addAction("ccc", true);
	addAction("bbb", true);
	collaborator.StartEpisode(initScenarioState);
	addAction("aaa", true);
	addAction("bbb", true);
	addAction("ccc", true);
	collaborator.StartEpisode(initScenarioState);
	addAction("aaa", true);
	addAction("bbb", true);
	ask();
	collaborator.StartEpisode(initScenarioState);
	ask();
	addAction("aaa", true);
	ask();
	addAction("ccc", true);
	ask();
	collaborator.StartEpisode(initScenarioState);
	addAction("aaa", true);
	addAction("bbb", true);
	addAction("ccc", true);
	addAction("bbb", false);
	addAction("bbb", false);
	addAction("bbb", true);
	*/

	std::string json;
	std::string json2;
	collaborator.SerializeExperience(json);
	collaborator.ClearAllExperiences();
	collaborator.ParseExperience(json);
	////collaborator.StartEpisode(initScenarioState);
	////addAction("aaa", true);
	////addAction("bbb", true);
	////addAction("ccc", true);
	collaborator.SerializeExperience(json2);
	if (json2 != json)
	{
		std::cout << "Serialization error:" << std::endl;
		std::cout << TrimString(json,json2) << std::endl;
	}
	//std::cout << Trim("aaabbbccc", "aaaccc") << std::endl;
}
