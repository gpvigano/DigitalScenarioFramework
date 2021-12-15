//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "SimplECircuitCybSys/SimplECircuitData.h"
#include "SimplECircuitCybSys/SimplECircuitSolver.h"
#include <set>

using namespace simplecircuit_cybsys;

namespace
{
	int ResistanceBetweenLeads(
		ElectronicCircuit& circuit,
		bool polarity, // true = +-, false = -+
		const std::shared_ptr<ElectronicComponentLead>& componentLead1,
		const std::shared_ptr<ElectronicComponentLead>& componentLead2,
		int r, std::set<std::shared_ptr<ElectronicComponentLead>>& visitedLeads)
	{
		if (!componentLead1->Connected())
		{
			return -1;
		}
		for (const auto& compItem : componentLead1->Connections)
		{
			std::shared_ptr<ElectronicComponent> component = circuit.Components[compItem.Component];
			std::shared_ptr<ElectronicComponentLead> lead = component->Lead(compItem.Lead);
			if (lead != componentLead1 && visitedLeads.find(lead) == visitedLeads.end())
			{
				visitedLeads.insert(lead);
				if (lead == componentLead2)
				{
					return r;
				}

				const auto sw = AsSwitch(component);
				if (sw && !sw->BurntOut)
				{
					auto pin1 = sw->Lead("In");
					auto pin2 = sw->Position == SwitchPosition::POS1 ? sw->Lead("Out1") : sw->Lead("Out0");
					if (pin1 == lead || pin2 == lead)
					{
						auto otherPin = pin1 == lead ? pin2 : pin1;

						int rc = ResistanceBetweenLeads(circuit, polarity, otherPin, componentLead2, r, visitedLeads);
						if (rc >= 0)
						{
							return r + rc;
						}
						else
						{
							return -1;
						}
					}
				}

				const auto resistor = AsResistor(component);
				if (resistor && !resistor->BurntOut)
				{
					auto otherPin = resistor->OtherLead(lead);
					int rc = ResistanceBetweenLeads(circuit, polarity, otherPin, componentLead2, r, visitedLeads);
					if (rc >= 0)
					{
						return r + rc + resistor->Ohm;
					}
				}

				const auto led = AsLed(component);
				if (led && !led->BurntOut)
				{
					if (lead == led->Lead("Anode"))
					{
						if (!polarity)
						{
							return -1;
						}
						auto cathode = led->Lead("Cathode");
						int rc = ResistanceBetweenLeads(circuit, polarity, cathode, componentLead2, 0, visitedLeads);
						if (rc >= 0)
						{
							return rc;
						}
					}
					if (lead == led->Lead("Cathode"))
					{
						if (polarity)
						{
							return -1;
						}
						auto anode = led->Lead("Anode");
						int rc = ResistanceBetweenLeads(circuit, polarity, anode, componentLead2, 0, visitedLeads);
						if (rc >= 0)
						{
							return -1;
						}
					}
				}
			}
		}
		return -1;
	}

}


int simplecircuit_cybsys::ResistanceBetweenLeads(
	ElectronicCircuit& circuit, bool polarity, // true = +-, false = -+
	const std::shared_ptr<ElectronicComponentLead>& componentLead1,
	const std::shared_ptr<ElectronicComponentLead>& componentLead2)
{
	std::set<std::shared_ptr<ElectronicComponentLead>> visitedLeads;
	return ::ResistanceBetweenLeads(circuit, polarity, componentLead1, componentLead2, 0, visitedLeads);
}


void simplecircuit_cybsys::SolvePowerSupplyDC(ElectronicCircuit& circuit, PowerSupplyDC& powerSupply)
{
	std::shared_ptr<ElectronicComponentLead> posLead = powerSupply.GetPositiveLead();
	std::shared_ptr<ElectronicComponentLead> negLead = powerSupply.GetNegativeLead();
	int r1 = ResistanceBetweenLeads(circuit, true, posLead, negLead);
	int r2 = ResistanceBetweenLeads(circuit, false, negLead, posLead);
	if (r1 == 0 || r2 == 0)
	{
		powerSupply.BurntOut = true;
		return;
	}
}


