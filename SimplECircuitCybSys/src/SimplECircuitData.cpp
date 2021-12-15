//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "SimplECircuitCybSys/SimplECircuitData.h"

#include <algorithm>

namespace simplecircuit_cybsys
{

	//---------------------------------------------------------------------
	// class ElectronicComponentLead

	bool ElectronicComponentLead::ConnectedTo(const std::string& component, const std::string& lead) const
	{
		for (const auto& conn : Connections)
		{
			if (conn.Component == component && conn.Lead == lead)
			{
				return true;
			}
		}
		return false;
	}


	bool ElectronicComponentLead::ConnectedTo(const std::string& component) const
	{
		for (const auto& conn : Connections)
		{
			if (conn.Component == component)
			{
				return true;
			}
		}
		return false;
	}

	//---------------------------------------------------------------------
	// class ElectronicComponent

	bool ElectronicComponent::Disconnect(const std::string& component)
	{
		bool found = false;
		for (const auto& item : Leads)
		{
			if (item.second->ConnectedTo(component))
			{
				for (size_t i = 0; i < item.second->Connections.size(); i++)
				{
					auto& conn = item.second->Connections[i];
					if (conn.Component == component)
					{
						item.second->Connections.erase(item.second->Connections.begin() + i);
						break;
					}
				}
				found = true;
			}
		}
		return found;
	}

	//---------------------------------------------------------------------
	// class ElectronicCircuit

	bool ElectronicCircuit::Connect(const std::string& componentName1, const std::string& leadName1,
		const std::string& componentName2, const std::string& leadName2)
	{
		std::shared_ptr<ElectronicComponent> component1 = FindComponent(componentName1);
		std::shared_ptr<ElectronicComponent> component2 = FindComponent(componentName2);
		if (!component1 || !component2)
		{
			return false;
		}

		std::shared_ptr<ElectronicComponentLead> lead1 = component1->Lead(leadName1);
		std::shared_ptr<ElectronicComponentLead> lead2 = component2->Lead(leadName2);

		if (!lead1 || !lead2)
		{
			return false;
		}

		// TODO: check if already present (impact on performance)
		lead1->Connections.push_back({ componentName2,leadName2 });
		lead2->Connections.push_back({ componentName1,leadName1 });

		return true;
	}


	bool ElectronicCircuit::GetCircuitComponents(std::vector<std::string>& foundComponents)
	{
		std::shared_ptr<PowerSupplyDC> powerSupply = GetPowerSupply();
		if (!powerSupply || !powerSupply->AllLeadsConnected())
		{
			return false;
		}
		foundComponents.clear();

		std::shared_ptr<ElectronicComponent> component = powerSupply;
		const std::string& powerSupplyName = GetComponentName(powerSupply);
		const std::string& firstCompName = powerSupply->Lead("+")->Connections[0].Component;
		const std::string& lastCompName = powerSupply->Lead("-")->Connections[0].Component;

		std::set<std::shared_ptr<ElectronicComponent>> visitedComponents;
		std::shared_ptr<ElectronicComponent> prevComponent;
		while (component)
		{
			visitedComponents.insert(component);
			std::vector<std::string> components;
			if (component == powerSupply)
			{
				components.push_back(firstCompName);
			}
			else
			{
				component->GetConnectedComponents(components);
			}
			if (components.empty())
			{
				return !foundComponents.empty();
			}
			for (const std::string& compName : components)
			{
				if (compName == lastCompName)
				{
					foundComponents.push_back(compName);
					return true;
				}
			}
			for (const std::string& compName : components)
			{
				std::shared_ptr<ElectronicComponent> connComponent = FindComponent(compName);
				if (visitedComponents.find(connComponent) == visitedComponents.end() && connComponent->GetConnectedLeadsCount() > 1)
				{
					component = connComponent;
					foundComponents.push_back(compName);
					break;
				}
				//else
				//{
				//	component = nullptr;
				//	break;
				//}
			}
			if (prevComponent == component)
			{
				// infinite loop detected
				return false;
			}
			prevComponent = component;
		}
		return false;
	}


