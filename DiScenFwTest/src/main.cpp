//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw_tests.h>
#include <gpvulc/console/console_menu.h>
#include <gpvulc/console/console_util.h>

#ifdef _WIN32
#include <cstdlib>
#endif

using namespace discenfw_test;

// Run with a test number as argument to automatically start a test,
// e.g.: DiScenFwTest 1 4
int main(int argc, char* argv[])
{
	int testChoice = -1;
	int subTestChoice = -1;
	bool done = false;
	if (argc > 1)
	{
		testChoice = gpvulc::GetChoice(argv[1]);
	}
	if (argc > 2)
	{
		subTestChoice = gpvulc::GetChoice(argv[2]);
	}

	while (!done)
	{
		if (testChoice < 0)
		{
			gpvulc::ClearConsole();
			testChoice = gpvulc::GetMenuChoice({
				"Tic-tac-toe",
				"LED circuit",
				"Gridworld",
				"Framework debugging"
			}, "Quit");
		}

		switch (testChoice)
		{
		case 1:
			done = TestTicTacToePlugin(subTestChoice);
			break;
		case 2:
			done = TestLedCircuit(subTestChoice);
			break;
		case 3:
			done = TestGridworldPlugin(subTestChoice);
			break;
		case 4:
			done = TestDiScenFw(subTestChoice);
			break;
		default:
			return EXIT_SUCCESS;
		}
		testChoice = -1;
		subTestChoice = -1;
		std::cout << std::endl;
	}

	gpvulc::ConsolePause();
}


