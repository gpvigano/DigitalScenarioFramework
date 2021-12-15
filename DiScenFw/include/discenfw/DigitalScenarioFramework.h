//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFwConfig.h>

//#   if defined(_MSC_VER)
//#     pragma warning(push) // Save warning settings
//#     pragma warning(disable : 4251) // disable warning: class ... needs to have dll-interface...
//#   endif

#include <discenfw/scen/CatalogItem.h>
#include <discenfw/xp/Action.h>
#include <discenfw/xp/Condition.h>
#include <discenfw/xp/ExperienceLevel.h>
#include <discenfw/xp/StateRewardRules.h>
#include <discenfw/RL/RLConfig.h>
#include <discenfw/xp/AgentStats.h>
#include <discenfw/xp/ref.h>

#include <memory>
#include <string>
#include <map>

/// @cond DOXYGEN_IGNORE

// Forward declarations

namespace discenfw
{

	class Catalog;
	class Entity;
	class Element;
	class Scenario;

	namespace scen
	{
		class ScenarioManager;
	}
	namespace sim
	{
		class SimulationManager;
		class SimulationController;
	}
	namespace xp
	{
		class EnvironmentModel;
		class EnvironmentState;
		class SharedArena;
		class CyberSystemAgent;
		class CyberSystemLink;
		class IAgent;
		class IAgentConfiguration;
	}
}

/// @endcond



namespace discenfw
{

	namespace xp
	{
		/*!
		Agent mode: the way an agent can behave when acting.
		*/
		enum AgentMode
		{
			JUST_ACT = 0x0, //!< Just do actions.
			LEARN = 0x1, //!< Let the agent learn.
			LOOP_DETECTION = 0x2, //!< Detect loops in actions sequence.
			DEFAULT = LEARN | LOOP_DETECTION //!< Let the agent learn and detect loops.
		};

		/*!
		Errors that can occur when trying to do an action.
		*/
		enum ActionError
		{
			NO_ERROR = 0x0, //!< No error occurred.
			AGENT_NOT_FOUND, //!< The needed agent does not exist.
			NO_ACTION_FOUND, //!< No action was found.
			UNKNOWN_ERROR, //!< An unknown error occurred.
		};

		/*!
		Outcome of an action and information about the consequent state.
		*/
		struct ActionOutcome
		{
			xp::ActionResult Result; //!< Result of the last action.
			bool MultiAgent = false; //!< More agents involved.
			bool NewEpisode = false; //!< The last action started a new episode.
			bool CompletedEpisode = false; //!< The last action completed an episode.
			bool Stuck = false; //!< No more actions available.
			ActionError Error = ActionError::NO_ERROR; //!< Error occurred (or no error).
			int Reward = 0; //!< Reward for the new state.
			int Performance = 0; //!< Episode performance for a successful episode (0 otherwise).

			ActionOutcome()
				: Result(), MultiAgent(false), NewEpisode(false), CompletedEpisode(false), Stuck(false), Error(ActionError::NO_ERROR), Reward(0), Performance(0)
			{
			}

			ActionOutcome(ActionError err) : Error(err)
			{
			}

			ActionOutcome(xp::ActionResult result, bool multiAgent, bool newEpisode, bool completedEpisode, bool stuck, int reward, int performance)
				: Result(result), MultiAgent(multiAgent), NewEpisode(newEpisode), CompletedEpisode(completedEpisode), Stuck(stuck), Reward(reward), Performance(performance)
			{
			}
		};
	}


	/*!
	Digital Scenario Framework - Entry point for the overall framework.

	@details Unless you are using the same toolset
	and building settings as the framework,
	you must call only methods of this class from a C++ client application
	when linking this framework as dynamic link library.
	For linking this framework as a DLL from other languages (C, C#, ...)
	look at Interoperability module (DiScenFw.h,  DiScenAPI.h,  DiScenXp.h).
	*/
	class DISCENFW_API DigitalScenarioFramework
	{
	public:

		/*!
		Get a global instance to the whole framework manager.
		*/
		static std::shared_ptr<DigitalScenarioFramework> GetInstance();

		~DigitalScenarioFramework();


		/*!
		Load a cyber system implementation from the given dynamic link library.
		@param cyberSystemName name of the cyber system dynamic link library to be loade
		                         (with the DLL extension and -x32 or -x64 postfix omitted)
		*/
		bool LoadCyberSystem(const std::string& cyberSystemName);

