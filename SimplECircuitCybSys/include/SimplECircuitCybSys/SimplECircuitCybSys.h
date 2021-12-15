//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "DiScenFw/interop/CyberSystemPlugin.h"
#include "SimplECircuitData.h"
#include <memory>

/*!
SimplECircuit: simplified electronic circuit cyber system implementation.
*/
namespace simplecircuit_cybsys
{
	using namespace discenfw;

	/*!
	SimplECircuit CyberSystemPlugin implementation.
	*/
	class SimplECircuitCybSys : public xp::CyberSystemPlugin
	{
	public:

		SimplECircuitCybSys();

		void SynchronizeState(std::shared_ptr<xp::EnvironmentState> environmentState) override;

		const std::vector<xp::ActionRef>& GetAvailableActions(
			const std::string& roleId = "",
			bool smartSelection = false
			) const override;

		void InitFailureConditions() override;

		void InitRoles() override;

		bool ExecuteAction(const xp::Action& action) override;

		void ResetSystem() override;

		const std::string GetSystemName() const override { return "SimplECircuit"; }

		const std::string GetSystemInfo(const std::string& infoId = "") const override;

		bool SetConfiguration(const std::string& config) override;
		const std::string GetConfiguration() override;

		const std::string ReadEntityConfiguration(const std::string& entityId) override;
		bool WriteEntityConfiguration(const std::string& entityId, const std::string& config) override;
		bool ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config) override;

		bool RemoveEntity(const std::string& entityId) override;

		std::shared_ptr<ElectronicComponent> CreateComponentFromConfiguration(
			const std::string& config, std::string& compId);
		const std::string GetComponentConfiguration(const std::string& compId);
		bool SetComponentConfiguration(const std::string& compId, const std::string& config);

	protected:

		std::unique_ptr<simplecircuit_cybsys::ElectronicCircuit> Circuit;

		std::shared_ptr<xp::EntityStateType> ComponentEntityType;
		std::shared_ptr<xp::EntityStateType> PowerEntityType;
		std::shared_ptr<xp::EntityStateType> LedEntityType;
		std::shared_ptr<xp::EntityStateType> SwitchEntityType;
		std::shared_ptr<xp::EntityStateType> ResistorEntityType;
		// //std::shared_ptr<xp::EntityStateType> TransistorEntityType;

		void CreateEntityStateTypes() override;

		virtual void ClearSystem() override;

		bool DoConnectAction(const xp::Action& action);

		bool DoSwitchAction(const xp::Action& action);

		bool DoDisconnectAction(const xp::Action& action);


		void ReadComponentConfiguration(std::istringstream& iStr, std::shared_ptr<ElectronicComponent> comp);

		std::shared_ptr<ElectronicComponent> MakeComponent(const std::string& compType);
	};
}

