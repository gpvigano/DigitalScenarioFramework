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

namespace discenfw
{
	namespace xp
	{
		/*!
		RL parameters configuration for reinforcement learning (see RLearner).
		*/
		class RLConfig : public IAgentConfiguration
		{
		public:

			/*!
			Value used to set initial values.
			*/
			float InitialValue = 0.0f;

			/*!
			Use the sample-average method in place of FixedStepSize.
			In this way the learning rate is decreased at each update.
			*/
			bool SampleAverage = false;

			/*!
			Step-size parameter (learning rate)

			The step-size is defined to be Alpha (0..1), but it can be
			automaticallly computed using the sample-average method.
			If the sample-average method is used this parameter is ignored
			(see SampleAverage parameter).
			*/
			float FixedStepSize = 0.5f;

			/*!
			Discount rate (0..1, default=0.9)
			*/
			float DiscountRate = 0.9f;

			/*!
			Probability of taking a random action in an epsilon-greedy policy (default=0.2).
			*/
			float Epsilon = 0.2f;

			/*!
			If set to a positive value (0..1) the Epsilon in epsilon-greedy policy
			is gradually reduced for each state by this factor,
			from the specified Epsilon value to 0,
			if 1 (default) or negative no reduction is applied.
			@note This parameter is modulated by the possible action choices for each state
			(more choices cause less reduction).
			*/
			float EpsilonReduction = 1.0f;



			RLConfig()
			{
			}

			virtual ~RLConfig()
			{
			}

			/*!
			Get the real class name.
			*/
			virtual const char* GetClassName() const override
			{
				return "RLConfig";
			}

			/*!
			Check if this class or one of its ancestors has the given class name.
			*/
			virtual bool IsA(const std::string& classType) const
			{
				return classType == RLConfig::GetClassName();
			}


			/*!
			Check and fix parameters.
			*/
			void CheckParameters()
			{

				// clamp between 0 and 1
				auto Clamp01 = [](float& val)
				{
					if (val < 0.0f) val = 0.0f;
					if (val > 1.0f) val = 1.0f;
				};

				// fix parameters ranges
				Clamp01(FixedStepSize);
				Clamp01(DiscountRate);
				Clamp01(Epsilon);
			}
		};

	}
}

