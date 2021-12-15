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
#include "discenfw/xp/ICyberSystem.h"
#include "discenfw/xp/Condition.h"
#include "discenfw/xp/EnvironmentState.h"

#include <memory>
#include <functional>

/// @cond DOXYGEN_IGNORE
namespace boost { template <class T> class shared_ptr; }
/// @endcond

namespace discenfw
{
	namespace xp
	{
		class CyberSystemPlugin;

		/*!
		Binding of ICyberSystem methods to an external DLL (CyberSystemPlugin).

		It provides methods to load a cyber system implementation at run-time and
		reroute interface methods calls to that implementation.
		@see ICyberSystem, CyberSystemPlugin
		*/
		class DISCENFW_API CyberSystemLink : public ICyberSystem
		{
		public:
			CyberSystemLink();
			virtual ~CyberSystemLink();

			virtual void SynchronizeState(std::shared_ptr<EnvironmentState> environmentState) override;

			const std::vector<ActionRef>& GetAvailableActions(
				const std::string& roleId = "",
				bool smartSelection = false
				) const override;

			void InitFailureConditions() override;

			virtual void InitRoles() override;

			bool ExecuteAction(const Action& action) override;

			const EnvironmentState& InterpretSystemState() override;

			void ResetSystem() override;

			const std::string GetSystemName() const override;

			const std::string GetSystemInfo(const std::string& infoId) const override;

			bool SetConfiguration(const std::string& config) override;

			const std::string GetConfiguration() override;

			const std::string ReadEntityConfiguration(const std::string& entityId) override;

			bool WriteEntityConfiguration(const std::string& entityId, const std::string& config) override;

			bool ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config) override;

			bool RemoveEntity(const std::string& entityId) override;

			void Initialize(bool rebuild = false) override;

			bool IsInitialized() const override;

			void Clear() override;

			const EnvironmentState& GetInitialSystemState() override;

			const EnvironmentState& GetLastSystemState() override;

			const Condition& GetFailureCondition() const override;

			void SetLogEnabled(bool enabled) override;
			bool IsLogEnabled() const override;

			/*!
			Get the model related to the linked cyber system.
			*/
			const std::shared_ptr<EnvironmentModel> GetModel() const;

			/*!
			Load a cyber system implementation with the given name.
			@param agentPluginName Path without extension (-x32 or -x64 are automatically added under Windows platform).
			@return true if successfully loaded, false otherwise.
			*/
			bool LoadCyberSystem(const std::string& cyberSystemPluginName);

			/*!
			Check if a cyber system implementation is loaded.
			*/
			bool IsCyberSystemLoaded() const;

			/*!
			Check if a cyber system implementation is loaded, log an error if not.
			*/
			bool CheckCyberSystemLoaded() const;

			/*!
			Check if the given cyber system implementation is loaded.
			*/
			bool IsCyberSystemLoaded(const std::string& cyberSystemPluginName) const;

			/*!
			Get the name of the loaded cyber system implementation (the name used to load it).
			@note This is different from GetSystemName() that returns an identifier of the cyber system.
			*/
			const std::string& GetCyberSystemPluginName() const { return CyberSystemPluginName; }

		protected:

			std::string CyberSystemPluginName;

			std::unique_ptr < boost::shared_ptr<CyberSystemPlugin> > PluginPtr;
		};
	}
}
