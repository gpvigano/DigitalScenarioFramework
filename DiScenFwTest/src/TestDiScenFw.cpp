//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <DiScenXpTest.h>

#include <DiScenFw/DigitalScenarioFramework.h>
#include <DiScenFw/scen/ScenarioManager.h>
#include <DiScenFw/sim/SimulationManager.h>

#include <gpvulc/console/console_menu.h>
#include <gpvulc/console/console_util.h>


using namespace discenfw;

namespace discenfw_test
{

	bool TestDiScenFw(int subTestChoice)
	{
		int savedSubTestChoice = subTestChoice;
		bool done = false;
		while (!done)
		{
			if (subTestChoice < 0)
			{
				subTestChoice = gpvulc::GetMenuChoice({
					"Scenario serialization",
					"Scenario serialization (errors)",
					"Scenario simulation",
					"Catalog serialization",
					//TODO: add XP tests
					"All"
				}, "Back");
			}

			auto testScenarioSerialization = []()
			{
				DiScenFw()->CreateSampleData();
				DiScenFw()->SaveScenario("../test/test.json");
				DiScenFw()->LoadScenario("../test/test.json");
			};

			auto testScenarioSerializationErrors = []()
			{
				DiScenFw()->LoadScenario("notfound.json");
				DiScenFw()->LoadScenario("../data/error.json");
				DiScenFw()->LoadScenario("../data/wrong.json");
			};

			auto testScenarioSimulation = [&]()
			{
				DiScenFw()->LoadScenario("../data/sample_scenario.json");
				DiScenFw()->LoadScenarioHistory("../data/history.json");
				DiScenFw()->ScenarioSimulationController()->PlaySimulation();
				for (int i = 0; i < 5; i++)
				{
					DiScenFw()->ScenarioSimulation()->UpdateSimulation(1.0f);
				}
				DiScenFw()->ScenarioSimulationController()->StopSimulation();
				for (int i = 0; i < 5; i++)
				{
					DiScenFw()->ScenarioSimulation()->UpdateSimulation(1.0f);
				}
				DiScenFw()->ScenarioSimulationController()->PlaySimulation();
				for (int i = 0; i < 20; i++)
				{
					DiScenFw()-> ScenarioSimulation()->UpdateSimulation(1.0f);
				}
			};

			auto testCatalogSerialization = []()
			{
				if (DiScenFw()->LoadCatalog("test_catalog", "../data/test_catalog.json"))
				{
					std::cout << "Catalog loaded." << std::endl;
					if (DiScenFw()->SaveCatalog("test_catalog", "../test/test_catalog_saved.json"))
					{
						std::cout << "Catalog saved." << std::endl;
					}
				}
				// TODO: compare input and output
			};


			auto testStart = GetTimeNow();

			switch (subTestChoice)
			{
			case 1:
				testScenarioSerialization();
				break;
			case 2:
				testScenarioSerializationErrors();
				break;
			case 3:
				testScenarioSimulation();
				break;
			case 4:
				testCatalogSerialization();
				break;
			case 5:
				testScenarioSerialization();
				testScenarioSimulation();
				testCatalogSerialization();
				break;
			default:
				DiScenFw()->ResetAll();
				return false;
			}
			DiScenFw()->ResetAll();
			PrintTestDuration(testStart);
			gpvulc::ConsolePause();
			if (savedSubTestChoice < 0)
			{
				subTestChoice = savedSubTestChoice;
			}
		}

		return true;
	}

}

