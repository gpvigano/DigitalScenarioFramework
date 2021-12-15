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
#include <memory>


namespace sample_cybsys
{
	using namespace discenfw;

	class SampleCybSys : public xp::CyberSystemPlugin
	{
	public:

		SampleCybSys();

		void SynchronizeState(std::shared_ptr<xp::EnvironmentState> environmentState) override;

		const std::vector<xp::ActionRef>& GetAvailableActions(
			const std::string& roleId = "",
			bool smartSelection = false
			) const override;

		void InitFailureConditions() override;

		void InitRoles() override;

		bool ExecuteAction(const xp::Action& action) override;

		void ResetSystem() override;

		const std::string GetSystemName() const override { return "SampleCyberSystem"; }

		const std::string GetSystemInfo(const std::string& infoId = "") const override;

		bool SetConfiguration(const std::string& config) override;

		const std::string GetConfiguration() override;

		const std::string ReadEntityConfiguration(const std::string& entityId) override;

		bool WriteEntityConfiguration(const std::string& entityId, const std::string& config) override;

		bool ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config) override;

		bool RemoveEntity(const std::string& entityId) override;

	protected:

		std::shared_ptr<xp::EntityStateType> SampleEntityType;


		void CreateEntityStateTypes() override;

		virtual void ClearSystem() override;

	};
}

