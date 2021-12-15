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
#include "discenfw/xp/CyberSystemAssistant.h"
#include <memory>
#include <vector>


namespace discenfw
{
	namespace xp
	{
		/*!
		Enable multiple agents acting together on the same cyber system
		in the same shared arena.
		*/
		class DISCENFW_API SharedArena : public std::enable_shared_from_this<SharedArena>
		{
		public:
			/*!
			List of actors (CyberSystemAssistant) sharing the same arena.
			*/
			std::vector< std::shared_ptr<CyberSystemAssistant> > Actors;

			/*!
			Shared scenario state, the actors share the same environment.
			*/
			std::shared_ptr<EnvironmentState> Environment;

			/*!
			Default empty constructor.
			*/
			SharedArena();

			/*!
			Construct a SharedArena initialized with a list of actors (CyberSystemAssistant).
			*/
			SharedArena(const std::vector< std::shared_ptr<CyberSystemAssistant> >& actors);

			/*!
			Call NewEpisode() on the given actor (CyberSystemAssistant),
			then call StartEpisode on the other actors in the Arena.
			@param actor acting CyberSystemAssistant
			*/
			void NewEpisode(const std::shared_ptr<CyberSystemAssistant> actor);

			/*!
			Call TakeAction() on each CyberSystemAssistant in the Arena, actors other than the given one process the action as external.
			@param actor Acting CyberSystemAssistant.
			@param action Action to be taken.
			@param updateXp Update experience (default is true).
			*/
			ActionResult TakeAction(
				const std::shared_ptr<CyberSystemAssistant> actor,
				const Action& action,
				bool updateXp = true
				);

			/*!
			Call AcquireTransition() on each CyberSystemAssistant in the Arena, other than the given one.
			@param actor Acting CyberSystemAssistant.
			@param action Action to be taken.
			@param updateXp Update experience (default is true).
			*/
			void NotifyTransition(
				const std::shared_ptr<CyberSystemAssistant> actor,
				const Transition& newTransition,
				bool updateXp = true
				);

			/*!
			Check if there is more than one actor in the arena.
			*/
			bool HasActor(const std::shared_ptr<CyberSystemAssistant> actor) const
			{
				return std::find(Actors.cbegin(), Actors.cend(), actor) == Actors.cend();
			}

			/*!
			Check if there is more than one actor in the arena.
			*/
			bool MultiActor() const { return Actors.size() > 1; }


			/*!
			Initialize the actors ensuring they have a reference to this arena.
			@note This method must be called soon after creation, before any other operation.
			*/
			void Initialize();
		};

	}
}