	bool ElectronicCircuit::Connected(const std::shared_ptr<ElectronicComponent>& component1,
		const std::string& componentName2)
	{
		std::set<std::shared_ptr<ElectronicComponent>> visitedComponents;
		return Connected(component1,componentName2,visitedComponents);
	}


	bool ElectronicCircuit::Connected(const std::shared_ptr<ElectronicComponent>& component1,
		const std::shared_ptr<ElectronicComponent>& component2)
	{
		return Connected(component1, GetComponentName(component2));
	}


	bool ElectronicCircuit::Connected(const std::string& componentName1, const std::string& componentName2)
	{

		return Connected(FindComponent(componentName1), componentName2);
	}


	bool ElectronicCircuit::Disconnect(const std::string& componentName1, const std::string& leadName1)
	{
		if (Components.find(componentName1) == Components.end())
		{
			return false;
		}
		std::shared_ptr<ElectronicComponent> component1 = Components[componentName1];
		std::shared_ptr<ElectronicComponentLead> lead1 = component1->Lead(leadName1);
		if (!lead1)
		{
			return false;
		}
		std::vector<ElectronicComponentLeadId> leadsToDisconnect;
		for (auto conn: lead1->Connections)
		{
			leadsToDisconnect.push_back(conn);
		}
		lead1->Connections.clear();
		for (auto leadToDisconn : leadsToDisconnect)
		{
			Disconnect(componentName1, leadName1, leadToDisconn.Component, leadToDisconn.Lead);
		}
		return true;
	}


	bool ElectronicCircuit::Disconnect(const std::string& componentName1, const std::string& leadName1, const std::string& componentName2, const std::string& leadName2)
	{
		std::shared_ptr<ElectronicComponent> component1 = FindComponent(componentName1);
		std::shared_ptr<ElectronicComponent> component2 = FindComponent(componentName2);
		if (!component1 || !component2)
		{
			return false;
		}

		std::shared_ptr<ElectronicComponentLead> lead1 = component1->Lead(leadName1);
		std::shared_ptr<ElectronicComponentLead> lead2 = component2->Lead(leadName2);

		if (!lead1 || !lead2)
		{
			return false;
		}

		const auto& connItr1 = std::find_if(lead1->Connections.begin(),lead1->Connections.end(),
			[&componentName2,&leadName2](const ElectronicComponentLeadId& conn)
			{
				return conn.Component == componentName2 && conn.Lead == leadName2;
			}
		);
		if (connItr1 != lead1->Connections.end())
		{
			lead1->Connections.erase(connItr1);
		}

		const auto& connItr2 = std::find_if(lead2->Connections.begin(),lead2->Connections.end(),
			[&componentName1,&leadName1](const ElectronicComponentLeadId& conn)
			{
				return conn.Component == componentName1 && conn.Lead == leadName1;
			}
		);
		if (connItr2 != lead2->Connections.end())
		{
			lead2->Connections.erase(connItr2);
		}

		return true;
	}


	bool ElectronicCircuit::Disconnect(const std::string& component)
	{
		bool found = false;
		for (auto& otherComp : Components)
		{
			if(otherComp.second->Disconnect(component))
			{
				found = true;
			}
		}

		return found;
	}


	void ElectronicCircuit::DisconnectAll()
	{
		for (const auto& comp : Components)
		{
			std::vector< std::shared_ptr<ElectronicComponentLead> > leads;
			comp.second->GetLeads(leads);
			for (const auto& lead : leads)
			{
				lead->Connections.clear();
			}
			comp.second->Connected = false;
		}
	}


	void ElectronicCircuit::Reset()
	{
		for (auto& compPair : Components)
		{
			auto& component = compPair.second;
			component->Connected = false;
			component->BurntOut = false;
			std::shared_ptr<Led> led = AsLed(component);
			if (led)
			{
				led->LitUp = false;
			}
			std::shared_ptr<Switch> sw = AsSwitch(component);
			if (sw)
			{
				sw->Position = SwitchPosition::POS0;
			}
		}
		DisconnectAll();
	}


