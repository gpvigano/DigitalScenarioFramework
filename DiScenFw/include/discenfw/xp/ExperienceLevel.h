//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#ifndef EXPERIENCE_LEVEL_H
#define EXPERIENCE_LEVEL_H

namespace discenfw
{
	namespace xp
	{

		/*!
		Level of experience.
		*/
		enum class ExperienceLevel
		{
			NONE, // No experience: no episode yet completed, not able to suggest actions.
			TRAINEE, // Learning: at least one episode completed, not able to suggest actions.
			ASSISTANT, // Assistant: able to suggest actions, still learning.
			TRAINER, // Trainer: able to suggest actions, no more learning.
			UNKNOWN // Unknown: the level cannot be determined (e.g. an error occurred).
		};
	}
}


#endif // EXPERIENCE_LEVEL_H
