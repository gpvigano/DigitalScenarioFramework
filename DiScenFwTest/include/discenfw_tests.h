//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

/*!
Digital Scenario Framework Tests.
*/
namespace discenfw_test
{

	///@addtogroup Tests
	///@{

	/*!
	Test the Digital Scenario Framework with the SimplECircuitCybSys.
	@param subTestChoice Selection number of specific test.
	@return Return true if the test was executed, false if not.
	*/
	bool TestLedCircuit(int subTestChoice);

	/*!
	Test the Digital Scenario Framework with the TicTacToeCybSys.
	@param subTestChoice Selection number of specific test.
	@return Return true if the test was executed, false if not.
	*/
	bool TestTicTacToePlugin(int subTestChoice);

	/*!
	Test the Digital Scenario Framework basic functionalities.
	@param subTestChoice Selection number of specific test.
	@return Return true if the test was executed, false if not.
	*/
	bool TestDiScenFw(int subTestChoice);

	/*!
	Test the Digital Scenario Framework with the Gridworld plugin.
	@param subTestChoice Selection number of specific test.
	@return Return true if the test was executed, false if not.
	*/
	bool TestGridworldPlugin(int subTestChoice);

	///@}
}
