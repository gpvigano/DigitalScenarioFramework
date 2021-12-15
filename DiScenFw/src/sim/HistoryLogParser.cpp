//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/sim/HistoryLogParser.h>
#include <discenfw/sim/ScenarioHistoryData.h>

#include <gpvulc/text/text_util.h>
#include <gpvulc/time/DateTimeUtil.h>

#include <fstream>
#include <sstream>

namespace discenfw
{
	namespace sim
	{

		HistoryLogParser::HistoryLogParser()
		{
		}


		HistoryLogParser::~HistoryLogParser()
		{
		}

		bool HistoryLogParser::ParseFile(const std::string& logPath, EntityHistory& history)
		{
			std::ifstream in(logPath);
			std::string logText;
			if (gpvulc::LoadText(logPath, logText))
			{
				return ParseText(logText, history);
			}
			return false;
		}

		bool HistoryLogParser::ParseText(const std::string& logText, EntityHistory& history)
		{
			std::istringstream textStrStr(logText);
			std::string line;
			if (!textStrStr.good())
			{
				return false;
			}
			while (textStrStr.good())
			{
				std::string startTime;
				std::string endTime;
				textStrStr >> startTime;
				if (!startTime.empty())
				{
					float x;
					float y;
					float z;
					float sc2mt;
					Vector3D rightAxis;
					std::string yAxis;
					std::string zAxis;
					std::shared_ptr<ElementState> state = std::make_shared<ElementState>();
					state->StartDateTime = gpvulc::StringToDateTime(startTime);
					textStrStr >> endTime;
					state->EndDateTime = gpvulc::StringToDateTime(endTime);
					textStrStr >> state->Origin;
					textStrStr >> x >> y >> z;
					textStrStr >> sc2mt;
					sc2mt *= ScaleFactor;
					state->Transform.CoordSys.Origin = { sc2mt*x,sc2mt*z,sc2mt*y };
					// xAxis
					textStrStr >> x >> y >> z;
					state->Transform.CoordSys.RightAxis = { sc2mt*x,sc2mt*z,sc2mt*y };
					// yAxis
					textStrStr >> x >> y >> z;
					state->Transform.CoordSys.UpAxis = { sc2mt*x,sc2mt*z,sc2mt*y };
					// zAxis
					textStrStr >> x >> y >> z;
					state->Transform.CoordSys.ForwardAxis = { sc2mt*x,sc2mt*z,sc2mt*y };
					textStrStr >> state->Transform.ParentId;
					state->Transform.UseCoordSys = true;
					if (state->Transform.ParentId == "ScenarioRoot")
					{
						state->Transform.ParentId.clear();
					}
					history.States.push_back(state);
				}
			}

			return true;
		}


	} // namespace disenapi
}
