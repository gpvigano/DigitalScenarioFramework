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

#include <DiScenFw/DigitalScenarioFramework.h>
#include <DiScenFw/interop/CyberSystemLink.h>
#include <DiScenFw/interop/AgentLink.h>
#include <DiScenFw/xp/CyberSystemAgent.h>
#include <DiScenFw/xp/Condition.h>

#include <DiScenFw/RL/RLConfig.h>

#include <DiScenFw/util/Rand.h>

#include "../../SampleAgent/include/SampleAgent/SampleAgentConfig.h"

#include <gpvulc/console/console_menu.h>
#include <gpvulc/console/console_util.h>

#include <iostream>
#include <iomanip>


using namespace discenfw;
using namespace discenfw::xp;

namespace discenfw_test
{
	void PrintOutcome(const ActionOutcome& outcome)
	{
		switch (outcome.Result)
		{
		case ActionResult::SUCCEEDED:
			std::cout << " Endpoint reached, performance: " << outcome.Performance << "." << std::endl;
			break;
		case ActionResult::FAILED:
			std::cout << " Trapped!" << std::endl;
			break;
		case ActionResult::DEADLOCK:
			std::cout << " (deadlock)" << std::endl;
			break;
		default:
			std::cout << std::endl;
			break;
		}
	}


	void TestGridworldAutomatic()
	{
		int actionCount = 0;
		int maxPerf = 0;
		bool newEpisode = true;
		bool goOn = true;
		bool breakOnlyOnSuccess = false;
		while (goOn)
		{
			gpvulc::ClearConsole();
			std::cout << "Gridworld Automatic Test\n"
				<< "__________________________\n" << std::endl;
			ActionOutcome outcome;
			if (newEpisode)
			{
				DiScenFw()->NewEpisode("pawn");
				actionCount = 0;
			}
			else
			{
				outcome = DiScenFw()->Train("pawn", false, AgentMode::LEARN);
				actionCount++;
			}

			std::cout << DiScenFw()->GetSystemInfo();
			PrintOutcome(outcome);
			newEpisode = outcome.CompletedEpisode;
			if (maxPerf<outcome.Performance)
			{
				maxPerf = outcome.Performance;
			}
			if(newEpisode
				&& (!breakOnlyOnSuccess || (outcome.Result == ActionResult::SUCCEEDED && outcome.Performance>=maxPerf)))
			{
				breakOnlyOnSuccess = false;
				AgentStats stats = DiScenFw()->GetAgentStats("pawn");
				std::cout << "Ep." << stats.EpisodesCount + 1
					<< " Succeeded: " << stats.SuccessCount
					<< " Failed: " << stats.FailedCount
					<< " Deadlock: " << stats.DeadlockCount
					<< " Actions: " << actionCount
					<< std::endl;
				std::cout << "Commands:" << std::endl;
				std::cout << " n=next success (excluding lower performance)" << std::endl;
				std::cout << " q=quit" << std::endl;
				std::cout << "Press ENTER to continue: ";

				std::string inStr;
				std::getline(std::cin, inStr);
				if (inStr == "q")
				{
					goOn = false;
				}
				if (inStr == "n")
				{
					breakOnlyOnSuccess = true;
				}
			}
		}
	}

