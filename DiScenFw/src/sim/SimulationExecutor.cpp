//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/sim/SimulationExecutor.h>

#include <discenfw/sim/ScenarioHistoryData.h>
#include <discenfw/ve/VirtualEnvironmentAPI.h>
#include <discenfw/ve/VeManager.h>
#include <discenfw/util/MessageLog.h>

#include <gpvulc/time/DateTimeUtil.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>


namespace discenfw
{

	using namespace ve;

	namespace sim
	{

		void SimulationExecutor::Update(const DateTime& dateTime)
		{
			if (IsRunning)
			{
				UpdateTarget(dateTime);
			}
		}

		bool SimulationExecutor::UpdateTarget(const DateTime& dateTime)
		{
			if (History->States.empty() || !TargetElement)
			{
				return false;
			}
			std::shared_ptr<ElementState> prevState;
			std::shared_ptr<ElementState> nextState;

			std::shared_ptr<ElementState> foundState = FindStateAt(dateTime, prevState, nextState);

			std::shared_ptr<ElementState> currState = nullptr;
			if (foundState) currState = foundState;
			else if (prevState) currState = prevState;
			else if (nextState) currState = nextState;

			if (currState)
			{
				std::vector<std::shared_ptr<TemporalState>>::iterator it = std::find(History->States.begin(), History->States.end(), currState);
				size_t idx = std::distance(History->States.begin(), it);
				LogMessage(LOG_VERBOSE, "State " + std::to_string(idx) + " start time = " + gpvulc::DateTimeToString(currState->StartDateTime), "DiScenFw|Sim", false, true, "SimState");
			}

			if (!currState)
			{
				return false;
			}

			// set parent
			if (!currState->Transform.ParentId.empty())
			{
				TargetElement->Transform.ParentId = currState->Transform.ParentId;
				if (VE()->SyncSceneObjectTransform)
				{
					VE()->SyncSceneObjectTransform(TargetElement->GetIdentifier());
				}
				//VE->SetElementTransform(TargetElement->GetIdentifier(), TargetElement->Transform);
				//VE->ReparentElement(TargetElement->GetIdentifier(), currState->Transform.ParentId);
			}

			// set representation
			//SimKeyFrame curr_repr_status = foundState;
			//if (curr_repr_status!=null) curr_repr_status = prevState;

			//if (curr_repr_status && curr_repr_status->Representation)
			//{
			//    //LoadRepresentation(currState->Representation);
			//    SelectRepresentation(curr_repr_status->Representation->Name);
			//}
			//else
			//{
			//    // if no foundState is defined before this one reset representation
			//    RestoreRepresentation();
			//}

			bool anim = !foundState && (prevState && nextState);
			if (!anim)
			{
				LogMessage(LOG_VERBOSE, TargetElement->GetIdentifier() + " (idle)", "DiScenFw|Sim", false, true, "SimStateAnim");
				//ResetAnimation();
				TargetElement->Transform = currState->Transform;
				if (VE()->SyncSceneObjectTransform)
				{
					VE()->SyncSceneObjectTransform(TargetElement->GetIdentifier());
				}
				return true;
			}
			/*
			Vector3 start_pos = prevState.position;
			Quaternion start_rot = prevState.rotation;
			Vector3 end_pos = nextState.position;
			Quaternion end_rot = nextState.rotation;

			if (prevState->Transform != nextState->Transform)
			{
				Matrix4x4 m1 = Matrix4x4.identity;
				if (prevState.parentTransform != null)
				{
					m1 = prevState.parentTransform.worldToLocalMatrix;
				}
				Matrix4x4 m2 = Matrix4x4.identity;
				if (nextState.parentTransform != null)
				{
					TemporalState next_status2 = nullptr;
					EntityHistory next_status_parent_hist = nextState.parentTransform.GetComponent<EntityHistory>();
					if (next_status_parent_hist)
					{
						TemporalState tmp1;
						TemporalState tmp2;
						next_status2 = next_status_parent_hist.FindKeyFrame(nextState.startTime, out tmp1, out tmp2);
					}
					if (next_status2 != null)
					{
						Vector3 end_pos2 = next_status2.position;
						Quaternion end_rot2 = next_status2.rotation;
						if (nextState.parentTransform.parent != null)
						{
							m2 = nextState.parentTransform.parent.localToWorldMatrix;
						}
						Matrix4x4 mTmp = Matrix4x4.TRS(end_pos2, end_rot2, nextState.parentTransform.parent.lossyScale);
						m2 *= mTmp;
					}
					else
					{
						m2 = nextState.parentTransform.localToWorldMatrix;
					}
				}
				Matrix4x4 m = m1 * m2;
				// pos2 to the same local space as pos1
				end_pos = m.MultiplyPoint(nextState.position);
				// rot2 to the same local space as rot1
				end_rot = MathUtility.MatrixToQuaternion(m) * nextState.rotation;
			}
			float t = 0;
			if (prevState.endTime != nextState.startTime)
			{
				t = (float)((simulationDateTime - prevState.endTime).TotalSeconds
					/ (nextState.startTime - prevState.endTime).TotalSeconds);
			}
			targetTransform.parent = prevState.parentTransform;
			targetTransform.localPosition = Vector3.Lerp(start_pos, end_pos, t);
			targetTransform.localRotation = Quaternion.Slerp(start_rot, end_rot, t);
			*/
			float t = 0;
			if (prevState->EndDateTime != nextState->StartDateTime)
			{
				double t1 = gpvulc::DateTimeDistanceSecD(prevState->EndDateTime, dateTime);
				double dt = gpvulc::DateTimeDistanceSecD(prevState->EndDateTime, nextState->StartDateTime);
				t = (float)(t1 / dt);
				LogMessage(LOG_VERBOSE, TargetElement->GetIdentifier() + " lerp = " + std::to_string(t), "DiScenFw|Sim", false, true, "SimStateAnim");
			}
			if (VE()->LerpElementTransform && VE()->SyncElementTransform)
			{
				VE()->LerpElementTransform(TargetElement->GetIdentifier(), prevState->Transform, nextState->Transform, t);
				VE()->SyncElementTransform(TargetElement->GetIdentifier());
			}
			// TODO: provide a default lerp function if LerpElementTransform is not defined

			//VE()->ReparentElement(TargetElement->GetIdentifier(), prevState->Transform.ParentId);
			//TargetElement->Transform = VE()->LerpLocalTransform(prevState->Transform, nextState->Transform, t);
			//VE()->SetElementTransform(TargetElement->GetIdentifier(), TargetElement->Transform);

			return true;
		}


