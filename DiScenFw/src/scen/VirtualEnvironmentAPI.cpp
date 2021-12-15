//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/ve/VirtualEnvironmentAPI.h>
#include <discenfw/util/MessageLog.h>

#include <iostream>
#include <sstream>

namespace discenfw
{
	namespace ve
	{

		bool VirtualEnvironmentAPI::Validate(int apiLevel)
		{
			ValidatedApiLevel = 0;
			ErrorList.clear();
			std::ostringstream msgStrStr;
			AllDefined = true;
			int boundFuncCount = 0;

#define DISCENAPI_VE_VALIDATE_FUNC(fn)\
				if (fn)\
				{\
					boundFuncCount++;\
				}\
				else\
				{\
					msgStrStr << "  " << #fn << " not defined." << std::endl;\
					allDefined = false;\
				}

			// check API definition

			bool allDefined = true;

			if (apiLevel&LOGGING)
			{
				allDefined = IsLogDisplayFunctionDefined();
				if (allDefined) ApiLevel |= LOGGING;
				else AllDefined = false;
				ValidatedApiLevel |= LOGGING;
			}
			allDefined = true;
			if (apiLevel&SCENARIO)
			{
				DISCENAPI_VE_VALIDATE_FUNC(SyncScenario)
					DISCENAPI_VE_VALIDATE_FUNC(SyncScene)
					if (allDefined) ApiLevel |= SCENARIO;
					else AllDefined = false;
					ValidatedApiLevel |= SCENARIO;
			}
			allDefined = true;
			if (apiLevel&SIMULATION)
			{
				//DISCENAPI_VE_VALIDATE_FUNC(LerpLocalTransform)

				//DISCENAPI_VE_VALIDATE_FUNC(SyncEntity)
				DISCENAPI_VE_VALIDATE_FUNC(SyncElementTransform)
					DISCENAPI_VE_VALIDATE_FUNC(SyncSceneObjectTransform)
					//DISCENAPI_VE_VALIDATE_FUNC(SyncPipelinePath)
					//DISCENAPI_VE_VALIDATE_FUNC(SetEntityActive)
					//DISCENAPI_VE_VALIDATE_FUNC(ReparentElement)
					if (allDefined) ApiLevel |= SIMULATION;
					else AllDefined = false;
					ValidatedApiLevel |= SIMULATION;
			}
			allDefined = true;
			if (apiLevel&NAVIGATION)
			{
				DISCENAPI_VE_VALIDATE_FUNC(ProjectDir)
					DISCENAPI_VE_VALIDATE_FUNC(TakeScreenshot)
					DISCENAPI_VE_VALIDATE_FUNC(MoveViewPointTo)
					DISCENAPI_VE_VALIDATE_FUNC(LookAt)
					DISCENAPI_VE_VALIDATE_FUNC(SetViewPointCoordSys)
					DISCENAPI_VE_VALIDATE_FUNC(SetViewPointDir)
					if (allDefined) ApiLevel |= NAVIGATION;
					else AllDefined = false;
					ValidatedApiLevel |= NAVIGATION;
			}

			// notify an error only if at least one function was bound
			// (thus if there is no VE no error is notified)
			if (!AllDefined && boundFuncCount > 0)
			{
				ErrorList = "VE API not completely defined:\n";
				ErrorList += msgStrStr.str();
				LogMessage(LOG_ERROR, ErrorList, "DiScenFw");
			}

			//// check other API definition

			//allDefined = true;
			//msgStrStr.str("");

			//if (!allDefined)
			//{
			//	AllDefined = false;
			//	WarningList = "VE API not completely defined:\n";
			//	WarningList += msgStrStr.str();
			//	LogMessage(WARNING, WarningList, "DiScenFw");
			//}
#undef DISCENAPI_VE_VALIDATE_FUNC

			return AllDefined;
		}



	} // namespace scen
} // namespace discenfw