		/*!
		Check if the cyber system implementation is loaded from the given dynamic link library.
		@param cyberSystemName name of the cyber system dynamic link library to be loade
		                         (with the DLL extension and -x32 or -x64 postfix omitted)
		*/
		bool IsCyberSystemLoaded(const std::string& cyberSystemName);

		/*!
		Check if any cyber system implementation is loaded from a dynamic link library.
		*/
		bool IsCyberSystemLoaded();

		/*!
		Get the configuration of the loaded system implementation.
		*/
		const std::string& GetSystemConfiguration();

		/*!
		Configure the loaded system implementation.
		@param configText configuration JSON text string
		*/
		bool SetSystemConfiguration(const std::string& configText);

		/*!
		Configure the given entity, both in cyber system and in the scenario data.
		@param entityId identifier of an existing entity.
		@param configText configuration JSON text string
		*/
		bool ConfigureEntity(const std::string& entityId, const std::string& configText);

		/*!
		Configure the cyber system with data from the loaded scenario.
		*/
		bool ConfigureSystemFromScenario();

		/*!
		Save the cyber system configuration to scenario data.
		*/
		bool SaveSystemConfigurationToScenario();


		/*!
		Get the model for the cyber system.
		*/
		const std::shared_ptr<xp::EnvironmentModel> GetSystemModel() const;


		/*!
		Create a catalog with the given name.
		@note If the catalog already exists it is replaced with a new one.
		@param catalogName name to be assigned to the catalog
		@return Return false if the name is empty.
		*/
		bool CreateCatalog(const std::string& catalogName);

		/*!
		Check if a catalog with the given name exists.
		*/
		bool CatalogExists(const std::string& catalogName);

		/*!
		Save a catalog to a JSON text file. Return true on success, false otherwise.
		@param catalogName name of an existing catalog
		@param jsonPath path to the JSON text file
		*/
		bool SaveCatalog(const std::string& catalogName, const std::string& jsonPath);

		/*!
		Load a JSON text file and create Catalog data. Return nullptr on error.
		@param catalogName name to be assigned to the catalog
		@param jsonPath path to the JSON text file
		*/
		const std::shared_ptr<Catalog> LoadCatalog(const std::string& catalogName, const std::string& jsonPath);

		/*!
		Get Catalog data.
		@param catalogName name of an existing catalog
		*/
		const std::shared_ptr<Catalog> GetCatalog(const std::string& catalogName) const;

		/*!
		Add an item to the catalog.
		@param catalogName name of an existing catalog
		@param item item data to be added to the given catalog
		@return Return false if the named catalog does not exist or if the item is invalid or duplicated.
		*/
		bool AddCatalogItem(const std::string& catalogName,const CatalogItem& item);

		/*!
		Set an abbreviation for the given category in the given catalog.

		If the abbreviation is an empty string the category abbreviation, if present, is removed.
		@param catalogName name of an existing catalog
		@param category category name to be abbreviated
		@param abbreviation abbreviation to be associated with the given category
		@return Return false if the named catalog does not exist or the category is empty.
		*/
		bool SetCatalogCategoryAbbreviation(
			const std::string& catalogName,
			const std::string& category,
			const std::string& abbreviation
		);


		/*!
		Load a JSON text file and fill Scenario data.
		@param jsonPath path to the JSON text file
		@param syncVe call the synchronization function of the Virtual Environment
		*/
		bool LoadScenario(const std::string& jsonPath, bool syncVe = false);

		/*!
		Write Scenario data to a JSON text file.
		@param jsonPath path to the JSON text file
		*/
		bool SaveScenario(const std::string& jsonPath, bool syncFromVe = false);


		/*!
		Create a sample scenario data (only for debugging).
		*/
		void CreateSampleData();


		/*!
		Load a JSON text file and fill history data.
		@param jsonPath path to the JSON text file
		*/
		bool LoadScenarioHistory(const std::string& jsonPath);

		/*!
		Write history data to a JSON text file.
		@param jsonPath path to the JSON text file
		*/
		bool SaveScenarioHistory(const std::string& jsonPath);


