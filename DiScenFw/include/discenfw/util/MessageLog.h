//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2019-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "DiScenFwConfig.h"

#include <memory>
#include <functional>
#include <string>

namespace discenfw
{
	/*!
	Message log severity level.
	*/
	enum LogLevel
	{
		LOG_DEBUG = 0, LOG_VERBOSE, LOG, LOG_WARNING, LOG_ERROR, LOG_FATAL
	};


	/*!
	Set the callback function used to display log messages.
	*/
	DISCENFW_API void SetLogDisplayFunction(
		std::function<void(
		int severity,
		const std::string& message,
		const std::string& category,
		bool onConsole,
		bool onScreen,
		const std::string& msgTag)>	displayMessageFunc);

	/*!
	Check if the callback for message logging is defined (see SetLogDisplayFunction()).
	*/
	DISCENFW_API bool IsLogDisplayFunctionDefined();

	/*!
	Log a message using the global MessageLogger.
	@param severity Severity level (see LogLevel).
	@param message Text of the message to be displayed.
	@param category Category name, used to identify the source of the message.
	@param toConsole Display the message on console (when available).
	@param toScreen Display the message on screen (when available).
	@param msgRef Reference identifier used to update existing messages instead of creating a new one.
	*/
	DISCENFW_API void LogMessage(
		int severity,
		const std::string& message,
		const std::string& category,
		bool toConsole,
		bool toScreen,
		const std::string& msgRef = "");


	/*!
	Log a message using the global MessageLogger.
	@param severity Severity level (see LogLevel).
	@param message Text of the message to be displayed.
	@param category Category name, used to identify the source of the message.
	@see SetDefaultOutput(), LogMessage(int,const std::string&,const std::string&,bool,bool,const std::string&)
	*/
	DISCENFW_API void LogMessage(
		int severity,
		const std::string& message,
		const std::string& category);

}
