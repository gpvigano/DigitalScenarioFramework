//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "DiScenXpTest.h"
#include <DiScenFw/xp/CyberSystemAgent.h>
//#include <discenfw/util/Rand.h> // definition of  RandIndex
#include "string_util.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>

namespace discenfw_test
{
	using namespace discenfw::xp;

	void TestLearner(std::shared_ptr<CyberSystemAgent> assistant, TrainingParameters params)
	{
		// create a shared arena with only one actor
		// to reuse the multi-agent function and avoid code duplication

		std::vector<std::shared_ptr<CyberSystemAssistant> > assistants;
		assistants.push_back(assistant);
		std::shared_ptr<SharedArena> arena = std::make_shared<SharedArena>(assistants);
		TestLearners(arena, params);
	}


	void TestLearners(std::shared_ptr<SharedArena> sharedArena, TrainingParameters params)
	{
		int learningIterations = params.LearningIterations;
		int trainingIterations = params.TrainingIterations;
		bool outputSuccessEpisodes = params.OutputSuccessEpisodes;
		bool outputFailedEpisodes = params.OutputFailedEpisodes;
		bool outputInProgressEpisodes = params.OutputInProgressEpisodes;
		bool deadlockDetectionEnabled = params.DeadlockDetectionEnabled;
		bool outputOnlyTraining = params.OutputOnlyTraining;
		bool extendUntilSuccess = params.ExtendUntilSuccess;
		sharedArena->Initialize();
		std::vector<std::shared_ptr<CyberSystemAssistant> >& actors = sharedArena->Actors;
		std::vector<std::shared_ptr<CyberSystemAgent> > agents;

		// set up agents
		for (size_t i = 0; i < actors.size(); i++)
		{
			std::shared_ptr<CyberSystemAgent> agent = std::dynamic_pointer_cast<CyberSystemAgent>(actors[i]);
			if (agent)
			{
				agent->SetLoopDetectionEnabled(deadlockDetectionEnabled);
				if (params.LearningConfiguration)
				{
					agent->SetAgentConfiguration(params.LearningConfiguration);
				}
				agent->SetLearningEnabled(true);
				agent->SetLogEnabled(false);
				//assistants[i]->SetLogEnabled(true);
				//agent->SetLevel(ExperienceLevel::TRAINEE);
				agents.push_back(agent);
			}
		}

		bool multiAgent = agents.size() > 1;

		int episodeCount = 0;
		int deadlockCount = 0;
		int iterations = learningIterations + trainingIterations;
		std::string progressBar = "[                ]";
		int progDiv = 100 / ((int)progressBar.size() - 2);
		int bestPerformance = -1;

		auto printAgentStats = [&episodeCount, &deadlockCount](std::shared_ptr<CyberSystemAgent> agent)
		{
			int successCount = agent->GetStatistics().SuccessCount;
			int failureCount = agent->GetStatistics().FailedCount;
			int episodeCount = successCount + failureCount + deadlockCount;
			int choiceCount = agent->GetStatistics().ActionChoiceCount;
			int randomCount = agent->GetStatistics().ExplorationActionCount;
			std::cout
				<< "___________________________________________________\n"
				<< "\n"
				<< "Training completed after " << episodeCount << " episodes.\n\n"
				<< std::setprecision(1) << std::fixed
				<< "Succeeded: " << successCount << " (" << (episodeCount > 0 ? successCount * 100.0f / episodeCount : 0) << "%).\n"
				<< "Failed: " << failureCount << " (" << (episodeCount > 0 ? failureCount * 100.0f / episodeCount : 0) << "%).\n"
				<< "Deadlock: " << deadlockCount << " (" << (episodeCount > 0 ? deadlockCount * 100.0f / episodeCount : 0) << "%).\n"
				<< "Examined states: " << agent->GetStatistics().StatesCount << ".\n"
				<< "Actions choices: " << choiceCount << ".\n";
			if (randomCount>=0)
			{
				std::cout << "Exploration actions: " << randomCount << " (" << (choiceCount > 0 ? randomCount * 100.0f / choiceCount : 0) << "%).\n";
			}
			std::cout << std::endl;
		};

		auto printStats = [&]()
		{
			for (size_t i = 0; i < agents.size(); i++)
			{
				printAgentStats(agents[i]);
			}
		};

		std::ostringstream oStr;
		std::string lastMsg;

		auto clearLine = [&]()
		{
			std::cout << std::setfill(' ') << std::setw(lastMsg.size() + 1) << " \r";
		};

		std::cout
			<< "___________________________________________________\n"
			<< "Agent training started\n";
		int successCount = 0;
		for (int iterIdx = 0; iterIdx < iterations; iterIdx++)
		{

			if (iterIdx == learningIterations)
			{
				clearLine();
				if (!extendUntilSuccess || successCount > 0)
				{
					printStats();
					std::cout
						<< "___________________________________________________\n"
						<< "Experience training started\n";
					//assistant->SetLogEnabled(true);

					for (size_t t = 0; t < agents.size(); t++)
					{
						agents[t]->ResetStats();
						agents[t]->SetLearningEnabled(false);
					}
					episodeCount = 0;
					successCount = 0;
					deadlockCount = 0;

					if (multiAgent)
					{
						sharedArena->NewEpisode(agents[0]);
					}
				}
				else
				{
					// extend the learning phase if none of the episodes was successful
					if (outputInProgressEpisodes)
					{
						std::cout
							<< "\nNo success, extending learning phase..."
							<< std::endl;
					}
					learningIterations += learningIterations / 2;
					iterations = learningIterations + trainingIterations;
				}
			}

			// update the progress bar
			int perc = (iterIdx < learningIterations) ? iterIdx * 100 / learningIterations : (iterIdx - learningIterations) * 100 / trainingIterations;
			for (int j = 1; j < (int)progressBar.size() - 1; j++)
			{
				progressBar[j] = perc / progDiv >= j ? '.' : ' ';
			}
			bool trainingXp = (iterIdx >= learningIterations);
			bool learningPhase = (iterIdx < learningIterations);
			bool enableOutput = (outputOnlyTraining && trainingXp) || !outputOnlyTraining;

			ActionResult result = ActionResult::IN_PROGRESS;
			bool completedEp = false;

			// // start randomly
			//size_t startIndex = RandIndex(assistants.size());

			//size_t currIndex = startIndex;

			// loop until an episode is completed (success, failure, deadlock)
			while (!completedEp)
			{
				bool someoneCompleted = false;
				//size_t currIndex = startIndex;
				for (size_t currIndex = 0; currIndex < agents.size(); currIndex++)
				{

					// stop the training phase if is the last iteration
					// and the current episode is completed
					if (trainingXp && iterIdx == trainingIterations && completedEp)
					{
						break;
					}

					//size_t currIndex = (n + startIndex) % assistants.size();
					//currIndex = (currIndex + 1) % assistants.size();
					std::shared_ptr<CyberSystemAgent>& agent = agents[currIndex];
					ActionResult prevResult = result;
					agent->SetLearningEnabled(learningPhase);
					result = agent->Train(trainingXp || (trainingIterations == 0));
					bool newEp = agent->IsNewEpisode();

					int perf = agent->GetLastEpisodePerformance();
					if (learningPhase && result == ActionResult::SUCCEEDED && bestPerformance < perf)
					{
						bestPerformance = perf;
					}

					if (enableOutput && outputInProgressEpisodes)
					{
						clearLine();
						if (newEp)
						{
							std::cout << "\n" << agent->GetRoleName()
								//<< "  " << ActionResultToString(result)
								<< " - New episode " << std::to_string(episodeCount + 1) << std::endl;
						}
						if (multiAgent)
						{
							std::cout << agent->GetCurrentGoal() << std::endl;
						}
						std::cout << agent->GetSystemInfo() << std::endl;
					}

					if (result != ActionResult::IN_PROGRESS && prevResult != result)
					{
						completedEp = true;
						if (result == ActionResult::SUCCEEDED)
						{
							successCount++;
						}

						// output statistics

						bool outputInfo = false;
						if (enableOutput && (outputSuccessEpisodes || outputFailedEpisodes))
						{
							clearLine();
							for (size_t t = 0; t < agents.size(); t++)
							{
								ActionResult res = agents[t]->LastEpisodeResult();
								if ((outputSuccessEpisodes && res == ActionResult::SUCCEEDED)
									|| (outputFailedEpisodes && (res == ActionResult::FAILED || res == ActionResult::DEADLOCK)))
								{
									outputInfo = true;
								}
							}
							if (outputInfo)
							{
								if (multiAgent)
								{
									std::cout << "Ep." << episodeCount + 1 << "  ";
									for (size_t t = 0; t < agents.size(); t++)
									{
										ActionResult res = agents[t]->LastEpisodeResult();
										if ((outputSuccessEpisodes && res == ActionResult::SUCCEEDED)
											|| (outputFailedEpisodes && res == ActionResult::FAILED || res == ActionResult::DEADLOCK))
										{
											std::cout << agents[t]->GetCurrentGoal() << ": " << ActionResultToString(res) << "  ";
										}
									}
									std::cout << std::endl;
								}
								if (!outputInProgressEpisodes)
								{
									if (!multiAgent)
									{
										if (!learningPhase && agent->LastEpisodeResult() == ActionResult::FAILED)
										{
											std::cout << "!>";
										}
										if (agent->LastEpisodeResult() == ActionResult::DEADLOCK)
										{
											std::cout << ">";
										}
										std::cout << ActionResultToString(agent->LastEpisodeResult()) << " (" << agent->GetLastEpisodePerformance() << "): ";
									}
									std::cout << agent->GetSystemInfo() << std::endl;
								}
								std::cout << std::endl;
							}
						}
					} // end if (result != ActionResult::IN_PROGRESS)

					// after each completed episode output a summary for all the agents
					if (completedEp)
					{

						// prevent double increment
						if (!someoneCompleted)
						{
							if (result == ActionResult::DEADLOCK) deadlockCount++;
							episodeCount++;
						}
						someoneCompleted = true;

						oStr.str("");
						oStr << std::setw(9) << ActionResultToString(result) << std::setw(0);
						oStr << " " << (trainingXp ? iterIdx - learningIterations : iterIdx)
							<< "/" << (trainingXp ? trainingIterations : learningIterations)
							<< " Success=";
						for (size_t t = 0; t < agents.size(); t++)
						{
							if (t > 0) oStr << ";";
							oStr << (agents[t]->GetStatistics().SuccessCount);
						}
						if (learningPhase && bestPerformance>0)
						{
							oStr << " Max perf=" << bestPerformance;
						}
						oStr << " Failed=";
						for (size_t t = 0; t < agents.size(); t++)
						{
							if (t > 0) oStr << ";";
							int failedCount = agents[t]->GetStatistics().FailedCount;
							oStr << failedCount;
						}
						oStr << " Deadlock=" << deadlockCount;
						oStr << " States:";
						for (size_t t = 0; t < agents.size(); t++)
						{
							if (t > 0) oStr << ";";
							oStr << agents[t]->GetStatistics().StatesCount;
						}
						oStr << " " << progressBar;
						lastMsg = oStr.str();
						std::cout << lastMsg << "\r";
					}

				} // end for

			} // end while


			//// extend the training phase if none of the episodes was successful
			//if (trainingXp && iterIdx >= TrainingIterations && successCount == 0)
			//{
			//	//std::cout
			//	//	<< "\nNo success, extending training phase..."
			//	//	<< std::endl;
			//	TrainingIterations += TrainingIterations / 2;
			//	iterations = LearningIterations + TrainingIterations;
			//}
		}

		clearLine();
		printStats();
	}