		/*!
		Access the simulation manager, create it if needed.
		*/
		std::shared_ptr<sim::SimulationManager> ScenarioSimulation();

		std::shared_ptr<sim::SimulationController> ScenarioSimulationController();

		const std::vector< std::shared_ptr<Entity> >& GetEntities();


		/*!
		Just check if the entity with the given identifier exists.
		*/
		bool EntityExists(const std::string& entityId);


		/*!
		Get the entity with the given identifier or nullptr if not found.
		*/
		const std::shared_ptr<Entity> GetEntity(const std::string& entityId);



		/*!
		Get the entity with the given identifier or nullptr if not found.
		*/
		const std::shared_ptr<Element> GetElement(const std::string& elementId);

		/*!
		Create an entity and add it to the scenario (ConfigureSystemFromScenario() must be called after configuring the entity).
		@param entityClass name of the class to create the proper entity.
		@param entityIdentity data used to create the entity in the scenario and to configure it in the system.
		@param updateXp update assistants' experience (if creation of entities is an allowed action).
		@return Return false if an entity with the given identifier already exists or if the class name is invalid.
		*/
		std::shared_ptr<Entity> CreateEntity(
			const std::string& entityClass,
			const EntityIdentity& entityIdentity,
			bool updateXp = false
			);

		/*!
		Update an entity (ConfigureSystemFromScenario() must be called after configuring the entity).
		@param entityData data used to update the entity in the scenario and to configure it in the system.
		@param updateXp update assistants' experience (if update of entities is an allowed action).
		@return Return false if an entity with an idenifier equal to the given entity does not exist.
		*/
		std::shared_ptr< Entity > UpdateEntityData(
			const EntityIdentity& entityData,
			bool updateXp = false
			);

		/*!
		Duplicate an entity and add it to the scenario (ConfigureSystemFromScenario() must be called after configuring the entity).
		@param entityId identifier of an existing entity.
		@param newEntityId identifier of the new entity (if empty it will be set to a variant of the original identifier).
		@param updateXp update assistants' experience (if creation of entities is an allowed action).
		@return Return a pointer to the new entity or an empty pointer if there is no entity with the given identifier or if an entity with the given new identifier already exists.
		*/
		std::shared_ptr<Entity> DuplicateEntity(
			const std::string& entityId,
			const std::string& newEntityId = "",
			bool updateXp = false
			);

		/*!
		Delete an entity of the scenario (ConfigureSystemFromScenario() must be called after changing the scenario).
		@param entityId Identifier of the entity to be deleted.
		@param updateXp update assistants' experience (if deletion of entities is an allowed action).
		@return Return false if the given entity does not exist.
		*/
		bool DeleteEntity(const std::string& entityId, bool updateXp = false);


		/*!
		Create a named CyberSystemAgent (it can be used as actor in a SharedArena).
		@param agentName name of the CyberSystemAgent
		@param roleName optional name of the agent's role (if empty the first role in the model is used)
		@param arenaName optional name of the arena the agent belongs to
		@param arenaName optional custom callback function used to create the internal agents
		@return Return false if the given agent already exists,
		if the specified arena does not exist or if a cyber system was not loaded.
		*/
		bool CreateAgent(
			const std::string& agentName,
			const std::string& roleName = "",
			const std::string& arenaName = "",
			std::function<std::shared_ptr<xp::IAgent>()> makeAgentCallback = nullptr
			);

		/*!
		Check if the named agent exists.
		@param agentName name of the CyberSystemAgent
		*/
		bool AgentExists(const std::string& agentName) const;

		/*!
		Set the current goal for the named agent.
		@param agentName name of the CyberSystemAgent
		@param goalName name of the goal to be selected
		@return Return false if the given agent does not exist.
		*/
		bool SetCurrentGoal(const std::string& agentName, const std::string& goalName);

		/*!
		Rename the current goals (internal mappings are updated).
		@param agentName name of the CyberSystemAgent
		@param newGoalName new name for the current goal
		@return Return false if the given agent does not exist or if the new name is not valid.
		*/
		bool RenameCurrentGoal(const std::string& agentName, const std::string& newGoalName);

		/*!
		Get the current goal for the named agent.
		@param agentName name of the CyberSystemAgent
		@return Return an empty string if the given agent does not exist.
		*/
		const std::string& GetCurrentGoal(const std::string& agentName) const;

