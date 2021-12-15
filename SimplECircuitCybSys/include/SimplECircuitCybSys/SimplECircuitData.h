//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <memory>
#include <vector>
#include <map>
#include <set>

namespace simplecircuit_cybsys
{
	/*!
	Symbolic description of an electronic component lead.
	*/
	struct ElectronicComponentLeadId
	{
		std::string Component;
		std::string Lead;
	};


	/*!
	Electronic component lead data.
	*/
	struct ElectronicComponentLead
	{
		std::string Name;
		std::vector<ElectronicComponentLeadId> Connections;
		bool Connected() const { return !Connections.empty(); }
		bool ConnectedTo(const std::string& component, const std::string& lead) const;
		bool ConnectedTo(const std::string& component) const;
	};


	/*!
	Electronic component data.
	*/
	struct ElectronicComponent
	{
		bool BurntOut = false;
		bool Connected = false;

		ElectronicComponent() {}


		void GetConnectedComponents(std::vector<std::string>& components) const
		{
			components.clear();
			for (const auto& lead : Leads)
			{
				for (const auto& conn : lead.second->Connections)
				{
					components.push_back(conn.Component);
				}
			}
		}


		bool ConnectedTo(const std::string& component) const
		{
			for (const auto& item : Leads)
			{
				if (item.second->ConnectedTo(component))
				{
					return true;
				}
			}
			return false;
		}


		bool Disconnect(const std::string& component);


		bool ConnectedTo(const std::string& component, const std::string& lead) const
		{
			for (const auto& item : Leads)
			{
				if (item.second->ConnectedTo(component, lead))
				{
					return true;
				}
			}
			return false;
		}


		bool AnyLeadConnected() const
		{
			for (const auto& item : Leads)
			{
				if (item.second->Connected())
				{
					return true;
				}
			}
			return false;
		}


		bool AllLeadsConnected() const
		{
			for (const auto& item : Leads)
			{
				if (!item.second->Connected())
				{
					return false;
				}
			}
			return true;
		}


		int GetLeadsCount() const
		{
			return (int)Leads.size();
		}


		int GetConnectedLeadsCount() const
		{
			int count = 0;
			for (const auto& item : Leads)
			{
				if (item.second->Connected()) count++;
			}
			return count;
		}


		void GetLeads(std::vector< std::shared_ptr<ElectronicComponentLead> >& leads) const
		{
			leads.clear();
			for (const auto& item : Leads)
			{
				leads.push_back(item.second);
			}
		}


		void GetLeads(std::vector< std::pair<std::string, std::shared_ptr<ElectronicComponentLead> >>& leads) const
		{
			leads.clear();
			for (const auto& item : Leads)
			{
				leads.push_back(item);
			}
		}


		void GetLeadNames(std::vector<std::string>& leadNames) const
		{
			for (const auto& item : Leads)
			{
				leadNames.push_back(item.first);
			}
		}


		std::shared_ptr<ElectronicComponentLead> Lead(const std::string& leadName)
		{
			return Leads.find(leadName) != Leads.end() ? Leads[leadName] : nullptr;
		}


		bool IsA(const std::string& componentName) const
		{
			return ComponentDefined.find(componentName) != ComponentDefined.cend();
		}


		std::string GetTypeName() const
		{
			return ComponentType;
		}

	protected:

		std::map<std::string, std::shared_ptr<ElectronicComponentLead> > Leads;
		std::map<std::string, bool > ComponentDefined;

		std::string ComponentType;


		void DefineComponent(const std::string& componentName)
		{
			ComponentDefined[componentName] = true;
			ComponentType = componentName;
		}


		std::shared_ptr<ElectronicComponentLead> DefineLead(const std::string& leadName)
		{
			std::shared_ptr<ElectronicComponentLead> newLead = std::make_shared<ElectronicComponentLead>();
			newLead->Name = leadName;
			Leads[leadName] = newLead;
			return newLead;
		}
	};


	/*!
	Power supply with direct current.
	*/
	struct PowerSupplyDC : public ElectronicComponent
	{
		int MaxCurrent_mA;
		int Voltage_mV;

		PowerSupplyDC()
		{
			DefineComponent("PowerSupplyDC");
			PositiveLead = DefineLead("+");
			NegativeLead = DefineLead("-");
		}

		std::shared_ptr<ElectronicComponentLead> GetPositiveLead() { return PositiveLead; }

		std::shared_ptr<ElectronicComponentLead> GetNegativeLead() { return NegativeLead; }

	protected:
		std::shared_ptr<ElectronicComponentLead> PositiveLead;
		std::shared_ptr<ElectronicComponentLead> NegativeLead;
	};


	/*!
	Switch position on a switch circuit component.
	*/
	enum class SwitchPosition
	{
		POS0 = 0,
		POS1 = 1,
	};


	/*!
	Switch circuit component.
	*/
	struct Switch : public ElectronicComponent
	{
		int MaxVoltage_mV = 50000;
		int MaxCurrent_mA = 2000;

		SwitchPosition Position = SwitchPosition::POS0;


