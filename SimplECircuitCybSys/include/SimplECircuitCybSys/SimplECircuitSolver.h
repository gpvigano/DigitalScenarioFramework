//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "SimplECircuitData.h"

namespace simplecircuit_cybsys
{
	int ResistanceBetweenLeads(
		ElectronicCircuit& circuit,
		bool polarity, // true = +-, false = -+
		const std::shared_ptr<ElectronicComponentLead>& componentLead1,
		const std::shared_ptr<ElectronicComponentLead>& componentLead2);

	void SolvePowerSupplyDC(ElectronicCircuit& circuit, PowerSupplyDC& powerSupply);
	void SolveResistor(ElectronicCircuit& circuit, Resistor& resistor);
	void SolveLed(ElectronicCircuit& circuit, Led& led);
	void SolveSwitch(ElectronicCircuit& circuit, Switch& sw);

	/*!
	Make a simplified calculation of the circuit and update circuit components states.
	*/
	void SolveElectronicCircuit(ElectronicCircuit& circuit);
}