		/*!
		Add a new goal with the given name.
		@param agentName name of the CyberSystemAgent
		@param newGoalName name for the new goal
		@return Return false if the given agent does not exist or if the new name is not valid.
		*/
		bool AddNewGoal(const std::string& agentName, const std::string& newGoalName);

		/*!
		Remove the current goal (only if there are other goals defined).
		@param agentName name of the CyberSystemAgent
		@param goalName new name for the current goal
		@return Return false if the given agent does not exist or if the new name is not valid.
		*/
		bool RemoveGoal(const std::string& agentName, const std::string& goalName);


		/*!
		Get the goals list for the named agent.
		@param agentName name of the CyberSystemAgent
		@return Return an empty string if the given agent does not exist.
		*/
		const std::vector<std::string>& GetGoals(const std::string& agentName) const;


		/*!
		Get the model name for the named agent.
		@param agentName name of the CyberSystemAgent
		@return Return an empty string if the given agent does not exist.
		*/
		const std::string& GetAgentModel(const std::string& agentName) const;


		/*!
		Get the role name of the named agent.
		@param agentName name of the CyberSystemAgent
		@return Return an empty string if the given agent does not exist.
		*/
		const std::string& GetAgentRole(const std::string& agentName) const;

		/*!
		Set the role for the given agent (it is created if it does not exist).
		@param agentName name of the CyberSystemAgent
		@param roleName name of the role
		@return Return false if the given agent does not exist or the role name is empty.
		*/
		bool SetAgentRole(const std::string& agentName, const std::string& roleName);


		/*!
		Set the success condition for the given agent (failure condition is evaluated before).
		@param agentName name of the CyberSystemAgent
		@param successCondition success condition to be set
		*/
		bool SetSuccessCondition(const std::string& agentName, const xp::Condition& successCondition);

		/*!
		Get the success condition for the given agent for the current goal.
		@param agentName name of the CyberSystemAgent
		@return Return the success condition or an empty condition if the given agent does not exist.
		*/
		const xp::Condition& GetSuccessCondition(const std::string& agentName) const;

		/*!
		Set the failure condition for the given agent (failure condition is evaluated before success condition).
		@param agentName name of the CyberSystemAgent
		@param failureCondition failure condition to be set
		@return Return false if the given agent does not exist.
		*/
		bool SetFailureCondition(const std::string& agentName, const xp::Condition& failureCondition);

		/*!
		Get the failure condition for the given agent for the current goal.
		@param agentName name of the CyberSystemAgent
		@return Return the failure condition or an empty condition if the given agent does not exist.
		*/
		const xp::Condition& GetFailureCondition(const std::string& agentName) const;


		/*!
		Set ignored/considered the system failure conditions for the given agent.
		@param agentName name of the CyberSystemAgent
		@param ignored system failure conditions ignored
		@return Return false if the given agent does not exist.
		*/
		bool SetSystemFailureIgnored(const std::string& agentName, bool ignored);

		/*!
		Check if the system failure conditions for the given agent are ignored.
		@param agentName name of the CyberSystemAgent
		@return Return true if ignored, false if not or if the given agent does not exist.
		*/
		bool IsSystemFailureIgnored(const std::string& agentName);


		/*!
		Set the state reward rules for the given agent, computed according to a set of conditions.
		@param agentName name of the CyberSystemAgent
		@param stateReward scenario state reward
		@return Return false if the given agent does not exist.
		*/
		bool SetRewardRules(const std::string& agentName, const xp::StateRewardRules& stateReward);

		/*!
		Get the state reward rules for the given agent for the current goal.
		@param agentName name of the CyberSystemAgent
		@return Return the scenario reward or an empty scenario reward if the given agent does not exist.
		*/
		const xp::StateRewardRules& GetRewardRules(const std::string& agentName) const;


		/*!
		Set the discounting constant for the current experience of the given agent.
		@param agentName name of the CyberSystemAgent
		@param discountingConstant Discounting constant (0..1, if set to a negative value it is automatically computed)
		@return Return false if the given agent does not exist.
		*/
		bool SetDiscountingConstant(const std::string& agentName, float discountingConstant);

		/*!
		Get the discounting constant for the current experience of the given agent.
		@param agentName name of the CyberSystemAgent
		@return Return the discounting constant or -2 if the given agent does not exist.
		*/
		float GetDiscountingConstant(const std::string& agentName) const;


