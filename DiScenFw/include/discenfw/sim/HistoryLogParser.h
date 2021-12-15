//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <string>
#include <ctime>

namespace discenfw
{
	namespace sim
	{
		struct EntityHistory;

		/*!
		Parser for importing simulation history log files.

		Simulation history log files are expected to be a sequence of lines in the following format:
		@code
		<StartDateTime> <EndDateTime> <Origin> <CoordinateSystem> <Reference>
		@endcode
		where <Reference> can be "ScenarioRoot" or the identifier of the element to which the coordinate system refers.
		Eaxmple:
		@code
		2013-11-27T09:00:00	2013-11-27T09:00:02	SIMULATED	0 0 100	0.01	1 0 0  0 1 0  0 0 1	ScenarioRoot
		@endcode
		@see DateTime, CoordSys3D
		*/
		class HistoryLogParser
		{
		public:

			/*!
			Scale factor for coordinates scaling.
			*/
			float ScaleFactor = 1.0f;

			HistoryLogParser();
			~HistoryLogParser();

			/*!
			Parse a simulation history log file.
			*/
			bool ParseFile(const std::string& logPath, EntityHistory& history);

			/*!
			Parse a simulation history log text.
			*/
			bool ParseText(const std::string& logText, EntityHistory& history);
		};
	}
}

