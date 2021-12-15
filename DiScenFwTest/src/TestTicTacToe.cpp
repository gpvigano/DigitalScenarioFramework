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
#include <DiScenFw/xp/CyberSystemAgent.h>
#include <DiScenFw/xp/Condition.h>

#include <DiScenFw/util/Rand.h>

#include <gpvulc/console/console_menu.h>
#include <gpvulc/console/console_util.h>

#include <iostream>
#include <iomanip>


using namespace discenfw::xp;

namespace discenfw_test
{
	std::shared_ptr<SharedArena> CreateComputerSharedArena(std::shared_ptr<CyberSystemLink> ticTacToe)
	{
		std::shared_ptr<CyberSystemAgent> agent1 = std::make_shared<CyberSystemAgent>(ticTacToe, "player1");
		std::shared_ptr<CyberSystemAgent> agent2 = std::make_shared<CyberSystemAgent>(ticTacToe, "player2");
		agent1->SetCurrentGoal("AI player (X)");
		agent2->SetCurrentGoal("AI player (O)");
		std::vector<std::shared_ptr<CyberSystemAssistant>> assistants = { agent1, agent2 };
		std::shared_ptr<SharedArena> arena = std::make_shared<SharedArena>(assistants);
		return arena;
	}


	void TestUserXpSuccess(std::shared_ptr<CyberSystemLink> ticTacToe)
	{
		std::shared_ptr<SharedArena> arena = CreateComputerSharedArena(ticTacToe);
		auto& player1 = arena->Actors[0];
		auto& player2 = arena->Actors[1];
		player1->SetLogEnabled(true);
		player1->SetLevel(ExperienceLevel::TRAINEE);
		player2->SetLogEnabled(true);
		player2->SetLevel(ExperienceLevel::TRAINEE);
		const bool hints = true;
		auto printHints = [&hints](std::shared_ptr<CyberSystemAssistant> assistant)
		{
			if (hints)
			{
				assistant->PrintHints();
				PrintAvailableActions(assistant);
			}
		};
		std::cout
			<< "___________________________________________________\n"
			"\n"
			"Testing user experience (success).\n"
			"___________________________________________________\n"
			<< std::endl;


		arena->NewEpisode(player2);
		printHints(player2);
		arena->TakeAction(player2, Action("move", { "1", "2" }));
		std::cout << player1->GetSystemInfo() << std::endl;
		printHints(player1);
		arena->TakeAction(player1, Action("move", { "4", "1" }));
		std::cout << player1->GetSystemInfo() << std::endl;
		printHints(player2);
		arena->TakeAction(player2, Action("move", { "2", "2" }));
		std::cout << player1->GetSystemInfo() << std::endl;
		printHints(player1);
		arena->TakeAction(player1, Action("move", { "5", "1" }));
		std::cout << player1->GetSystemInfo() << std::endl;
		printHints(player2);
		arena->TakeAction(player2, Action("move", { "3", "2" }));
		std::cout << player1->GetSystemInfo() << std::endl;

		// Note: calling player1->NewEpisode() or  player1->TakeAction()
		// does not update the state for the other player,
		// SharedArena methods must be used instead
	}


