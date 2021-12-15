//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/RL/IAgentConfiguration.h"

#include <memory>
#include <set>

namespace sample_agent
{

	class SampleAgentConfig : public discenfw::xp::IAgentConfiguration
	{
	public:

		/*!
		Boost exploration: always choose unknown actions first.
		*/
		bool BoostExploration = false;

		/*!
		Value used to set values for new (not yet evaluated) actions.
		*/
		float NewActionValue = 0.0f;

		/*!
		Learning rate (0..1).
		*/
		float LearningRate = 0.5f;


		SampleAgentConfig()
		{
		}

		virtual ~SampleAgentConfig()
		{
		}

		/*!
		Get the real class name.
		*/
		virtual const char* GetClassName() const override
		{
			return "SampleAgentConfig";
		}

		/*!
		Check if this class or one of its ancestors has the given class name.
		*/
		virtual bool IsA(const std::string& classType) const
		{
			return classType == SampleAgentConfig::GetClassName();
		}

	};

}
