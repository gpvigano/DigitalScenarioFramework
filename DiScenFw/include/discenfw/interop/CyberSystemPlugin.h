//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/ICyberSystem.h"
#include "discenfw/xp/EnvironmentState.h"
#include "discenfw/xp/Action.h"
#include "discenfw/xp/Condition.h"
#include "discenfw/xp/EnvironmentModel.h"
#include <discenfw/util/MessageLog.h>

#include <boost/config.hpp>
#include <string>
#include <memory>
#include <sstream>

namespace discenfw
{
	namespace xp
	{
		/*!
		Definition of a generic cyber system implementation.
		All cyber system implementations (plugins) must derive from this abstract class.
		*/
		class BOOST_SYMBOL_VISIBLE CyberSystemPlugin : public ICyberSystem
		{
		public:

			virtual ~CyberSystemPlugin() {}

			/*!
			Get the model related to this cyber system.
			*/
			const std::shared_ptr<EnvironmentModel> GetModel() const;


			/*!
			Initialize the system and use current configuration to build the initial state.
			@param rebuild Rebuild the initial state, even if already initialized.
			*/
			void Initialize(bool rebuild = false) override;

			/*!
			Check if the cyber system was initialized.
			*/
			virtual bool IsInitialized() const override { return Initialized; }


			/*!
			Completely clear the system, set it to "not initialized".
			*/
			virtual void Clear() override;

			/*!
			Get the intial state, valid only after the first call to Initialize().
			*/
			const EnvironmentState& GetInitialSystemState() override;

			/*!
			Get the current state, valid only after the first call to InterpretSystemState().
			*/
			const EnvironmentState& GetLastSystemState() override;

			/*!
			Update the internal copy of the last state, computed from the system.
			*/
			const EnvironmentState& InterpretSystemState() override;

			/*!
			Get the list of system failure conditions.
			*/
			const Condition& GetFailureCondition() const override { return FailureCondition; }

			void SetLogEnabled(bool enabled) override { LogEnabled = enabled; }
			bool IsLogEnabled() const override { return LogEnabled; }

			/*!
			Create or update the role with the given name.
			*/
			void SetRole(
				const std::string& roleName,
				Condition successCondition,
				Condition failureCondition,
				Condition deadlockCondition,
				StateRewardRules stateRewardRules
				);

			/*!
			Create a new entity state type, inheriting from the given entity state type,
			with the given name, the given default property values, the given possible property values
			and the given links.
			*/
			std::shared_ptr<EntityStateType> CreateEntityStateType(
				const std::string& parentTypeName,
				const std::string& typeName,
				const std::map<std::string, std::string>& defaultPropertyValues,
				const std::map< std::string, std::vector<std::string> >& possiblePropertyValues,
				const std::vector<std::string>& links
				);

				/*!
			Display and empty the internal stream using the MessageLogger
			*/
			void FlushLog(int severity);

		protected:

			bool LogEnabled = false;

			bool Initialized = false;

			/*!
			Temporary string strem used for logging.
			*/
			std::ostringstream LogStream;

			/*!
			List of system failure conditions.
			*/
			Condition FailureCondition;

			/*!
			Internal copy of the initial system state (see ResetSystem()).
			*/
			std::shared_ptr<EnvironmentState> InitSystemState;

			/*!
			Internal copy of the last state computed from the system (see InterpretSystemState()).
			*/
			std::shared_ptr<EnvironmentState> LastSystemState;

			/*!
			Internal copy of the available actions (GetAvailableActions() must return a reference to this vector).
			*/
			mutable std::vector<xp::ActionRef> CachedAvailableActions;


			/*!
			Define entity state types (see EntityStateType::CreateEntityStateType()).
			*/
			virtual void CreateEntityStateTypes() = 0;

			/*!
			Clear the system definition (remove all the entity definitions).
			*/
			virtual void ClearSystem() = 0;

			/*!
			Encode and add the given action to internal cache.
			*/
			void CacheAvailableAction(const Action& action) const;
		};


		inline void CyberSystemPlugin::Initialize(bool rebuild)
		{
			CreateEntityStateTypes();
			if (!IsInitialized())
			{
				InitFailureConditions();
				Initialized = true;
			}

			if (!InitSystemState || rebuild)
			{
				InitSystemState = EnvironmentState::Make();
			}
			// use the current configuration to build the initial state
			SynchronizeState(InitSystemState);
		}


		inline void CyberSystemPlugin::Clear()
		{
			ClearSystem();
			Initialized = false;
			LogStream.clear();
			FailureCondition = Condition();
			InitSystemState = nullptr;
		}


		inline const EnvironmentState& CyberSystemPlugin::GetInitialSystemState()
		{
			if (!IsInitialized())
			{
				Initialize();
			}
			return *InitSystemState;
		}


		inline const EnvironmentState& CyberSystemPlugin::GetLastSystemState()
		{
			if (!LastSystemState)
			{
				InterpretSystemState();
			}

			return *LastSystemState;
		}


		inline const EnvironmentState& CyberSystemPlugin::InterpretSystemState()
		{
			if (!LastSystemState)
			{
				LastSystemState = EnvironmentState::Make();
			}
			SynchronizeState(LastSystemState);

			return *LastSystemState;
		}


		inline const std::shared_ptr<EnvironmentModel> CyberSystemPlugin::GetModel() const
		{
			return xp::GetModel(GetSystemName());
		}


		inline void CyberSystemPlugin::SetRole(
			const std::string& roleName,
			Condition successCondition,
			Condition failureCondition,
			Condition deadlockCondition,
			StateRewardRules stateRewardRules
			)
		{
			xp::SetRole(
				roleName,
				successCondition,
				failureCondition,
				deadlockCondition,
				stateRewardRules,
				GetSystemName()
				);
		}


		inline std::shared_ptr<EntityStateType> CyberSystemPlugin::CreateEntityStateType(
			const std::string& parentTypeName,
			const std::string& typeName,
			const std::map<std::string,std::string>& defaultPropertyValues,
			const std::map<std::string, std::vector<std::string>>& possiblePropertyValues,
			const std::vector<std::string>& links
			)
		{
			std::shared_ptr<EnvironmentModel> model = GetModel();

			std::shared_ptr<EntityStateType> entStateType = model->GetEntityStateType(typeName);
			if (entStateType)
			{
				return entStateType;
			}
			return model->CreateEntityStateType(
				parentTypeName,
				typeName,
				defaultPropertyValues,
				possiblePropertyValues,
				links
				);
		}

		inline void CyberSystemPlugin::FlushLog(int severity)
		{
			std::string msg = LogStream.str();
			if (!msg.empty())
			{
				LogMessage(severity, msg, GetSystemName());
			}
			LogStream.str("");
		}


		inline void CyberSystemPlugin::CacheAvailableAction(const Action& action) const
		{
			CachedAvailableActions.push_back(GetModel()->EncodeAction(action));
		}

	}
}
