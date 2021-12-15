//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/SharedArena.h>
#include <discenfw/util/MessageLog.h>

#include <iostream>

namespace discenfw
{
	namespace xp
	{

		SharedArena::SharedArena()
		{
		}


		SharedArena::SharedArena(const std::vector<std::shared_ptr<CyberSystemAssistant>>& actors)
			: Actors(actors)
		{
		}


		void SharedArena::Initialize()
		{
			// This method cannot be called by the constructor, because
			// 'this' pointer cannot be used before the object is constructed
			for (auto& actor : Actors)
			{
				if (actor)
				{
					actor->SetArena(shared_from_this(), false);
				}
			}
		}


		void SharedArena::NewEpisode(const std::shared_ptr<CyberSystemAssistant> actor)
		{
			Initialize();
			actor->NewEpisode();

			if (MultiActor())
			{
				std::shared_ptr<EnvironmentState> initState = actor->GetLastState();
				for (auto& otherActor : Actors)
				{
					if (otherActor != actor)
					{
						otherActor->StartEpisode(initState);
					}
				}
			}
		}


		ActionResult SharedArena::TakeAction(
			const std::shared_ptr<CyberSystemAssistant> actor,
			const Action& action,
			bool updateXp
			)
		{
			Initialize();
			// check if the given actor belongs to this arena
			if (std::find(Actors.begin(), Actors.end(), actor) == Actors.end())
			{
				LogMessage(LOG_ERROR, "Actor " + actor->GetId() + " does not belong to this arena.", "DiScenFw");
				return ActionResult::DENIED;
			}

			if (!actor->GetLastState())
			{
				actor->NewEpisode();
			}
			bool noOtherActor = (Actors.empty() || (Actors.size() == 1 && Actors[0] == actor));
			if (noOtherActor)
			{
				return actor->TakeAction(action, updateXp);
			}
			std::shared_ptr<EnvironmentState> prevState = actor->GetLastState();
			ActionRef actionRef = actor->GetCurrentExperience()->GetModel()->EncodeAction(action);
			ActionResult result = actor->TakeAction(action, updateXp);
			Environment = actor->GetLastState();
			NotifyTransition(actor, { prevState, actionRef, Environment }, updateXp);
			return result;
		}


		void SharedArena::NotifyTransition(
			const std::shared_ptr<CyberSystemAssistant> actor,
			const Transition& newTransition,
			bool updateXp
			)
		{
			const StateRef prevState = newTransition.StartState;
			const StateRef newState = newTransition.EndState;
			if (prevState == newState)
			{
				return;
			}
			Initialize();
			for (auto& otherActor : Actors)
			{
				if (otherActor != actor)
				{
					if (!otherActor->GetLastState())
					{
						// each actor must participate in the episode
						otherActor->StartEpisode(prevState);
					}
					// state changes must be notified to each actor
					otherActor->AcquireTransition(newTransition, updateXp);
				}
			}
		}

	} // namespace xp
} // namespace discenfw

