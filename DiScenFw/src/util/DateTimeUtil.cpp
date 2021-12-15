//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/util/DateTimeUtil.h>

#include <gpvulc/time/DateTimeUtil.h>

#include <string>


namespace discenfw
{
	long long DateTimeDistanceSec(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		return gpvulc::DateTimeDistanceSec(dateTime1, dateTime2);
	}


	double DateTimeDistanceSecD(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		return gpvulc::DateTimeDistanceSecD(dateTime1, dateTime2);
	}


	long long DateTimeDistanceMs(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		return gpvulc::DateTimeDistanceMs(dateTime1, dateTime2);
	}


	const char* DateTimeDistanceString(const DateTime& dateTime1, const DateTime& dateTime2)
	{
		static std::string str;
		str = gpvulc::DateTimeDistanceString(dateTime1, dateTime2);
		return str.c_str();
	}


	DateTime DateTimeAdd(const DateTime& dateTime, int h, int m, int s, int ms)
	{
		return gpvulc::DateTimeAdd(dateTime, h, m, s, ms);
	}


	DateTime DateTimeAddMs(const DateTime& dateTime, long ms)
	{
		return gpvulc::DateTimeAddMs(dateTime, ms);
	}


	//-------------------------------------------------------------------
	// time-string conversion


	DateTime StringToDateTime(const std::string& timeString)
	{
		return gpvulc::StringToDateTime(timeString);
	}


	const char* DateTimeToString(const DateTime& dateTime)
	{
		static std::string str;

		str = gpvulc::DateTimeToString(dateTime);
		return str.c_str();
	}

} // namespace discenfw