	void TestGridworldInteractive()
	{
		bool newEpisode = true;
		bool goOn = true;
		int actionCount = 0;
		std::string action;
		while(goOn)
		{
			gpvulc::ClearConsole();
			std::cout << "Gridworld Interactive Test\n"
					  << "__________________________\n" << std::endl;

			ActionOutcome outcome;
			if (newEpisode)
			{
				DiScenFw()->NewEpisode("pawn");
				actionCount = 0;
			}
			else
			{
				if (!action.empty())
				{
					outcome = DiScenFw()->TakeAction("pawn", { action }, false);//, AgentMode::LEARN);
				}
				else
				{
					outcome = DiScenFw()->Train("pawn", false, AgentMode::LEARN);
				}
				actionCount++;
			}

		std::cout << DiScenFw()->GetSystemInfo();
			PrintOutcome(outcome);

			if (!newEpisode)
			{
				AgentStats stats = DiScenFw()->GetAgentStats("pawn");
				std::cout << "Ep." << stats.EpisodesCount+1
					<< " Succeeded: " << stats.SuccessCount
					<< " Failed: " << stats.FailedCount
					<< " Deadlock: " << stats.DeadlockCount
					<< " Actions: " << actionCount
					<< " Reward: " << outcome.Reward
					<< std::endl;
			}
			newEpisode = outcome.CompletedEpisode;
			if (outcome.CompletedEpisode)
			{
				std::cout << " Press ENTER to continue ('q' to quit): ";
			}
			else
			{
				std::cout << " Press ENTER for AI choice or enter an action (";
				const std::vector<ActionRef>& availableActions = DiScenFw()->GetAvailableActions("pawn", true);
				for (size_t i = 0; i < availableActions.size(); i++)
				{
					char c = availableActions[i]->TypeId[0];
					if (i > 0)
					{
						std::cout << ",";
					}
					std::cout << c;
				}
				std::cout << ") ('q' to quit): ";
			}

			std::string inStr;
			std::getline(std::cin, inStr);
			action.clear();
			if (inStr == "q")
			{
				goOn = false;
			}
			if (!outcome.CompletedEpisode && !inStr.empty())
			{
				switch (inStr[0])
				{
				case 'r':
					action = "right";
					break;
				case 'l':
					action = "left";
					break;
				case 'u':
					action = "up";
					break;
				case 'd':
					action = "down";
					break;
				default:
					break;
				}
			}
		}
	}


	bool TestGridworldPlugin(int subTestChoice)
	{
		DiScenFw()->LoadCyberSystem("Gridworld");

		// test custom agent setting
		DiScenFw()->CreateAgent("pawn","Pawn","",[]()
				{
					std::shared_ptr<AgentLink> agentLink = std::make_shared<AgentLink>();
					agentLink->LoadAgentPlugin("SampleAgent");
					return agentLink;
				}
			);

		DiScenFw()->SetCurrentGoal("pawn","Go to end");

		/*
		First line: columns rows
		following lines: grid layout
		S=start
		E=end
		B=bonus
		#=obstacle
		!=trap
		(space=empty)
		*/
		std::string config =
			"8 5\n"\
			"E  !#E$!\n"\
			"    # $ \n"\
			"### #   \n"\
			"  # ### \n"\
			"S       ";
		DiScenFw()->SetSystemConfiguration(config);

		//std::shared_ptr<RLConfig> learningConfig = std::make_shared<RLConfig>();
		//learningConfig->Epsilon = 0.1f;
		////learningConfig->Epsilon = 0.75f;
		////learningConfig->SampleAverage = true;
		//learningConfig->InitialValue = 5.0f; // boost exploration
		//learningConfig->FixedStepSize = 0.5f;
		////learningConfig->Epsilon = 1.0f;
		////learningConfig->EpsilonReduction = 0.99f;

		auto learningConfig = std::make_shared<sample_agent::SampleAgentConfig>();
		//learningConfig->BoostExploration = true;
		learningConfig->NewActionValue = 50.0f; // boost exploration
		learningConfig->LearningRate = 0.5f;

		DiScenFw()->SetAgentConfiguration("pawn", learningConfig);

		int savedSubTestChoice = subTestChoice;
		bool done = false;
		while (!done)
		{
			if (subTestChoice < 0)
			{
				subTestChoice = gpvulc::GetMenuChoice({
					"Automatic test",
					"Interactive test",
				}, "Back");
			}

			auto testStart = GetTimeNow();

			switch (subTestChoice)
			{
			case 1:
				TestGridworldAutomatic();
				break;
			case 2:
				TestGridworldInteractive();
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
} // namespace discenfw_test