	std::shared_ptr<PowerSupplyDC> ElectronicCircuit::GetPowerSupply()
	{
		std::shared_ptr<PowerSupplyDC> powerSupply;
		for (auto& item : Components)
		{
			powerSupply = AsPowerSupplyDC(item.second);
			if (powerSupply) return powerSupply;
		}
		return nullptr;
	}


	std::shared_ptr<ElectronicComponent> ElectronicCircuit::FindComponent(const std::string& id)
	{
		auto found = Components.find(id);
		if (found != Components.end())
		{
			return found->second;
		}
		return nullptr;
	}


	std::string ElectronicCircuit::GetComponentName(const std::shared_ptr<ElectronicComponent>& component)
	{
		for (auto found : Components)
		{
			if (found.second == component)
			{
				return found.first;
			}
		}
		return "";
	}


	std::shared_ptr<PowerSupplyDC> ElectronicCircuit::AddPowerSupplyDC(const std::string& id, int mV, int mA)
	{
		if (Components.find(id) != Components.cend())
		{
			return nullptr;
		}
		std::shared_ptr<PowerSupplyDC> powerSupply = std::make_shared<PowerSupplyDC>();
		powerSupply->MaxCurrent_mA = mA;
		powerSupply->Voltage_mV = mV;
		Components[id] = powerSupply;
		return powerSupply;
	}


	std::shared_ptr<Switch> ElectronicCircuit::AddSwitch(const std::string& id, int mV, int mA)
	{
		if (Components.find(id) != Components.cend())
		{
			return nullptr;
		}
		std::shared_ptr<Switch> sw = std::make_shared<Switch>();
		sw->MaxCurrent_mA = mA;
		sw->MaxVoltage_mV = mV;
		Components[id] = sw;
		return sw;
	}


	std::shared_ptr<Resistor> ElectronicCircuit::AddResistor(const std::string& id, int ohm, int mW)
	{
		if (Components.find(id) != Components.cend())
		{
			return nullptr;
		}
		std::shared_ptr<Resistor> resistor = std::make_shared<Resistor>();
		resistor->Ohm = ohm;
		resistor->Max_mW = mW;
		Components[id] = resistor;
		return resistor;
	}


	std::shared_ptr<Led> ElectronicCircuit::AddLed(const std::string& id, const std::shared_ptr<LedType>& ledType)
	{
		if (Components.find(id) != Components.cend() || !ledType)
		{
			return nullptr;
		}
		std::shared_ptr<Led> led = std::make_shared<Led>(ledType);
		Components[id] = led;
		return std::shared_ptr<Led>();
	}



	//---------------------------------------------------------------------
	// Utility

	bool CheckComponentTypeName(const std::shared_ptr<ElectronicComponent>& component, const std::string& componentName)
	{
		return component && component->IsA(componentName);
	}


	std::shared_ptr<PowerSupplyDC> AsPowerSupplyDC(std::shared_ptr<ElectronicComponent> component)
	{
		if (!CheckComponentTypeName(component, "PowerSupplyDC")) return nullptr;
		return std::static_pointer_cast<PowerSupplyDC>(component);
	}


	std::shared_ptr<Switch> AsSwitch(std::shared_ptr<ElectronicComponent> component)
	{
		if (!CheckComponentTypeName(component, "Switch")) return nullptr;
		return std::static_pointer_cast<Switch>(component);
	}


	std::shared_ptr<Led> AsLed(std::shared_ptr<ElectronicComponent> component)
	{
		if (!CheckComponentTypeName(component, "LED")) return nullptr;
		return std::static_pointer_cast<Led>(component);
	}


	std::shared_ptr<Resistor> AsResistor(std::shared_ptr<ElectronicComponent> component)
	{
		if (!CheckComponentTypeName(component, "Resistor")) return nullptr;
		return std::static_pointer_cast<Resistor>(component);
	}


	//std::shared_ptr<Transistor> AsTransistor(std::shared_ptr<ElectronicComponent> component)
	//{
	//	if (!CheckComponentTypeName(component, "Transistor")) return nullptr;
	//	return std::static_pointer_cast<Transistor>(component);
	//}