	void TestXp(const std::shared_ptr<CyberSystemAssistant> assistant)
	{
		std::cout
			<< "\n"
			<< "___________________________________________________\n"
			<< "\n"
			<< "Testing stored experience.\n"
			<< "___________________________________________________\n"
			<< std::endl;
		assistant->SetLogEnabled(true);
		assistant->SetLevel(ExperienceLevel::TRAINER);
		assistant->NewEpisode();
		std::vector<Action> suggestedActions;
		ActionResult result = ActionResult::IN_PROGRESS;
		while (result != ActionResult::SUCCEEDED)
		{
			assistant->PrintHints();
			if (!assistant->GetSuggestedActions(suggestedActions)) break;
			result = assistant->TakeAction(suggestedActions[0],false);
		}
		std::cout << assistant->GetSystemInfo() << std::endl;
	}


	void TestXpSerialization(std::shared_ptr<CyberSystemAssistant> assistant, const std::string& fileName)
	{
		std::string json;
		std::string json2;
		assistant->SerializeExperience(json);
		assistant->ClearAllExperiences();
		assistant->ParseExperience(json);
		assistant->SerializeExperience(json2);
		if (json2 != json)
		{
			std::cout << "Serialization error:" << std::endl;
			std::cout << DiffString(json, json2) << std::endl;
		}
		else
		{
			std::cout << "Serialization OK." << std::endl;
		}
		if (!fileName.empty())
		{
			if (
				assistant->SaveExperience(fileName, "", true) &&
				assistant->LoadExperience(fileName, true)
				)
			{
				std::cout << "Save/Load OK." << std::endl;
			}
		}
	}


	void PrintTestDuration(std::chrono::time_point<std::chrono::system_clock> testStart)
	{
		auto diff = std::chrono::system_clock::now() - testStart;
		auto min = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		auto sec = seconds % 60;
		auto ms = millis % 1000;
		std::cout << "Test duration: " << std::setfill('0')
			<< std::setw(2) << min << ":" << std::setw(2) << sec << "."
			<< std::setw(3) << ms << std::setw(0) << std::endl;
	}


	void PrintAvailableActions(std::shared_ptr<CyberSystemAssistant> assistant)
	{
		std::vector<ActionRef> possibleActions;
		std::cout << "Available actions: ";
		possibleActions = assistant->GetAvailableActions();
		for (const auto& action : possibleActions)
		{
			std::cout << action->ToString() << "\t";
		}
		std::cout << std::endl;
	};

}