void simplecircuit_cybsys::SolveResistor(ElectronicCircuit& circuit, Resistor& resistor)
{
	std::shared_ptr<PowerSupplyDC> powerSupply = circuit.GetPowerSupply();
	std::shared_ptr<ElectronicComponentLead> posLead = powerSupply->GetPositiveLead();
	std::shared_ptr<ElectronicComponentLead> negLead = powerSupply->GetNegativeLead();
	std::shared_ptr<ElectronicComponentLead> pin1 = resistor.Lead("Pin1");
	std::shared_ptr<ElectronicComponentLead> pin2 = resistor.Lead("Pin2");
	std::shared_ptr<ElectronicComponentLead> poweredPin = pin1;
	std::shared_ptr<ElectronicComponentLead> otherPin = pin2;
	int r1 = ResistanceBetweenLeads(circuit, true, posLead, poweredPin);
	if (r1 < 0)
	{
		// if no path from the consideredpin to positive lead, try with the other pin
		std::swap(poweredPin, otherPin);
		r1 = ResistanceBetweenLeads(circuit, true, posLead, poweredPin);
	}
	if (r1 < 0)
	{
		return; // resistor disconnected
	}
	int r2 = ResistanceBetweenLeads(circuit, true, otherPin, negLead);
	if (r2 < 0)
	{
		return; // resistor disconnected
	}
	// total resistance
	int R = r1 + r2 + resistor.Ohm;
	// get the power voltage
	int V = powerSupply->Voltage_mV;
	// calculate the current
	int I = R > 0 ? V / R : powerSupply->MaxCurrent_mA;
	// calculate the wattage
	int W = V * I / 1000;
	////int W = R * I * I; // Joule law: P = R*I^2
	if (W > resistor.Max_mW)
	{
		// it does not really bunrn out soon, it's just a simplification
		resistor.BurntOut = true;
	}
}


void simplecircuit_cybsys::SolveLed(ElectronicCircuit& circuit, Led& led)
{
	std::shared_ptr<PowerSupplyDC> powerSupply = circuit.GetPowerSupply();
	std::shared_ptr<ElectronicComponentLead> posLead = powerSupply->GetPositiveLead();
	std::shared_ptr<ElectronicComponentLead> negLead = powerSupply->GetNegativeLead();
	std::shared_ptr<ElectronicComponentLead> anode = led.Lead("Anode");
	std::shared_ptr<ElectronicComponentLead> cathode = led.Lead("Cathode");
	int ra = ResistanceBetweenLeads(circuit, true, posLead, anode);
	int rc = ResistanceBetweenLeads(circuit, true, cathode, negLead);
	if (ra < 0 || rc < 0 || !anode->Connected() || !cathode->Connected())
	{
		led.LitUp = false;
	}
	else
	{
		int r = ra + rc;
		int mV = powerSupply->Voltage_mV;
		int mA = r > 0 ? mV / r : powerSupply->MaxCurrent_mA;

		int dmA = mA - led.Type->TypicalWorkingCurrent_mA;

		int mVmin = led.Type->MinForwardVoltage_mV;
		int u = mV - mVmin;
		int rd = u / led.Type->TypicalWorkingCurrent_mA;
		int dmV = mA * rd;
		// TODO: V must be reduced by the forward voltage of each LED in series

		led.LitUp = !led.BurntOut && dmV > 0;
		// arbitrary set to 5mA more than typical current
		bool tooMuchCurrent = mA > led.Type->TypicalWorkingCurrent_mA + 5;
		// arbitrary set to 1V more than typical voltage
		bool tooMuchVoltage = dmV + mVmin > led.Type->TypicalForwardVoltage_mV + 1000;
		// it does not really burn out soon, it's just a simplification
		if (tooMuchCurrent || tooMuchVoltage)
		{
			led.BurntOut = true;
		}
		//led.LitUp = !led.BurntOut && dmV > 0;
	}
}


