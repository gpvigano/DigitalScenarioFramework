## Digital Scenario Framework Tutorial ##


### Building the framework

This repository includes projects for some compilers, see *[How to build Digital Scenario Framework](./HOW_TO_BUILD.md)* to learn how to build the framework and its dependencies.

### Running the examples

To run the examples in the IDE:

* [Visual Studio] - open the solution in the root folder of this repository, *Set as StartUp Project* the *DiScenFwTest* project and *Start Without Debugging* or *Start Debugging*;
* [Code::Blocks] - open the workspace in the root folder of this repository, *Activate* the *DiScenFwTest* project and *Run* or *Start/Continue*.

A console will open and a menu will be displayed. You can choose an example pressing the corresponding number followed by *enter*, then you have to choose the running mode in a a similar way.

### About the projects in this repository

The core of *Digital Scenario Framework* API is implemented in the *DiScenFw* project. This project includes a singleton is provided to simplify the access to the framework API (class *DigitalScenarioFramework*). This singleton provides an abstraction layer that offers a simplified management (scenario, simulation, agents, configuration) and hides some lower level operations (initialization, error checking, etc.).

In the *TestGridworld* example of *DiScenFwTest* project you can see how to access the *DigitalScenarioFramework* methods. The *TestGridworld* example depends on two other projects: the *Gridworld* project, that demonstrates how to build cyber system plugin, and the *SampleAgent* project, a custom RL agent implementation with a custom configuration.

The other examples are based on a lower level access to the framework API. In particular *TicTacToe* example is a test for multi-agent scenario, a classical example of a zero-sum game with perfect information.
*LedCircuit* example shows how a task can be learned and it is replicated in [other projects](./README.md/#related-projects).

### Building blocks

To build an application based on *DigitalScenarioFramework* you need to use
- the framework library (*DiScenFw*);
- a cyber system plugin that implements *CyberSystemPlugin* interface (see *SampleCybSys* project);
- optionally a custom agent implementation that implements *AgentPlugin* interface, if you want to replace the default *RLAgent* (a built-in RL agent implementation), and optionally a custom agent configuration implementing the *IAgentConfiguration* interface (see *SampleAgent* project).

### Building a console application

*SampleCybSys* project can be used as a template to implement new cyber system plugins. There you can an explanation in code comments of each overridden method.

To build your application you need to include the following header files in your source code:
```cpp
#include <DiScenFw/DigitalScenarioFramework.h>// using the framework API
#include <DiScenFw/interop/CyberSystemLink.h> // loading your cyber system plugin
#include <DiScenFw/interop/AgentLink.h> // loading your custom agent
//include "SampleAgentConfig.h" // using your custom agent configuration
// or using the default agent configuration:
//#include <DiScenFw/RL/RLConfig.h>

using namespace discenfw; // this can be optionally done to simplify your code.
```

#### Initialization

The first call to `DiScenFw()` initializes the API and creates the singleton (we assume you are using `discenfw` namespace), so following calls just operate on that singleton.
```cpp
// Load your cyber system:
// (replace 'YourCyberSystem' with the name of your cyber system)
DiScenFw()->LoadCyberSystem("YourCyberSystem");

// Create an agent
// (replace 'YourAgentId' with a name used to identify your agent
// in the following operations; replace 'YourRole' with the role defined
// in YourCyberSystem):
// DiScenFw()->CreateAgent("YourAgentId","YourRole","");
// or load your custom agent implementation 'YourAgentImplementation':
DiScenFw()->CreateAgent("YourAgentId","YourRole","",[]()
		{
			std::shared_ptr<AgentLink> agentLink = std::make_shared<AgentLink>();
			agentLink->LoadAgentPlugin("YourAgentImplementation");
			return agentLink;
		}
	);

// Set a goal for 'YourAgentId'
DiScenFw()->SetCurrentGoal("YourAgentId","Your goal");

// Configure your cyber system with a suitable configuration string
std::string config = "...";
DiScenFw()->SetSystemConfiguration(config);

// Create a proper configuration for your custom agent
auto learningConfig = std::make_shared<sample_agent::SampleAgentConfig>();
// or use the default configuration for RLAgent
// auto learningConfig = std::make_shared<RLConfig>();

// Set configuration parameters...

DiScenFw()->SetAgentConfiguration("YourAgentId", learningConfig);

```

#### Training the agent

A *Cyber System Agent* can be trained in two ways:
* **automatically** (method `Train()`): the agent uses a policy to take actions autonomously, and learns thanks to a RL algorithm;
* "**manually**", or user guided (method `TakeAction()`): the agent take an action (generated by a program or by the user input) and learns observing the actions outcomes.

```cpp

// Store here the information about the outcome of each action
ActionOutcome outcome;

// The agent experience is updated and episodes are stored if true.
bool updateExperience = false;

// just train the agent without updating its experience
for(int i=0; i<100; i++)
{
	outcome = DiScenFw()->Train("YourAgentId", updateExperience, AgentMode::LEARN);
	// ... (manage the action outcome, switch on its result) 
}

// Train the agent updating its experience.
updateExperience = true;
for(int i=0; i<100; i++)
{
	outcome = DiScenFw()->Train("YourAgentId", updateExperience, AgentMode::LEARN);
	// ...
}

// An action recognized by YourCyberSystem (replace 'YourValidAction')
Action aValidAction("YourValidAction");

// To "teach by showing" you can call the TakeAction() methods.
outcome = DiScenFw()->TakeAction("YourAgentId", aValidAction);

// You can also print information about YourCyberSystem, if available:
// std::cout << DiScenFw()->GetSystemInfo();

```
#### Exploiting the assistant

Each *CyberSystemAgent* is also a *DigitalAssistant* that can suggest a sequence of successful actions and can warn about forbidden actions.

```cpp
// get a list of available actions
const std::vector<ActionRef>& availableActions = DiScenFw()->GetAvailableActions("YourAgentId");

// get a list of available actions filtered by the cyber system heuristics
const std::vector<ActionRef>& availableSmartActions = DiScenFw()->GetAvailableActions("YourAgentId", true);

// get a list of suggested actions
const std::vector<ActionRef>& suggestedActions = DiScenFw()->GetSuggestedActions("YourAgentId");

// get a list of forbidden actions
const std::vector<ActionRef>& forbiddenActions = DiScenFw()->GetForbiddenActions("YourAgentId");

```


**(More information coming in future.)**

[Visual Studio]: https://visualstudio.microsoft.com/
[Code::Blocks]: https://www.codeblocks.org/