		Switch()
		{
			DefineComponent("Switch");
			DefineLead("In");
			DefineLead("Out0");
			DefineLead("Out1");
		}
	};


	/*!
	LED color type.
	*/
	enum class LedColor
	{
		RED,
		YELLOW,
		GREEN,
		BLUE,
		WHITE,
		WARM_WHITE
	};


	/*!
	LED model type.
	*/
	struct LedType
	{
		std::string Name;
		int MinForwardVoltage_mV = 2100;
		int TypicalForwardVoltage_mV = 2400;
		int TypicalWorkingCurrent_mA = 20;
		LedColor Color = LedColor::RED;
		bool SuperBright = false;
	};


	/*!
	LED electronic component.
	*/
	struct Led : public ElectronicComponent
	{
		std::shared_ptr<LedType> Type;
		bool LitUp = false;


		Led(std::shared_ptr<LedType> ledType)
			: Type(ledType)
		{
			DefineComponent("LED");
			DefineLead("Anode");
			DefineLead("Cathode");
		}
	};


	/*!
	Resistor electronic component.
	*/
	struct Resistor : public ElectronicComponent
	{
		int Ohm;
		int Max_mW;


		Resistor()
		{
			DefineComponent("Resistor");
			DefineLead("Pin1");
			DefineLead("Pin2");
		}

		std::shared_ptr<ElectronicComponentLead> OtherLead(std::shared_ptr<ElectronicComponentLead> lead)
		{
			return Leads["Pin1"] == lead ? Leads["Pin2"] : Leads["Pin1"];
		}
	};

	//enum class TransistorPolarization
	//{
	//	NPN,
	//	PNP
	//};

	//struct TransistorType
	//{
	//	std::string ModelType;
	//	TransistorPolarization Polarization;
	//	// Many other parameters...
	//};

	//struct Transistor : public ElectronicComponent
	//{
	//	std::shared_ptr<TransistorType> Type;

	//	Transistor(std::shared_ptr<TransistorType> trType)
	//		: Type(trType)
	//	{
	//		DefineComponent("Transistor");
	//		DefineLead("Emitter");
	//		DefineLead("Base");
	//		DefineLead("Collector");
	//	}
	//};

	/*!
	Digital model of an electronic circuit.
	*/
	struct ElectronicCircuit
	{
		std::map< std::string, std::shared_ptr<ElectronicComponent> > Components;

		bool Connect(const std::string& componentName1, const std::string& leadName1,
			const std::string& componentName2, const std::string& leadName2);

		bool Connected(const std::shared_ptr<ElectronicComponent>& component1, const std::string& componentName2);

		bool Connected(const std::string& componentName1, const std::string& componentName2);

		bool Connected(const std::shared_ptr<ElectronicComponent>& component1,
			const std::shared_ptr<ElectronicComponent>& component2);

		bool GetCircuitComponents(std::vector<std::string>& foundComponents);


		bool Disconnect(const std::string& componentName1, const std::string& leadName1);

		bool Disconnect(const std::string& componentName1, const std::string& leadName1,
			const std::string& componentName2, const std::string& leadName2);

		bool Disconnect(const std::string& component);

		void DisconnectAll();

		void Reset();

		std::shared_ptr<PowerSupplyDC> GetPowerSupply();

		std::shared_ptr<ElectronicComponent> FindComponent(const std::string& id);

		std::string GetComponentName(const std::shared_ptr<ElectronicComponent>& component);


		std::shared_ptr<PowerSupplyDC> AddPowerSupplyDC(const std::string& id, int mV, int mA);

		std::shared_ptr<Switch> AddSwitch(const std::string& id, int mV, int mA);

		std::shared_ptr<Resistor> AddResistor(const std::string& id, int ohm, int mW);

		std::shared_ptr<Led> AddLed(const std::string& id, const std::shared_ptr<LedType>& ledType);

		//circuit.AddTransistor(...);

	private:

		bool Connected(const std::shared_ptr<ElectronicComponent>& component1, const std::string& componentName2, std::set<std::shared_ptr<ElectronicComponent>>& visitedComponents);
	};

	/*!
	Check if a component is of the named type.
	*/
	bool CheckComponentTypeName(const std::shared_ptr<ElectronicComponent>& component, const std::string& componentName);

	///@name Convenience cast methods
	///@{

	std::shared_ptr<PowerSupplyDC> AsPowerSupplyDC(std::shared_ptr<ElectronicComponent> component);

	std::shared_ptr<Switch> AsSwitch(std::shared_ptr<ElectronicComponent> component);

	std::shared_ptr<Led> AsLed(std::shared_ptr<ElectronicComponent> component);

	std::shared_ptr<Resistor> AsResistor(std::shared_ptr<ElectronicComponent> component);

	//std::shared_ptr<Transistor> AsTransistor(std::shared_ptr<ElectronicComponent> component);

	///@}

	/*!
	Convert a string to LedType.
	*/
	std::shared_ptr<LedType> GetLedType(const std::string& id);
}