		/*!
		Create a named SharedArena with a list of named assistants (CyberSystemAgent).
		@param[in] name name of the shared arena
		@param[in] assistantNames names of the assistants to be included in the given arena
		@return Return false if the given arena already exists or if an actor in the list does not exists.
		*/
		bool CreateSharedArena(const std::string& name, const std::vector<std::string>& assistantNames);


		/*!
		Get the list of actors (CyberSystemAgent) of a named shared arena.
		@param[in] name name of the shared arena
		@param[out] assistantNames names of the CyberSystemAgent included in the given arena
		@return Return false if the given arena does not exist.
		*/
		bool GetSharedArenaActors(const std::string& name, std::vector<std::string>& assistantNames) const;


		/*!
		Start a new episode with the initial state of the previous one (if any).
		@param agentName name of the CyberSystemAgent
		@return Return false if the given agent does not exist.
		*/
		bool NewEpisode(const std::string& agentName);


		/*!
		Get the last stored scenario state from the cyber system.
		*/
		const std::shared_ptr<xp::EnvironmentState> GetLastSystemState() const;

		/*!
		Get the required system information from the cyber system.
		*/
		const std::string& GetSystemInfo(const std::string& infoId = "") const;

		/*!
		Get the last stored scenario state for the current episode.
		@param agentName name of the CyberSystemAgent
		*/
		const std::shared_ptr<xp::EnvironmentState> GetLastScenarioState(const std::string& agentName) const;


		/*!
		Get the last stored result for the current episode.
		@param agentName name of the CyberSystemAgent
		*/
		const xp::ActionOutcome GetLastActionOutcome(const std::string& agentName) const;


		/*!
		Get from the given agent a list of successful actions for the current state (consulting the current experience).
		@param agentName name of the CyberSystemAgent
		*/
		const std::vector<xp::Action>& GetSuggestedActions(const std::string& agentName) const;

		/*!
		Get from the given agent a list of failing actions for the current state (consulting the current experience).
		@param agentName name of the CyberSystemAgent
		*/
		const std::vector<xp::Action>& GetForbiddenActions(const std::string& agentName) const;

		/*!
		Get a list of available actions for the current state (consulting the current experience).
		@param roleId role identifier: actions can be different for different roles in some cyber systems
		@param smartSelection select only actions (if known) that make sense (using heuristics to prune the decision tree),
				e.g. in particular states select only actions that lead to an immediate success or prevent an immediate failure
		       (if the current cyber system does not support this feature, all available actions are returned)
		*/
		const std::vector<xp::ActionRef>& GetAvailableActions(
			const std::string& roleId = "",
			bool smartSelection = false) const;

		/*!
		Check if the current episode has at least an action for the given agent.
		@param agentName name of the CyberSystemAgent
		*/
		const bool EpisodeStarted(const std::string& agentName) const;

		/*!
		Get the list of actions done by the given agent (consulting the current experience).
		@param agentName name of the CyberSystemAgent
		*/
		const std::vector<xp::Action>& GetActionsSequence(const std::string& agentName) const;


		/*!
		Execute a single training step. The relative learning agent is automatically created at the first call.
		@param agentName name of the CyberSystemAgent
		@param updateXp update experience with DigitalAssistant
		@param rlMode Mode for RL agent learning (default is: let the agent learn and detect loops)
		@return The last action outcome (ActionOutcome)
		@see discenfw::xp::ActionOutcome, ActionResult::DEADLOCK
		*/
		xp::ActionOutcome Train(const std::string& agentName, bool updateXp, xp::AgentMode rlMode = xp::AgentMode::DEFAULT);

		/*!
		Let the named agent take the given action.
		@param[in] agentName name of the CyberSystemAgent
		@param[in] action action to do (results are computed internally, this parameter is not updated)
		@param[in] updateXp update the agent's experience
		@param rlMode Mode for RL agent learning (default is: let the agent learn and detect loops)
		@return The last action outcome (ActionOutcome)
		@see discenfw::xp::ActionOutcome, ActionResult::DEADLOCK
		*/
		xp::ActionOutcome TakeAction(const std::string& agentName, const xp::Action& action, bool updateXp = true, xp::AgentMode rlMode = xp::AgentMode::DEFAULT);


