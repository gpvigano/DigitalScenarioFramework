//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <DiScenXpTest.h>
#include <string_util.h>

#include <DiScenFw/interop/CyberSystemLink.h>
#include <DiScenFw/xp/CyberSystemAssistant.h>
#include <DiScenFw/xp/CyberSystemAgent.h>
#include <DiScenFw/xp/Condition.h>

#include <DiScenFw/RL/RLAgent.h>

#include <gpvulc/console/console_menu.h>
#include <gpvulc/console/console_util.h>

#include <iostream>


using namespace discenfw::xp;

namespace discenfw_test
{

	void InitElectronicCircuit(std::shared_ptr<ICyberSystem> eCircuit)
	{
		std::string conf =
			"\
PowerSupplyDC Battery 6000 50\n\
LED LED1 Red\n\
Resistor R1 2200 500\n\
Resistor R2 50 250\n\
Switch SW1 12000 40\n";
		eCircuit->SetConfiguration(conf);

		//eCircuit.AddPowerSupplyDC("Battery", 6000, 50);
		//eCircuit.AddSwitch("SW1", 12000, 40);
		//eCircuit.AddResistor("R1", 2200, 500);
		//eCircuit.AddResistor("R2", 50, 250);
		//eCircuit.AddLed("LED1", GetLedType("Red"));
		//eCircuit.AddLed("LED2", GetLedType("Blue"));
	}


	void InitExperience(std::shared_ptr<DigitalAssistant> assistant)
	{
		assistant->SetCurrentGoal("LED circuit");

		PropertyCondition connectedCond({ "connected","true" });
		PropertyCondition connCond({ "connections","2" });
		PropertyCondition swOnCond({ "position","1" });

		PropertyCondition litUpCond({ "lit up", "true" });
		Condition successCond({ {"LED1",{litUpCond} } });
		successCond.AddCondition(LogicOp::AND, Condition({ {"SW1",{connectedCond,connCond,swOnCond} } }));

		RelationshipCondition positiveConnectedToSw1(
			"+",{"SW1", "In"}
		);
		EntityCondition batteryConnectedToSwitch(
			"Battery", {}, { positiveConnectedToSw1 }
		);

		assistant->SetRole(
			"Default",
			//  condition
			successCond,
			// failure condition
			{},//anyBurntOut
			// deadlock condition
			{},
			// Reward rules
			{
				// Result rewards
				{
					{ ActionResult::IN_PROGRESS,-1 },
					{ ActionResult::SUCCEEDED,5000 },
					{ ActionResult::FAILED,-1000 },
					{ ActionResult::DEADLOCK,-500 },
				},
			// Cumulative rewards
			{
				// reward the minimum number of resistors
				{ "Resistor", { "connected", "true" }, -10 },
			},
			// Entity condition rewards
			{
				{ batteryConnectedToSwitch, 20 },
			},
			// FeatureRewards
			{
			}

			}
			);

	}