	void TestHumanVsAgent(std::shared_ptr<CyberSystemLink> ticTacToe)
	{
		std::cout << "Testing human vs RL agent (AI)\n"
			<< "_________________________\n" << std::endl;

		std::shared_ptr<CyberSystemAssistant> human = std::make_shared<CyberSystemAssistant>(ticTacToe, "player1");
		human->SetCurrentGoal("Human player (X)");
		human->SetLogEnabled(false);
		human->SetLevel(ExperienceLevel::ASSISTANT);

		std::shared_ptr<CyberSystemAgent> agent = std::make_shared<CyberSystemAgent>(ticTacToe, "player2");
		agent->SetCurrentGoal("AI player (O)");

		std::shared_ptr<RLConfig> config = std::make_shared<RLConfig>();

		//config->InitialValue = 1.0f; // boost exploration
		//config->Epsilon = 1.0f; // define initial random action probability
		//config->EpsilonReduction = 0.9f; // enable epsilon reduction

		if (agent->LoadExperience("../test/TicTacToe_ComputerXp.json"))
		{
			agent->SetLevel(ExperienceLevel::ASSISTANT);
			config->Epsilon = 0.0f; // disable random action choice
		}
		else
		{
			agent->SetLevel(ExperienceLevel::TRAINEE);
			std::cout << "No previous experience found, AI set to 'trainee' level." << std::endl;
		}
		std::cout << std::endl;
		agent->SetLogEnabled(false);

		std::vector<std::shared_ptr<CyberSystemAssistant>> assistants = { human, agent };
		std::shared_ptr<SharedArena> arena = std::make_shared<SharedArena>(assistants);
		agent->SetLearningEnabled(true);
		agent->SetLoopDetectionEnabled(false);
		agent->Initialize();
		agent->SetAgentConfiguration(config);

		arena->NewEpisode(human);

		int humanVictoryCount = 0;
		int computerVictoryCount = 0;
		int gameCount = 0;

		auto resetCounters = [&humanVictoryCount, &computerVictoryCount, &gameCount]()
		{
			humanVictoryCount = 0;
			computerVictoryCount = 0;
			gameCount = 0;
		};

		// If the game is over print the result and return true, else return false.
		auto printResult = [&humanVictoryCount, &computerVictoryCount, &gameCount](ActionResult result)
		{
			if (result == ActionResult::IN_PROGRESS || result == ActionResult::DENIED)
			{
				return false;
			}
			switch (result)
			{
			case ActionResult::FAILED:
				std::cout << "You lose";
				break;
			case ActionResult::SUCCEEDED:
				std::cout << "You win";
				break;
			case ActionResult::DEADLOCK:
				std::cout << "It's a draw";
				break;
			}
			std::cout << " (n." << gameCount << ", " << humanVictoryCount << " - " << computerVictoryCount << ")." << std::endl;
			return true;
		};

		int autoPlayStartGameCount = 0;
		bool autoPlay = false;
		int autoPlayEpisodes = 1000;
		bool quit = false;
		bool aiFirst = false;
		bool firstMove = true;

		while (!quit)
		{
			const std::vector<ActionRef>& availableActions = human->GetAvailableActions();
			std::set<char> choices;
			for (size_t i = 0; i < availableActions.size(); i++)
			{
				char c = availableActions[i]->Params[0][0];
				choices.insert(c);
			}

			if (availableActions.empty())
			{
				if (!autoPlay)
				{
					std::cout << "New game.\n" << std::endl;
				}
				if (aiFirst)
				{
					arena->NewEpisode(agent);
				}
				else
				{
					arena->NewEpisode(human);
				}
				firstMove = true;
				continue;
			}

			const std::vector<ActionRef>& availableSmartActions = human->GetAvailableActions(true);
			if (!autoPlay && !(firstMove && aiFirst))
			{
				std::cout << human->GetCyberSystem()->GetSystemInfo("PossibleMoves");
				std::cout << "Game n.: " << gameCount + 1 << ". "
					<< "Total score (You-AI): " << humanVictoryCount << " - " << computerVictoryCount << "." << std::endl;
				//AgentStats aiStats = agent->GetStatistics();
				//std::cout << " Examined states: " << aiStats.StatesCount << std::endl;;
				std::cout << "Commands: r=restart, q=quit, l=load xp, s=save xp, c=clear xp" << std::endl;
				std::cout << "          a=auto play " << autoPlayEpisodes << " games, aN=auto play N games" << std::endl;
				std::cout << "Enter a choice (";
				for (size_t i = 0; i < availableSmartActions.size(); i++)
				{
					char c = availableSmartActions[i]->Params[0][0];
					if (i > 0) std::cout << ",";
					std::cout << c;
				}
				std::cout << " ENTER=random choice): ";
			}

			char choice = 0;
			if (!(firstMove && aiFirst))
			{
				bool randomChoice = false;
				std::string inStr;
				if (!autoPlay)
				{
					std::getline(std::cin, inStr);
					randomChoice = inStr.empty();
				}
				else
				{
					randomChoice = true;
				}

				if (randomChoice)
				{
					int randIdx = RandIndex((int)availableSmartActions.size());
					choice = availableSmartActions[randIdx]->Params[0][0];
				}
				else
				{
					choice = inStr[0];
				}

				if (!autoPlay)
				{
					bool pass = false;
					std::cout << std::endl;
					if (choice == 'a' || choice == 'A')
					{
						if (inStr.length() > 1)
						{
							int n = std::atoi(&inStr[1]);
							if (n > 0)
							{
								autoPlayEpisodes = n;
							}
						}
						arena->NewEpisode(agent);
						autoPlayStartGameCount = gameCount;
						autoPlay = true;
						firstMove = true;
						agent->ResetStats();
						aiFirst = true;
						pass = true;
					}
					if (choice == 'r' || choice == 'R')
					{
						std::cout << "New game.\n" << std::endl;
						aiFirst = !aiFirst;
						firstMove = true;
						if (aiFirst)
						{
							arena->NewEpisode(agent);
							pass = true;
						}
						else
						{
							arena->NewEpisode(human);
							continue;
						}
					}
					if (choice == 'l' || choice == 'L')
					{
						agent->LoadExperience("../test/TicTacToe_ComputerXp.json");
						resetCounters();
						agent->SetLevel(ExperienceLevel::ASSISTANT);
						config->Epsilon = 0.0f; // disable random action choice
						agent->SetAgentConfiguration(config);
						arena->NewEpisode(human);
						firstMove = true;
						std::cout << "Experience loaded. New game.\n" << std::endl;
						continue;
					}
					if (choice == 'c' || choice == 'C')
					{
						agent->ClearCurrentExperience();
						agent->ResetAgent();
						resetCounters();
						arena->NewEpisode(human);
						firstMove = true;
						std::cout << "Experience cleared. New game.\n" << std::endl;
						continue;
					}
					if (choice == 's' || choice == 'S')
					{
						arena->NewEpisode(human);
						firstMove = true;
						agent->SaveExperience("../test/TicTacToe_ComputerXp.json");
						std::cout << "Experience saved. New game.\n" << std::endl;
						continue;
					}
					quit = (choice == 'q' || choice == 'Q');

					if (!quit && !pass)
					{
						if (choices.find(choice) == choices.end()) continue;
						std::cout << "choice: " << (char)choice << "\n" << std::endl;
					}
				}
			}

			bool completed = false;

			if (!quit)
			{

				if (!firstMove || !aiFirst || autoPlay)
				{
					Action humanAction;
					humanAction.TypeId = "move";
					humanAction.Params = { " ", "1" };
					humanAction.Params[0][0] = choice;
					arena->TakeAction(human, humanAction);
					firstMove = false;
					completed = human->LastEpisodeResult() != ActionResult::IN_PROGRESS;
					if (completed)
					{
						gameCount++;
					}

					switch (human->LastEpisodeResult())
					{
					case ActionResult::FAILED:
						computerVictoryCount++;
						break;
					case ActionResult::SUCCEEDED:
						humanVictoryCount++;
						break;
					}

					if (!autoPlay)
					{
						printResult(human->LastEpisodeResult());
						std::cout << human->GetCyberSystem()->GetSystemInfo();
					}
				}
			}

			if (!quit)
			{
				ActionResult result = agent->Train(true);

				if ((!autoPlay && firstMove && aiFirst) || (autoPlay&&autoPlayEpisodes == 1))
				{
					//printResult(agent->LastEpisodeResult());
					std::cout << agent->GetCyberSystem()->GetSystemInfo();
				}
				firstMove = false;
				if (result != ActionResult::IN_PROGRESS)
				{
					gameCount++;
				}
				switch (result)
				{
				case ActionResult::FAILED:
					humanVictoryCount++;
					break;
				case ActionResult::SUCCEEDED:
					computerVictoryCount++;
					break;
				}

				if (!autoPlay)
				{
					switch (result)
					{
					case ActionResult::SUCCEEDED:
						printResult(ActionResult::FAILED);
						std::cout << agent->GetCyberSystem()->GetSystemInfo();
						break;
					case ActionResult::DEADLOCK:
						printResult(ActionResult::DEADLOCK);
						std::cout << agent->GetCyberSystem()->GetSystemInfo();
						break;
					default:
						if (printResult(human->LastEpisodeResult()))
						{
							std::cout << human->GetCyberSystem()->GetSystemInfo();
						}
						break;
					}
				}
				else
				{
					AgentStats aiStats = agent->GetStatistics();
					int autoPlayEpisodeNum = gameCount - autoPlayStartGameCount;
					if (aiStats.EpisodesCount >= autoPlayEpisodes)
					{
						std::cout << "                                                                               " << std::endl;
						autoPlay = false;
						std::cout << "In " << aiStats.EpisodesCount << " episodes (random actions for opponent):\n"
							<< std::setprecision(3)
							<< " AI victories: " << aiStats.SuccessCount << " = " << (aiStats.SuccessCount*100.0f / aiStats.EpisodesCount) << "%.\n"
							<< " AI defeats: " << aiStats.FailedCount << " = " << (aiStats.FailedCount*100.0f / aiStats.EpisodesCount) << "%.\n"
							<< " Examined states: " << aiStats.StatesCount
							<< "\n Random choices: " << aiStats.ExplorationActionCount
							<< std::endl;
					}
					else if (aiStats.EpisodesCount > 0)
					{
						std::cout
							<< std::setprecision(3)
							<< aiStats.EpisodesCount << ": AI "
							<< aiStats.SuccessCount << " victories (" << aiStats.SuccessCount*100.0f / (float)aiStats.EpisodesCount << "%), "
							<< aiStats.FailedCount << " defeats (" << aiStats.FailedCount*100.0f / (float)aiStats.EpisodesCount << "%)        \r";
					}
				}
			}
		};
	}