		/*!
		Get the agent learning configuration parameters.
		@param agentName name of the CyberSystemAgent
		@return Return false if the given agent does not exist or the configuration.
		*/
		const std::shared_ptr<xp::IAgentConfiguration> GetAgentConfiguration(
			const std::string& agentName
			);

		/*!
		Set the agent learning configuration parameters.
		@param agentName name of the CyberSystemAgent
		@param[in] config configuration data
		@return Return false if the given agent does not exist.
		*/
		bool SetAgentConfiguration(
			const std::string& agentName,
			const std::shared_ptr<xp::IAgentConfiguration> config
			);


		/*!
		Load the agent learning configuration parameters (see RLConfig).
		@param agentName name of the CyberSystemAgent
		@param filePath path to the configuration file
		@return Return false if the given agent does not exist or if failed to load.
		*/
		bool LoadRLConfiguration(const std::string& agentName, const std::string& filePath);

		/*!
		Save and check the agent learning configuration parameters (see RLConfig).
		@param agentName name of the CyberSystemAgent
		@param filePath path to the configuration file
		@return Return false if the given agent does not exist or if failed to save.
		*/
		bool SaveRLConfiguration(const std::string& agentName, const std::string& filePath);


		/*!
		Get statistics related to the agent.
		@param agentName name of the CyberSystemAgent
		@return Return the agent statistics or empty statistics if the agent does not exist.
		*/
		xp::AgentStats GetAgentStats(const std::string& agentName);

		/*!
		Reset the statistics related to the agent.
		@param agentName name of the CyberSystemAgent
		@return Return false if the given agent does not exist.
		*/
		bool ResetAgentStats(const std::string& agentName);

		/*!
		Completely reset the agent.
		@param agentName name of the CyberSystemAgent
		@return Return false if the given agent does not exist.
		*/
		bool ResetAgent(const std::string& agentName);


		/*!
		Set the agent experience level for the current goal.
		@param agentName name of the CyberSystemAgent
		@param level Level to be assigned of the DigitalAssistant (see DigitalAssistant::SetLevel())
		@note The experience level is automatically promoted from NONE to TRAINEE
		when an episode is completed for the current goal.
		@return Return false if the given agent does not exist.
		*/
		bool SetLevel(const std::string& agentName, xp::ExperienceLevel level);

		/*!
		Get the agent experience level for the current goal.
		@param agentName name of the CyberSystemAgent
		@return Return the experience level, or ExperienceLevel::UNKNOWN if the given agent does not exist.
		*/
		xp::ExperienceLevel GetLevel(const std::string& agentName);


		/*!
		Load and deserialize the experience for the given agent from a JSON text file (previous experience is lost).
		@param agentName name of the CyberSystemAgent
		@param filePath path to the experience file
		@return Return false if failed to load, or if the given agent does not exist.
		*/
		bool LoadExperience(const std::string& agentName, const std::string& filePath);


		/*!
		Serialize and save the experience for the given agent to a JSON text file.
		@param agentName name of the CyberSystemAgent
		@param filePath Path to the JSON text file.
		@param goalName Optional name of goal, if not specified the current goal is used.
		@return Return false if failed to save, or if the given agent does not exist.
		*/
		bool SaveExperience(const std::string& agentName, const std::string& filePath, const std::string& goalName = "");


		/*!
		Optimize the stored experience for assistance (keep only best successful episodes and failures)
		@param agentName name of the CyberSystemAgent
		@return Return false if the given agent does not exist.
		*/
		bool OptimizeExperienceForAssistance(const std::string& agentName);

		/*!
		Clear the current experience for the given agent.
		@param agentName name of the CyberSystemAgent
		@return Return false if the given agent does not exist.
		*/
		bool ClearExperience(const std::string& agentName);

		/*!
		Clear the current experience for all assistants.
		*/
		void ClearAllExperiences();

		/*!
		Enable logging for the given agent, used for debugging.
		@param agentName name of the CyberSystemAgent
		@return Return false if the given agent does not exist.
		*/
		bool SetLogEnabled(const std::string& agentName, bool enabled);

		/*!
		Reset all scenario data.
		*/
		void ResetScenario();

		/*!
		Reset all experience data (all assistants and agents are destroyed).
		*/
		void ResetXp();

