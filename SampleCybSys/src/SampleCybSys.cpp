//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <SampleCybSys/SampleCybSys.h>

#include <discenfw/xp/EnvironmentModel.h>

#include <iostream>
#include <sstream>
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT




using namespace discenfw::xp;
using namespace sample_cybsys;

namespace
{
	inline std::string BoolToString(bool val) { return val ? "true" : "false"; }
	inline bool StringToBool(const std::string& str) { return str == "true"; }
}

namespace sample_cybsys
{

SampleCybSys::SampleCybSys()
{
	// Construct your cyber system
}


void SampleCybSys::CreateEntityStateTypes()
{
	SampleEntityType = CreateEntityStateType(
		"",
		"SampleEntityState",
		{
			{"prop1","default_val1"},
			{"prop2","default_val2"},
		},
		{
			{"prop1", { "default_val1","other_val1" } },
			{"prop2", { "default_val2","other_val2" } },
		},
		{}
		);

}

void SampleCybSys::ClearSystem()
{
	// remove all definitions, clear the system
}


void SampleCybSys::InitFailureConditions()
{
	PropertyCondition propCond({ "prop2","other_val2" });
	Condition failCond({ {"my_entity2",{propCond}} });
	FailureCondition = failCond;
}


void SampleCybSys::ResetSystem()
{
	// Reset the cyber system
}


bool SampleCybSys::ExecuteAction(const Action& action)
{
	if (action.TypeId == "sample_action")
	{
		// do the action
		return true;
	}
	return false;
}


void SampleCybSys::SynchronizeState(std::shared_ptr<xp::EnvironmentState> environmentState)
{
	std::shared_ptr<EntityState> entState = environmentState->GetEntityState("my_entity1");
	if (!entState)
	{
		entState = EntityState::Make(SampleEntityType->GetTypeName(),SampleEntityType->GetModelName());
		environmentState->SetEntityState("my_entity1", entState);
	}

	// Get properties from the cyber system
	//entState->SetPropertyValue("prop1", ...);
	//entState->SetPropertyValue("prop2", ...);

	//...
}


const std::vector<ActionRef>& SampleCybSys::GetAvailableActions(
	const std::string& roleId,
	bool smartSelection
	) const
{
	// empty the cache
	CachedAvailableActions.clear();

	// fill available actions for the current system state
	// CacheAvailableAction({ "sample_action", { ... } });

	return CachedAvailableActions;
}


void SampleCybSys::InitRoles()
{
	PropertyCondition role1wins({ "winner","role1" });
	PropertyCondition role2wins({ "winner","role2" });
	Condition role1SuccessCond({ { "game",{role1wins }} });
	Condition role2SuccessCond({ { "game",{role2wins }} });

	SetRole(
		"role1",
		role1SuccessCond, // SuccessCondition
		role2SuccessCond, // FailureCondition
		{}, // DeadlockCondition
		{
			// ResultReward
			{
				{ ActionResult::IN_PROGRESS,0 },
				{ ActionResult::SUCCEEDED,1000 },
				{ ActionResult::FAILED,-1000 },
				{ ActionResult::DEADLOCK,-10 },
			},
			// PropertyCountRewards
			{},
			// EntityConditionRewards
			{
				{ { "my_entity1",{{ "prop1","other_val1" }} }, 25 },
				{ { "my_entity2",{{ "prop1","other_val1" }} }, -50 },
			},
			// FeatureRewards
			{
			}
		}
	);

	SetRole(
		"role2",
		role2SuccessCond, // SuccessCondition
		role1SuccessCond, // FailureCondition
		{}, // DeadlockCondition
		{
			// ResultReward
			{
				{ ActionResult::IN_PROGRESS,0 },
				{ ActionResult::SUCCEEDED,1000 },
				{ ActionResult::FAILED,-1000 },
				{ ActionResult::DEADLOCK,-10 },
			},
			// PropertyCountRewards
			{},
			// EntityConditionRewards
			{
				{ { "my_entity1",{{ "prop1","other_val1" }} }, -50 },
				{ { "my_entity2",{{ "prop1","other_val1" }} }, 25 },
			},
			// FeatureRewards
			{
			}
		}
	);
}


bool SampleCybSys::SetConfiguration(const std::string& config)
{
	// configure your system with the given configuration text
	return true;
}


const std::string SampleCybSys::GetConfiguration()
{
	// return your system configuration as text
	return "";
}


const std::string SampleCybSys::ReadEntityConfiguration(const std::string& entityId)
{
	// return entity configuration from your system as text
	return "";
}


bool SampleCybSys::WriteEntityConfiguration(const std::string& entityId, const std::string& config)
{
	// configure an entity in your system with the given configuration text
	return true;
}


bool SampleCybSys::ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config)
{
	// create and configure an entity in your system with the given configuration text
	return true;
}

bool SampleCybSys::RemoveEntity(const std::string& entityId)
{
	return false;
}


const std::string SampleCybSys::GetSystemInfo(const std::string& infoId) const
{
	std::ostringstream oStr;
	if (infoId == "") // default info
	{
		//oStr << ...;
		//oStr << ...;
		oStr << std::endl;
	}
	else if (infoId == "SampleInfo") // other sample info
	{
		//oStr << ...;
		//oStr << ...;
		oStr << std::endl;
	}
	std::string info = oStr.str();
	return info;
}


// Exporting `sample_cybsys::CyberSystem` variable with alias name `CyberSystem`
// (Has the same effect as `BOOST_DLL_ALIAS(sample_cybsys::CyberSystem, CyberSystem)`)
extern "C" BOOST_SYMBOL_EXPORT SampleCybSys CyberSystem;
SampleCybSys CyberSystem;


} // namespace sample_cybsys