	void TestAgentVsAgent(std::shared_ptr<CyberSystemLink> ticTacToe, int learningIterations, int exploringIterations = 0)
	{
		std::cout << "Testing RL agent vs RL agent\n"
			<< "___________________________\n" << std::endl;

		std::shared_ptr<SharedArena> arena = CreateComputerSharedArena(ticTacToe);

		std::shared_ptr<RLConfig> learningConfig = std::make_shared<RLConfig>();
		//learningConfig->Epsilon = 0.2f;
		//learningConfig->FixedStepSize = 0.5f;

		std::shared_ptr<RLConfig> exploringConfig = std::make_shared<RLConfig>();
		exploringConfig->InitialValue = 1100.0f;
		exploringConfig->Epsilon = 0.5f;
		//exploringConfig->EpsilonReduction = 0.99f;
		//exploringConfig->FixedStepSize = 0.5f;
		//exploringConfig->SampleAverage = true;

		if (exploringIterations > 0)
		{
			TestLearners(arena, { exploringIterations, 3, true, true, true, false, true, false, exploringConfig });
		}
		if (learningIterations > 0)
		{
			TestLearners(arena, { learningIterations, 3, true, true, true, false, true, false, learningConfig });
		}

		arena->Actors[1]->SaveExperience("../test/TicTacToe_ComputerXp.json");
	}