		/*!
		Reset the cyber system.
		*/
		void ResetCyberSystem();

		/*!
		Reset all scenario and experience data.
		*/
		void ResetAll();


	private:

		/*!
		Digital Scenario Framework API singleton.
		*/
		static std::shared_ptr<DigitalScenarioFramework> Instance;

		///@name Persistent invalid data used to return a valid reference from some methods
		///@{

		static xp::StateRewardRules EmptyStateReward;
		static std::string EmptyString;
		static std::vector<std::string> EmptyStringVector;
		static std::vector<xp::Action> EmptyActionVector;
		static std::vector<xp::ActionRef> EmptyActionRefVector;
		static xp::Condition EmptyCondition;

		///@}

		/*!
		Link to a cyber system plugin.
		*/
		std::shared_ptr<xp::CyberSystemLink> CyberSystem;

		mutable std::string LastSystemInfo;
		mutable std::string LastSystemConfig;

		/*!
		Index of created agents.
		*/
		std::map< std::string, std::shared_ptr<xp::CyberSystemAgent> > CyberSystemAgents;

		/*!
		Index of shared arenas.
		*/
		std::map< std::string, std::shared_ptr<xp::SharedArena> > SharedArenas;

		std::shared_ptr<scen::ScenarioManager> DigitalScenarioManager;
		std::shared_ptr<sim::SimulationManager> ScenarioSimulationManager;

		/*!
		Current shared arena.
		*/
		std::shared_ptr<xp::SharedArena> CurrentSharedArena;

		/*!
		Index of catalogs.
		*/
		std::map< std::string, std::shared_ptr<Catalog> > Catalogs;


		/*!
		Index of action outcomes for each agent.
		*/
		std::map< std::string, xp::ActionOutcome > CachedLastActionOutcome;

		/*!
		Constructor used to create a singleton.
		*/
		DigitalScenarioFramework();

		/*!
		Create a cyber system link.
		*/
		void InitCyberSystem();

		/*!
		Check if the cyber system is loaded, if not an error message is displayed.
		*/
		bool CheckCyberSystem() const;

		/*!
		Initialize the digital scenario.
		*/
		void InitDigitalScenario();

		/*!
		Initialize the simulation.
		*/
		void InitScenarioSimulation();

		/*!
		Access the scenario manager, create it if needed.
		*/
		std::shared_ptr<scen::ScenarioManager> DigitalScenario();


		/*!
		Access the scenario data, create it if needed.
		*/
		std::shared_ptr<Scenario> ScenarioData();

		/*!
		Load a catalog from the given path, return its shared pointer or nullptr on error.
		*/
		std::shared_ptr<Catalog> LoadCatalogFromJson(const std::string& jsonPath);

		/*!
		Parse a catalog from the text, return its shared pointer or nullptr on error.
		*/
		std::shared_ptr<Catalog> ParseCatalogJson(const std::string& jsonText);

		/*!
		Check if a catalog with the given name exists, if not an error message is displayed.
		*/
		bool CheckCatalog(const std::string& catalogName);

		/*!
		Get a reference to the agent with the given name, if not found nullptr is returned and an error message is displayed.
		*/
		std::shared_ptr<xp::CyberSystemAgent> GetAgent(const std::string& agentName);

		/*!
		Check if an agent with the given name exists, if not an error message is displayed.
		*/
		bool CheckAgent(const std::string& agentName) const;

		/*!
		Add the given entity to the scenario, configure it in the cyber system and update the environment state.
		*/
		void AddEntity(std::shared_ptr<Entity> newEntity, bool updateXp);

		/*!
		Clear cached data for each agent.
		*/
		void ClearCache();


		/*!
		Process the outcome of an episode.
		*/
		xp::ActionOutcome ProcessEpisode(
			std::shared_ptr<xp::CyberSystemAgent> agent,
			xp::ActionResult prevResult,
			xp::ActionResult result
			);
	};


	/*!
	Shortcut for DigitalScenarioFramework::GetInstance()
	*/
	inline std::shared_ptr<DigitalScenarioFramework> DiScenFw()
	{
		return DigitalScenarioFramework::GetInstance();
	}

}

//#   if defined(_MSC_VER)
//#     pragma warning(pop) // restore warning settings.
//#   endif
