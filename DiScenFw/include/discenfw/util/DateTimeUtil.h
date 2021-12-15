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

#include "discenfw/util/DateTime.h"

#include <vector>
#include <string>
#include <memory>
#include <limits>

namespace discenfw
{
	using gpvulc::WDay;
	using gpvulc::DateTime;

	/*!
	Parse a string and return the resulting DateTime structure.
	*/
	DISCENFW_API DateTime StringToDateTime(const std::string& timeString);

	/*!
	Encode a DateTime structure to a string.
	*/
	DISCENFW_API const char* DateTimeToString(const DateTime& dateTime);

	/*!
	Calculate the time in milliseconds between two DateTime.
	*/
	DISCENFW_API long long DateTimeDistanceMs(const DateTime & dateTime1, const DateTime & dateTime2);

	/*!
	Calculate the time in seconds between two DateTime.
	*/
	DISCENFW_API long long DateTimeDistanceSec(const DateTime& dateTime1, const DateTime& dateTime2);

	/*!
	Calculate the time in seconds (with decimals) between two DateTime.
	*/
	DISCENFW_API double DateTimeDistanceSecD(const DateTime & dateTime1, const DateTime & dateTime2);

	/*!
	Calculate the time between two date/time and convert it to a string with hours, minutes, seconds.
	*/
	DISCENFW_API const char* DateTimeDistanceString(const DateTime& dateTime1, const DateTime& dateTime2);


	/*!
	Add to a date time the given hours, minutes, seconds, milliseconds.
	*/
	DISCENFW_API DateTime DateTimeAdd(const DateTime& dateTime1, int h, int m, int s, int ms = 0);

	/*!
	Add to a date time the given milliseconds.
	*/
	DISCENFW_API DateTime DateTimeAddMs(const DateTime& dateTime, long ms);

}

#ifdef DISCENFW_IMPORT
bool DISCENFW_API operator<(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

bool DISCENFW_API operator==(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

inline bool operator!=(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2)
{
	return !(dateTime1 == dateTime2);
}

bool DISCENFW_API operator>(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

bool DISCENFW_API operator<=(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);

bool DISCENFW_API operator>=(const gpvulc::DateTime& dateTime1, const gpvulc::DateTime& dateTime2);
#endif
