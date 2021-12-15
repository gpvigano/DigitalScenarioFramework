//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2019-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//



/// @brief Date/Time data structure definition
/// @file DateTime.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef GPVULC_DATETIME_H
#define GPVULC_DATETIME_H

namespace gpvulc
{

	/// @addtogroup Time
	/// @{

	enum WDay { SUNDAY = 1, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY };

	//! Date and time data structure with mill milliseconds and time offset
	struct DateTime
	{
		/*!
		Years (A.D., since 0)
		*/
		int Year = 0;

		/*!
		Months since January [1, 12]
		*/
		int Month = 0;

		/*!
		Day of the month [1, 31]
		*/
		int Day = 0;

		/*!
		Day of the week [1, 7]
		*/
		int WeekDay = 0;

		/*!
		Hours since midnight [0, 23]
		*/
		int Hour = 0;

		/*!
		Minutes after the hour [0, 59]
		*/
		int Minute = 0;

		/*!
		Seconds after the minute [0, 59]
		*/
		int Second = 0;

		/*!
		Milliseconds after the second [0, 999]
		*/
		int Millisecond = 0;

		/*!
		UTC time offset hour [-12, +14]
		*/
		int TimeOffsetHour = 0;

		/*!
		UTC time offset minute [0, +-30]
		*/
		int TimeOffsetMinute = 0;

		/*!
		Is daylight savings time.
		*/
		bool IsDST = false;

		DateTime(int year = 0,
			int month = 0,
			int day = 0,
			int hour = 0,
			int minute = 0,
			int second = 0,
			int millisecond = 0,
			int timeOffsetHour = 0,
			int timeOffsetMinute = 0)
			: Year(year),
			Month(month),
			Day(day),
			Hour(hour),
			Minute(minute),
			Second(second),
			Millisecond(millisecond),
			TimeOffsetHour(timeOffsetHour),
			TimeOffsetMinute(timeOffsetMinute)
		{
		}

		/*!
		Check if this date/time is valid.
		*/
		bool Valid() { return Month > 0 && Day > 0 && WeekDay > 0; }

		/*!
		Reset (invalidate) this date/time.
		*/
		void Reset() { *this = DateTime(); }
	};
}//namespace gpvulc



#endif // GPVULC_DATETIME_H

