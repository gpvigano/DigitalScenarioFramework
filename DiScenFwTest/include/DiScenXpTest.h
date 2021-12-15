//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFw/RL/RLConfig.h>
#include <DiScenFw/xp/CyberSystemAgent.h>
#include <chrono>

/*! @mainpage Digital Scenario Framework Tests
* @author Giovanni Paolo Vigano'
* @author <b>CNR-STIIMA</b>
* @author <i>Consiglio Nazionale delle Ricerche -
* @author Istituto di Sistemi e Tecnologie Industriali Intelligenti per il Manifatturiero Avanzato</i>
* @version 1.0
* @date 2020
*
[Digital Scenario Framework](https://github.com/gpvigano/DigitalScenarioFramework)
is a programming interface for digital scenarios, designed to study the collaboration
between human and artificial intelligence, applied to a generic digital scenario.

This project is a collection of tests to explore and test the framework in a console based application.
*/

namespace discenfw_test
{
	using namespace discenfw;

	struct TrainingParameters
	{
		int LearningIterations = 0; // Number of iterations in the learning phase.
		int TrainingIterations = 0; // Number of iterations in the training phase.
		bool OutputSuccessEpisodes = true; // Print info about successful episodes.
		bool OutputFailedEpisodes = false; // Print info about failed episodes.
		bool OutputInProgressEpisodes = false; // Print info about intermediate episode steps.
		bool DeadlockDetectionEnabled = true; // Scan previous states to find deadlock situations.
		bool OutputOnlyTraining = false; // Print info only in the training phase
		bool ExtendUntilSuccess = false; // Extend the learning phase until a successful episode is completed.
		std::shared_ptr<xp::RLConfig> LearningConfiguration;

		TrainingParameters(
			int learningIterations, // Number of iterations in the learning phase.
			int trainingIterations, // Number of iterations in the training phase.
			bool outputSuccessEpisodes = true, // Print info about successful episodes.
			bool outputFailedEpisodes = false, // Print info about failed episodes.
			bool outputInProgressEpisodes = false, // Print info about intermediate episode steps.
			bool deadlockDetectionEnabled = true, // Scan previous states to find deadlock situations.
			bool outputOnlyTraining = false, // Print info only in the training phase
			bool extendUntilSuccess = false, // Extend the learning phase until a successful episode is completed.
			const std::shared_ptr<xp::RLConfig> learningConfiguration = nullptr
			) :
			LearningIterations(learningIterations),
			TrainingIterations(trainingIterations),
			OutputSuccessEpisodes(outputSuccessEpisodes),
			OutputFailedEpisodes(outputFailedEpisodes),
			OutputInProgressEpisodes(outputInProgressEpisodes),
			DeadlockDetectionEnabled(deadlockDetectionEnabled),
			OutputOnlyTraining(outputOnlyTraining),
			ExtendUntilSuccess(extendUntilSuccess),
			LearningConfiguration(learningConfiguration)
		{
		}
	};

	// Train an agent and update its experience.
	void TestLearner(std::shared_ptr<xp::CyberSystemAgent> assistant, TrainingParameters params);

	// Train agents and update their experience in a shared arena.
	void TestLearners(std::shared_ptr<xp::SharedArena> sharedArena, TrainingParameters params);

	// Test the experience of an assistant using its suggestions.
	void TestXp(const std::shared_ptr<xp::CyberSystemAssistant> assistant);

	// Test the experience serialization on a file (check if output matches input).
	void TestXpSerialization(std::shared_ptr<xp::CyberSystemAssistant> assistant, const std::string& fileName);

	// Save the current time instant.
	inline std::chrono::time_point<std::chrono::system_clock> GetTimeNow() { return std::chrono::system_clock::now(); }

	// Use the previously saved time instant to compute the elapsed time and print out test duration.
	void PrintTestDuration(std::chrono::time_point<std::chrono::system_clock> testStart);

	// Print a list of available actions for the given assistant.
	void PrintAvailableActions(std::shared_ptr<xp::CyberSystemAssistant> assistant);

}