	std::shared_ptr<LedType> GetLedType(const std::string& id)
	{
		std::shared_ptr<LedType> ledType;
		static std::map< std::string, std::shared_ptr<LedType> > ledTypeCatalog;
		if (ledTypeCatalog.empty())
		{
			{
				std::shared_ptr<LedType> ledType = std::make_shared<LedType>();
				ledType->Name = "Red";
				ledType->Color = LedColor::RED;
				ledType->TypicalForwardVoltage_mV = 2400;
				ledType->MinForwardVoltage_mV = 2100;
				ledType->TypicalWorkingCurrent_mA = 20;
				ledTypeCatalog[ledType->Name] = ledType;
			}
			{
				std::shared_ptr<LedType> ledType = std::make_shared<LedType>();
				ledType->Name = "Yellow";
				ledType->Color = LedColor::YELLOW;
				ledType->TypicalForwardVoltage_mV = 2400;
				ledType->MinForwardVoltage_mV = 2100;
				ledType->TypicalWorkingCurrent_mA = 20;
				ledTypeCatalog[ledType->Name] = ledType;
			}
			{
				std::shared_ptr<LedType> ledType = std::make_shared<LedType>();
				ledType->Name = "Green";
				ledType->Color = LedColor::GREEN;
				ledType->TypicalForwardVoltage_mV = 3800;
				ledType->MinForwardVoltage_mV = 3400;
				ledType->TypicalWorkingCurrent_mA = 20;
				ledTypeCatalog[ledType->Name] = ledType;
			}
			{
				std::shared_ptr<LedType> ledType = std::make_shared<LedType>();
				ledType->Name = "Blue";
				ledType->Color = LedColor::BLUE;
				ledType->TypicalForwardVoltage_mV = 3800;
				ledType->MinForwardVoltage_mV = 3400;
				ledType->TypicalWorkingCurrent_mA = 20;
				ledTypeCatalog[ledType->Name] = ledType;
			}
			{
				std::shared_ptr<LedType> ledType = std::make_shared<LedType>();
				ledType->Name = "White";
				ledType->Color = LedColor::WHITE;
				ledType->TypicalForwardVoltage_mV = 3800;
				ledType->MinForwardVoltage_mV = 3400;
				ledType->TypicalWorkingCurrent_mA = 20;
				ledTypeCatalog[ledType->Name] = ledType;
			}
			{
				std::shared_ptr<LedType> ledType = std::make_shared<LedType>();
				ledType->Name = "Warm white";
				ledType->Color = LedColor::WARM_WHITE;
				ledType->TypicalForwardVoltage_mV = 3800;
				ledType->MinForwardVoltage_mV = 3400;
				ledType->TypicalWorkingCurrent_mA = 20;
				ledTypeCatalog[ledType->Name] = ledType;
			}
		}
		if (ledTypeCatalog.find(id) == ledTypeCatalog.end())
		{
			return nullptr;
		}
		return ledTypeCatalog[id];
	}


	bool ElectronicCircuit::Connected(const std::shared_ptr<ElectronicComponent>& component1, const std::string& componentName2, std::set<std::shared_ptr<ElectronicComponent>>& visitedComponents)
	{
		if (!component1 || componentName2.empty())
		{
			return false;
		}
		std::vector<std::string> components;
		component1->GetConnectedComponents(components);

		// check if connected to nothing
		if (components.empty())
		{
			return false;
		}
		// check directly connected components
		for (const std::string& compName : components)
		{
			if (compName == componentName2)
			{
				return true;
			}
		}
		// prevent loops
		if (componentName2 == GetComponentName(component1))
		{
			return false;
		}
		// check indirectly connected components
		for (const std::string& compName : components)
		{
			std::shared_ptr<ElectronicComponent> connComponent = FindComponent(compName);
			bool alreadyVisited = (visitedComponents.find(connComponent) != visitedComponents.end());
			if (!alreadyVisited)
			{
				visitedComponents.insert(connComponent);
				if (Connected(connComponent, componentName2,visitedComponents))
				{
					return true;
				}
			}
		}
		return false;
	}



} // namespace simplecircuit_cybsys