	bool TestTicTacToePlugin(int subTestChoice)
	{
		std::shared_ptr<CyberSystemLink> ticTacToe = std::make_shared<CyberSystemLink>();
		ticTacToe->LoadCyberSystem("TicTacToeCybSys");

		int savedSubTestChoice = subTestChoice;
		bool done = false;
		while (!done)
		{
			if (subTestChoice < 0)
			{
				subTestChoice = gpvulc::GetMenuChoice({
					"RL agent vs RL agent (short training)",
					"RL agent vs RL agent (long training)",
					"RL agent vs RL agent (deep exploration)",
					"Human vs RL agent",
					"RL agent training, then human vs RL agent",
					"RL agent vs RL agent (victory test)",
				}, "Back");
			}

			auto testStart = GetTimeNow();

			switch (subTestChoice)
			{
			case 1:
				TestAgentVsAgent(ticTacToe, 100);
				break;
			case 2:
				TestAgentVsAgent(ticTacToe, 1000);
				break;
			case 3:
				TestAgentVsAgent(ticTacToe, 0, 10000);
				break;
			case 4:
				TestHumanVsAgent(ticTacToe);
				break;
			case 5:
				TestAgentVsAgent(ticTacToe, 300);
				TestHumanVsAgent(ticTacToe);
				break;
			case 6:
				TestUserXpSuccess(ticTacToe);
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