		std::shared_ptr<ElementState> SimulationExecutor::GetElementStateAt(unsigned index)
		{
			std::shared_ptr<TemporalState> entityState = History->States[index];
			if (!entityState->IsA("ElementState"))
			{
				return nullptr;
			}
			return std::static_pointer_cast<ElementState>(entityState);
		}


		std::shared_ptr<ElementState> SimulationExecutor::FindStateAt(const DateTime& time, std::shared_ptr<ElementState>& prevState, std::shared_ptr<ElementState>& nextState)
		{
			if (History->States.empty())
			{
				return nullptr;
			}
			// binary search of the foundState inside the history
			int first = 0;
			int last = (int)(History->States.size()) - 1;
			int middle = (first + last) / 2;
			prevState = nullptr;
			nextState = nullptr;
			std::shared_ptr<ElementState> foundState = nullptr;

			while (first <= last)
			{
				bool stateStarted = time >= History->States[middle]->StartDateTime;
				bool stateNotEnded = time <= History->States[middle]->EndDateTime;
				if (stateStarted && stateNotEnded)
					//if (time >= History->States[middle]->StartDateTime && time <= History->States[middle]->EndDateTime)
				{
					foundState = GetElementStateAt(middle);
					prevState = (middle > 0) ? GetElementStateAt(middle - 1) : nullptr;
					nextState = (middle < (int) History->States.size() - 1) ? GetElementStateAt(middle + 1) : nullptr;
					break;
				}
				else if (first == last)
				{
					bool firstStateEnded = time > History->States[first]->EndDateTime;
					if (firstStateEnded)
					{
						prevState = GetElementStateAt(first);
						nextState = (middle < (int) History->States.size() - 1) ? GetElementStateAt(middle + 1) : nullptr;
					}
					bool firstStateNotStarted = time < History->States[first]->StartDateTime;
					if (firstStateNotStarted)
					{
						prevState = (middle > 0) ? GetElementStateAt(middle - 1) : nullptr;
						nextState = GetElementStateAt(middle);
					}
					break;
				}
				bool stateEnded = time > History->States[middle]->EndDateTime;
				bool stateNotStarted = time < History->States[middle]->EndDateTime;
				if (stateEnded)
				{
					first = middle + 1;
				}
				else if (stateNotStarted)
				{
					last = middle - 1;
				}
				int index = middle;
				middle = (first + last) / 2;
				if (index == middle)
				{
					LogMessage(LOG_ERROR, "SimulationExecutor::FindStateAt - loop detected", "DiScenFw|Sim", false, true, "SimFindState");
					// infinite loop detected, stop here
					break;
				}
			}

			bool finalStateEnded = time > History->States[History->States.size() - 1]->EndDateTime;
			if (finalStateEnded)
			{
				prevState = GetElementStateAt((int)History->States.size() - 1);
				nextState = nullptr;
			}
			bool initialStateNotStarted = time < History->States[0]->StartDateTime;
			if (initialStateNotStarted)
			{
				nextState = GetElementStateAt(0);
				prevState = nullptr;
			}
			return foundState;
		}


	} // namespace scen
}