	void TrainTestXp(const std::shared_ptr<CyberSystemAssistant>& assistant)
	{
		assistant->SetLogEnabled(true);
		assistant->SetLevel(ExperienceLevel::TRAINEE);

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "SW1", "In" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "SW1", "Out1", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));
		TestXp(assistant);
	}


	void TestUserXpSuccess(const std::shared_ptr<CyberSystemAssistant>& assistant)
	{
		auto printAvailableActions = [&]()
		{
			std::cout << "Available actions: ";
			const std::vector<ActionRef>& possibleActions = assistant->GetAvailableActions();
			for (const auto& action : possibleActions)
			{
				std::cout << action->ToString() << "\t";
			}
			std::cout << std::endl;
		};

		std::cout
			<< "___________________________________________________\n"
			<< "\n"
			<< "Testing user experience (success).\n"
			<< "___________________________________________________\n"
			<< std::endl;
		assistant->SetLogEnabled(true);
		assistant->SetLevel(ExperienceLevel::TRAINEE);

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "SW1", "In" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "SW1", "Out1", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "SW1", "In", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Cathode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "0" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "-", "SW1", "Out1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));
	}


	void TestUserXpFailure(const std::shared_ptr<CyberSystemAssistant>& assistant)
	{
		const bool hints = true;
		auto printHints = [&]()
		{
			if (hints)
			{
				assistant->PrintHints();
			}
		};
		std::cout
			<< "___________________________________________________\n"
			<< "\n"
			<< "Testing user experience (failure).\n"
			<< "___________________________________________________\n"
			<< std::endl;
		assistant->SetLogEnabled(true);
		assistant->SetLevel(ExperienceLevel::TRAINEE);


		assistant->NewEpisode();
		printHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "Battery", "-" }));

		assistant->NewEpisode();
		printHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "LED1", "Anode" }));
		printHints();
		assistant->TakeAction(Action("connect", { "Battery", "-", "LED1", "Cathode" }));

		assistant->NewEpisode();
		printHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "R2", "Pin1" }));
		printHints();
		assistant->TakeAction(Action("connect", { "Battery", "-", "R2", "Pin2" }));
	}


	void TestUserXp(const std::shared_ptr<CyberSystemAssistant>& assistant)
	{
		std::cout
			<< "___________________________________________________\n"
			<< "\n"
			<< "Testing user experience.\n"
			<< "___________________________________________________\n"
			<< std::endl;

		assistant->SetLogEnabled(true);
		assistant->SetLevel(ExperienceLevel::TRAINEE);

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "SW1", "In" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "SW1", "Out1", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));

		assistant->SetLevel(ExperienceLevel::ASSISTANT);

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "SW1", "In" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "SW1", "Out1", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "SW1", "In" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin1",  "SW1", "Out1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "-", "LED1", "Cathode" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "-", "R1", "Pin2" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "R2", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "-", "R2", "Pin2" }));

		assistant->NewEpisode();
		assistant->TakeAction(Action("connect", { "Battery", "+", "R2", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "-", "R2", "Pin2" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "Battery", "-" }));

		assistant->NewEpisode();
		assistant->PrintHints();
	}


	void TestDuplicateEpisode(const std::shared_ptr<CyberSystemAssistant>& assistant)
	{
		std::cout
			<< "___________________________________________________\n"
			<< "\n"
			<< "Testing duplicate episodes.\n"
			<< "___________________________________________________\n"
			<< std::endl;

		assistant->SetLogEnabled(true);
		assistant->SetLevel(ExperienceLevel::TRAINEE);

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "SW1", "In" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "SW1", "Out1", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "SW1", "In" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "SW1", "Out1", "R1", "Pin1" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "R1", "Pin2", "LED1", "Anode" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "LED1", "Cathode", "Battery", "-" }));
		assistant->PrintHints();
		assistant->TakeAction(Action("switch", { "SW1", "1" }));

		assistant->SetLevel(ExperienceLevel::ASSISTANT);

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "Battery", "-" }));

		assistant->NewEpisode();
		assistant->PrintHints();
		assistant->TakeAction(Action("connect", { "Battery", "+", "Battery", "-" }));

		assistant->NewEpisode();
		assistant->PrintHints();
	}


	bool TestLedCircuit(int subTestChoice)
	{
		std::shared_ptr<CyberSystemLink> eCircuit = std::make_shared<CyberSystemLink>();
		eCircuit->LoadCyberSystem("SimplECircuitCybSys");
		std::shared_ptr<CyberSystemAgent> assistant = std::make_shared<CyberSystemAgent>(eCircuit);

		// test custom agent setting
		assistant->SetCustomAgentMaker([]()
			{
				return std::make_shared<RLAgent>();
			}
		);

		InitElectronicCircuit(eCircuit);
		InitExperience(assistant);

		int savedSubTestChoice = subTestChoice;
		bool done = false;
		while (!done)
		{
			if (subTestChoice < 0)
			{
				subTestChoice = gpvulc::GetMenuChoice({
					"Successful episode",
					"Failed episode",
					"Sample episodes",
					"Duplicate episodes",
					"Serialization test",
					"Machine learning & experience training (short)",
					"Machine learning & experience training (long)",
				}, "Back");
			}


			auto testStart = GetTimeNow();
			assistant->ClearCurrentExperience();
			std::shared_ptr<RLConfig> learningConfig = std::make_shared<RLConfig>();

			switch (subTestChoice)
			{
			case 1:
				TestUserXpSuccess(assistant);
				break;
			case 2:
				TestUserXpFailure(assistant);
				break;
			case 3:
				TestUserXp(assistant);
				break;
			case 4:
				TestDuplicateEpisode(assistant);
				break;
			case 5:
				TestUserXp(assistant);
				TestXpSerialization(assistant, "../test/LedCircuitTestXp.json");
				break;
			case 6:
				TestLearner(assistant, { 200, 5, true, true, false, true, true, true, learningConfig });
				assistant->SaveExperience("../test/LedCircuitShortTraining.json");
				break;
			case 7:
				TestLearner(assistant, { 5000, 5, true, true, false, true, true, true, learningConfig });
				assistant->SaveExperience("../test/LedCircuitLongTraining.json");
				break;
			default:
				EnvironmentModel::RemoveAllModels();
				return false;
			}

			PrintTestDuration(testStart);
			gpvulc::ConsolePause();
			if (savedSubTestChoice < 0)
			{
				subTestChoice = savedSubTestChoice;
			}
		}

		return true;
	}

}