void simplecircuit_cybsys::SolveSwitch(ElectronicCircuit& circuit, Switch& sw)
{
	std::shared_ptr<PowerSupplyDC> powerSupply = circuit.GetPowerSupply();
	std::shared_ptr<ElectronicComponentLead> posLead = powerSupply->GetPositiveLead();
	std::shared_ptr<ElectronicComponentLead> negLead = powerSupply->GetNegativeLead();
	std::shared_ptr<ElectronicComponentLead> inLead = sw.Lead("In");
	std::shared_ptr<ElectronicComponentLead> out0Lead = sw.Lead("Out0");
	std::shared_ptr<ElectronicComponentLead> out1Lead = sw.Lead("Out1");
	if (inLead->Connected() && (out0Lead->Connected() || out1Lead->Connected()))
	{
		int rInPos = ResistanceBetweenLeads(circuit, true, posLead, inLead);
		int rInNeg = ResistanceBetweenLeads(circuit, false, negLead, inLead);
		bool inToPos = rInPos >= 0;
		bool inToNeg = rInNeg >= 0;
		if (inToPos != inToNeg) // in connected to +  XOR  in connected to -
		{
			int rIn = inToPos ? rInPos : rInNeg;
			int rOut = -1;
			bool swOn = sw.Position == SwitchPosition::POS1;
			std::shared_ptr<ElectronicComponentLead> outLead = swOn ? out1Lead : out0Lead;
			std::shared_ptr<ElectronicComponentLead> outPowerLead = inToPos ? negLead : posLead;
			rOut = ResistanceBetweenLeads(circuit, inToPos, outLead, outPowerLead);
			if (rOut >= 0)
			{
				int r = rIn + rOut;
				int mV = powerSupply->Voltage_mV;
				int mA = r > 0 ? mV / r : powerSupply->MaxCurrent_mA;
				int dmV = mA * r;
				bool tooMuchCurrent = mA > sw.MaxCurrent_mA;
				bool tooMuchVoltage = dmV > sw.MaxVoltage_mV;
				// it does not really burn out soon, it's just a simplification
				if (tooMuchCurrent || tooMuchVoltage)
				{
					sw.BurntOut = true;
				}
			}
		}
	}
}


void simplecircuit_cybsys::SolveElectronicCircuit(ElectronicCircuit& circuit)
{
	std::shared_ptr<PowerSupplyDC> powerSupply = circuit.GetPowerSupply();
	if (!powerSupply)
	{
		return;
	}

	std::shared_ptr<ElectronicComponentLead> posLead = powerSupply->GetPositiveLead();
	std::shared_ptr<ElectronicComponentLead> negLead = powerSupply->GetNegativeLead();
	powerSupply->Connected = (posLead->Connected() && negLead->Connected());
	for (const auto& compItem : circuit.Components)
	{
		std::shared_ptr<ElectronicComponent> component = compItem.second;
		if (component != powerSupply)
		{
			component->Connected = circuit.Connected(component, powerSupply);
		}
	}
	if (!powerSupply->Connected)
	{
		return;
	}
	std::vector< std::shared_ptr<Led> > leds;
	for (const auto& compItem : circuit.Components)
	{
		std::shared_ptr<ElectronicComponent> component = compItem.second;
		if (component->Connected && component->IsA("LED"))
		{
			std::shared_ptr<Led> led = AsLed(component);
			leds.push_back(led);
			SolveLed(circuit, *led);
		}
	}

	for (const auto& compItem : circuit.Components)
	{
		std::shared_ptr<ElectronicComponent> component = compItem.second;
		if (component->Connected && component->IsA("Resistor"))
		{
			auto resistorPtr = AsResistor(component);
			SolveResistor(circuit, *resistorPtr);
		}
	}

	for (const auto& compItem : circuit.Components)
	{
		std::shared_ptr<ElectronicComponent> component = compItem.second;
		if (component->Connected && component->IsA("Switch"))
		{
			auto swPtr = AsSwitch(component);
			SolveSwitch(circuit, *swPtr);
		}
	}

	if (leds.size() > 1)
	{
		for (const auto& led : leds)
		{
			SolveLed(circuit, *led);
		}
	}
	SolvePowerSupplyDC(circuit, *powerSupply);
}

