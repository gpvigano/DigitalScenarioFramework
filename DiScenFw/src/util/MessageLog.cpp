//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/util/MessageLog.h>

#include <gpvulc/console/MessageLogger.h>

#include <iostream>
#include <sstream>

namespace discenfw
{

	void SetLogDisplayFunction(
		std::function<void(
		int severity,
		const std::string& message,
		const std::string& category,
		bool onConsole,
		bool onScreen,
		const std::string& msgTag)>	displayMessageFunc)
	{
		gpvulc::GetGlobalLogger()->DisplayMessage = displayMessageFunc;
	}


	bool IsLogDisplayFunctionDefined()
	{
		return gpvulc::GetGlobalLogger()->DisplayMessage != nullptr;
	}


	void LogMessage(
		int severity,
		const std::string& message,
		const std::string& category,
		bool toConsole,
		bool toScreen,
		const std::string& msgRef)
	{
		gpvulc::LogMessage(
			severity, message, category, toConsole, toScreen, msgRef);
	}


	void LogMessage(
		int severity,
		const std::string& message,
		const std::string& category)
	{
		gpvulc::LogMessage(severity, message, category);
	}

}

