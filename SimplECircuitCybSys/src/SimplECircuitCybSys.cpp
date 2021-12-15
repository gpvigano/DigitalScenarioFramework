//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <SimplECircuitCybSys/SimplECircuitCybSys.h>
#include <SimplECircuitCybSys/SimplECircuitSolver.h>

#include <discenfw/xp/EnvironmentModel.h>
#include <discenfw/util/MessageLog.h>

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

#include <iostream>
#include <sstream>

using namespace discenfw::xp;

namespace
{
	inline std::string BoolToString(bool val) { return val ? "true" : "false"; }
}


namespace simplecircuit_cybsys
{

	SimplECircuitCybSys::SimplECircuitCybSys()
	{
		Circuit = std::make_unique<simplecircuit_cybsys::ElectronicCircuit>();
	}


	void SimplECircuitCybSys::CreateEntityStateTypes()
	{
		if (ComponentEntityType)
		{
			return;
		}

		ComponentEntityType = CreateEntityStateType(
			"",
			"Electronic Component",
			{
				{"connections","0"},
				{"connected","false"},
				{"burnt out","false"},
			},
			{
				{ "connected", { "true","false" } },
				{"burnt out", { "true","false" }},
			},
			{}
			);

		ResistorEntityType = CreateEntityStateType(
			"Electronic Component",
			"Resistor",
			{
			},
			{
				{"connections",{"0","1","2"}},
			},
			{ "Pin1", "Pin2" }
			);

		LedEntityType = CreateEntityStateType(
			"Electronic Component",
			"LED",
			{
				{"lit up","false"},
			},
			{
				{"connections",{"0","1","2"}},
				{ "lit up", { "true","false" } },
			},
			{ "Anode", "Cathode" }
			);

		PowerEntityType = CreateEntityStateType(
			"Electronic Component",
			"Power Supply",
			{
			},
			{
				{"connections",{"0","1","2"}},
			},
			{ "+", "-" }
			);

		SwitchEntityType = CreateEntityStateType(
			"Electronic Component",
			"Switch",
			{
				{"position","0"},
			},
			{
				{"connections",{"0","1","2","3"}},
				{ "position", { "0","1" } },
			},
			{ "In", "Out0", "Out1" }
		);
	}


	void SimplECircuitCybSys::ClearSystem()
	{
		Circuit->Components.clear();
	}


	void SimplECircuitCybSys::InitFailureConditions()
	{
		// set a default failure condition (any component burnt out)
		PropertyCondition burntOutCond({ "burnt out",BoolToString(true) });
		Condition anyBurntOut({ {EntityCondition::ANY,{burntOutCond}} });
		FailureCondition = anyBurntOut;
	}


	void SimplECircuitCybSys::InitRoles()
	{
		std::shared_ptr<EnvironmentModel> model = GetModel();
		if (model->GetRoleNames().empty())
		{
			PropertyCondition burntOutCond({ "burnt out",BoolToString(true) });
			Condition anyBurntOut({ { EntityCondition::ANY,{ burntOutCond } } });
			xp::SetRole(
				"Default",
				{}, // SuccessCondition
				{}, // FailureCondition
				{}, // DeadlockCondition
				{
					// ResultReward
					{
						{ ActionResult::IN_PROGRESS,0 },
						{ ActionResult::SUCCEEDED,1000 },
						{ ActionResult::FAILED,-1000 },
						{ ActionResult::DEADLOCK,-10 },
					},
				// PropertyCountRewards
					{},
				// EntityConditionRewards
					{
					},
					// FeatureRewards
					{
					}
				}
				);
		}
	}


	void SimplECircuitCybSys::ResetSystem()
	{
		Circuit->Reset();
	}


	bool SimplECircuitCybSys::ExecuteAction(const Action& action)
	{
		enum ActionId { CONNECT, SWITCH, DISCONNECT };
		static std::map<std::string, ActionId> actionNames{
			{ "connect",CONNECT },
			{ "switch",SWITCH },
			{ "disconnect",DISCONNECT },
		};
		switch (actionNames[action.TypeId])
		{
		case CONNECT:
			return DoConnectAction(action);
		case SWITCH:
			return DoSwitchAction(action);
		case DISCONNECT:
			return DoDisconnectAction(action);
		default:
			break;
		}
		return false;
	}


	void SimplECircuitCybSys::SynchronizeState(std::shared_ptr<xp::EnvironmentState> environmentState)
	{
		// TODO: alternative: just update existing entities and remove dangling entities?
		environmentState->Clear();

		// synchronize component states
		for (const auto& comp : Circuit->Components)
		{
			std::string entityId = comp.first;
			const auto& component = comp.second;
			std::shared_ptr<Led> led = AsLed(component);
			std::shared_ptr<Switch> sw = AsSwitch(component);
			std::shared_ptr<PowerSupplyDC> powerSupply = AsPowerSupplyDC(component);
			std::shared_ptr<Resistor> resistor = AsResistor(component);

			// get the properties of the current component
			bool nowBurnt = component->BurntOut;
			bool nowConnected = component->Connected;
			int nowConn = component->GetConnectedLeadsCount();
			bool nowLit = led && led->LitUp;
			int nowPos = (int)(sw && sw->Position == SwitchPosition::POS1);

			// update the entity state for the current component
			std::shared_ptr<EntityState> entState = environmentState->GetEntityState(entityId);
			if (!entState)
			{
				std::shared_ptr<EntityStateType> entType;

				if (!ComponentEntityType)
				{
					CreateEntityStateTypes();
				}

				if (powerSupply) entType = PowerEntityType;
				else if (led) entType = LedEntityType;
				else if (sw) entType = SwitchEntityType;
				else if (resistor) entType = ResistorEntityType;
				//else if (transistor) entType = TransistorEntityType;
				else entType = ComponentEntityType;

				entState = EntityState::Make(entType->GetTypeName(),entType->GetModelName());
				//entState = std::make_shared<EntityState>(entType->GetTypeName(),entType->GetModelName());
				environmentState->SetEntityState(entityId, entState);
			}

			std::vector< std::pair<std::string, std::shared_ptr<ElectronicComponentLead> >> leads;
			component->GetLeads(leads);

			// store new component relationships
			for (const auto& eLead : leads)
			{
				if (!eLead.second->Connections.empty())
				{
					RelationshipLink link;
					link.EntityId = eLead.second->Connections[0].Component;
					link.LinkId = eLead.second->Connections[0].Lead;
					entState->SetRelationship(eLead.first, link);
				}
				else
				{
					entState->RemoveRelationship(eLead.first);
				}
			}


			// set the properties for the entity state
			entState->SetPropertyValue("burnt out", BoolToString(nowBurnt));
			entState->SetPropertyValue("connected", BoolToString(nowConnected));
			entState->SetPropertyValue("connections", std::to_string(nowConn));

			if (led)
			{
				entState->SetPropertyValue("lit up", BoolToString(nowLit));
			}

			if (sw)
			{
				entState->SetPropertyValue("position", std::to_string(nowPos));
			}

			if (LogEnabled)
			{
				if (nowBurnt)
				{
					LogStream << entityId << " => burnt out" << std::endl;
					FlushLog(LOG_DEBUG);
				}
				if (nowLit)
				{
					LogStream << entityId << " => lit up" << std::endl;
					FlushLog(LOG_DEBUG);
				}

				if (nowPos)
				{
					LogStream << entityId << " => on" << std::endl;
					FlushLog(LOG_DEBUG);
				}
			}
		}
	}


	const std::vector<ActionRef>& SimplECircuitCybSys::GetAvailableActions(
		const std::string& roleId,
		bool smartSelection
		) const
	{
		CachedAvailableActions.clear();
		std::shared_ptr<PowerSupplyDC> powerSupply = Circuit->GetPowerSupply();
		bool circuitIsComplete = false;
		if (smartSelection)
		{
			if(powerSupply && powerSupply->GetConnectedLeadsCount()==2)
			{
				circuitIsComplete = true;
				for (const auto& compItem : Circuit->Components)
				{
					// Check if at least one component is connected but not fully connected
					// (power supply was already checked)
					if (compItem.second != powerSupply && compItem.second->GetConnectedLeadsCount() == 1)
					{
						circuitIsComplete = false;
						break;
					}
				}
			}
		}

		std::vector<std::string> actionNames{ "connect", "switch" };
		for (const auto& compItem : Circuit->Components)
		{
			std::string compId = compItem.first;
			std::shared_ptr<ElectronicComponent> component = compItem.second;
			bool powerSupplyConnected = powerSupply && powerSupply->AnyLeadConnected();
			std::string powerId = Circuit->GetComponentName(powerSupply);
			if (!component->BurntOut)
			{
				auto sw = AsSwitch(component);

				if (sw)
				{
					bool canBeSwitched = true;
					if (smartSelection)
					{
						canBeSwitched = circuitIsComplete && sw->GetConnectedLeadsCount() > 1 && sw->Lead("In")->Connected();
					}
					// possible deadlock: repeatedly switching --> deadlock detection required
					if (canBeSwitched)
					{
						CacheAvailableAction(
							{ "switch",{ compId,sw->Position == SwitchPosition::POS0 ? "1" : "0" } }
							);
					}
					// alternative solution:
					// allow switch-on only, this avoids possible deadlocks
					//if (canBeSwitched && sw->Position == SwitchPosition::POS0)
					//...
				}


				// if (!component->AllLeadsConnected()) ...

				// allow only 2 connections per component to simplify the circuit simulation
				if (component->GetConnectedLeadsCount() < 2)
				{
					std::vector< std::shared_ptr<ElectronicComponentLead> > leads;
					std::vector< std::string > leadNames;
					component->GetLeads(leads);
					component->GetLeadNames(leadNames);
					for (const auto& otherCompItem : Circuit->Components)
					{
						std::string otherCompId = otherCompItem.first;
						std::shared_ptr<ElectronicComponent> otherComponent = otherCompItem.second;
						bool connectingPowerSupply = (otherComponent == powerSupply || component == powerSupply);
						bool connectedToPowerSupply = (otherComponent->Connected || component->Connected);
						bool canBeConnected =
							(!otherComponent->BurntOut
								&& otherComponent != component
								&& !otherComponent->AllLeadsConnected()
								&& !component->ConnectedTo(otherCompId)
								);
						if (smartSelection)
						{
							canBeConnected = canBeConnected && (connectingPowerSupply || connectedToPowerSupply);
						}
						if (canBeConnected)
						{
							std::vector< std::shared_ptr<ElectronicComponentLead> > otherLeads;
							std::vector< std::string > otherLeadNames;
							otherComponent->GetLeads(otherLeads);
							otherComponent->GetLeadNames(otherLeadNames);
							for (size_t i = 0; i < leads.size(); i++)
							{
								const auto& lead = leads[i];
								// allow only 1 connection per lead to simplify the circuit simulation
								if (!lead->Connected())
								{
									for (size_t j = 0; j < otherLeads.size(); j++)
									{
										const auto& otherLead = otherLeads[j];

										if (!otherLead->Connected())
										{
											// prevent parallel connections of components to simplify the circuit simulation
											//if (otherLead != lead && !otherLead->ConnectedTo(compId, leadNames[i]))
											if (otherLead != lead && !otherLead->ConnectedTo(compId))
											{
												CacheAvailableAction(
													{ "connect",
														{
															compId, leadNames[i],
															otherCompId, otherLeadNames[j]
														}
													}
													);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		return CachedAvailableActions;
	}


	bool SimplECircuitCybSys::DoConnectAction(const Action& action)
	{
		// decode parameters
		const std::string& component1Id = action.Params[0];
		const std::string& lead1Id = action.Params[1];
		const std::string& component2Id = action.Params[2];
		const std::string& lead2Id = action.Params[3];

		// prevent connecting a lead with itself
		if (component1Id == component2Id && lead1Id == lead2Id)
		{
			return false;
		}

		std::shared_ptr<ElectronicComponent> component1 = Circuit->FindComponent(component1Id);
		std::shared_ptr<ElectronicComponent> component2 = Circuit->FindComponent(component2Id);

		// check if components exist
		if (!component1 || !component2)
		{
			return false;
		}

		// check if already connected
		if (component1->ConnectedTo(component2Id, lead2Id))
		{
			return false;
		}

		// allow only 1 connection per lead to simplify the circuit simulation
		if (component1->Lead(lead1Id)->Connected() || component2->Lead(lead2Id)->Connected())
		{
			return false;
		}

		if (LogEnabled)
		{
			LogStream << "> " << component1Id << "/" << lead1Id
				<< " <--> " << component2Id << "/" << lead2Id << std::endl;
			FlushLog(LOG_DEBUG);
		}

		// update the circuit
		Circuit->Connect(component1Id, lead1Id, component2Id, lead2Id);
		SolveElectronicCircuit(*Circuit);

		return true;
	}


	bool SimplECircuitCybSys::DoSwitchAction(const Action& action)
	{
		// decode parameters
		const std::string& switchId = action.Params[0];
		const std::string& posId = action.Params[1];
		bool pos = std::atoi(posId.c_str()) > 0;

		auto sw = AsSwitch(Circuit->FindComponent(switchId));
		if (!sw)
		{
			return false;
		}

		if (LogEnabled)
		{
			LogStream << std::noboolalpha << "> " << switchId << ": " << pos << std::endl;
			FlushLog(LOG_DEBUG);
		}

		// update the circuit
		sw->Position = (SwitchPosition)pos;
		SolveElectronicCircuit(*Circuit);
		return true;
	}


	bool SimplECircuitCybSys::DoDisconnectAction(const Action& action)
	{
		size_t numParams = action.Params.size();
		if(numParams!=2 && numParams!=4)
		{
			return false;
		}

		// decode parameters
		const std::string& component1Id = action.Params[0];
		const std::string& lead1Id = action.Params[1];
		std::shared_ptr<ElectronicComponent> component1 = Circuit->FindComponent(component1Id);

		if (!component1->Lead(lead1Id)->Connected())
		{
			return false;
		}
		if (numParams == 2)
		{
			// update the circuit
			Circuit->Disconnect(component1Id, lead1Id);
		}
		else //if (numParams == 4)
		{
			const std::string& component2Id = action.Params[2];
			const std::string& lead2Id = action.Params[3];

			// prevent connecting a lead with itself
			if (component1Id == component2Id && lead1Id == lead2Id)
			{
				return false;
			}

			std::shared_ptr<ElectronicComponent> component2 = Circuit->FindComponent(component2Id);

			// check if components exist
			if (!component1 || !component2)
			{
				return false;
			}

			// check if already connected
			if (!component1->ConnectedTo(component2Id, lead2Id))
			{
				return false;
			}

			if (!component2->Lead(lead2Id)->Connected())
			{
				return false;
			}

			if (LogEnabled)
			{
				LogStream << "> " << component1Id << "/" << lead1Id
					<< " X--X " << component2Id << "/" << lead2Id << std::endl;
				FlushLog(LOG_DEBUG);
			}

			// update the circuit
			Circuit->Disconnect(component1Id, lead1Id, component2Id, lead2Id);
		}
		SolveElectronicCircuit(*Circuit);

		return true;
	}


	std::shared_ptr<ElectronicComponent> SimplECircuitCybSys::CreateComponentFromConfiguration(
		const std::string& config, std::string& compId)
	{
		std::istringstream iStr(config);

		if (!iStr.good())
		{
			return nullptr;
		}
		std::shared_ptr<ElectronicComponent> comp;

		std::string compType;

		iStr >> compType >> compId;
		comp = MakeComponent(compType);

		if (comp)
		{
			ReadComponentConfiguration(iStr, comp);
			Circuit->Components[compId] = comp;
		}
		return comp;
	}


	const std::string SimplECircuitCybSys::GetComponentConfiguration(const std::string& compId)
	{
		std::shared_ptr<ElectronicComponent> comp = Circuit->FindComponent(compId);
		if (!comp)
		{
			return "";
		}
		std::ostringstream oStr;
		std::shared_ptr<Led> led = AsLed(comp);
		if (led)
		{
			oStr << led->Type->Name;
		}
		std::shared_ptr<Resistor> r = AsResistor(comp);
		if (r)
		{
			oStr << r->Ohm
				<< " " << r->Max_mW;
		}
		std::shared_ptr<Switch> sw = AsSwitch(comp);
		if (sw)
		{
			oStr << sw->MaxVoltage_mV
				<< " " << sw->MaxCurrent_mA;
		}
		std::shared_ptr<PowerSupplyDC> powerSupply = AsPowerSupplyDC(comp);
		if (powerSupply)
		{
			oStr << powerSupply->Voltage_mV
				<< " " << powerSupply->MaxCurrent_mA;
		}
		return oStr.str();
	}


	bool SimplECircuitCybSys::SetComponentConfiguration(const std::string& compId, const std::string& config)
	{
		std::shared_ptr<ElectronicComponent> comp = Circuit->FindComponent(compId);
		if (!comp)
		{
			return false;
		}
		std::istringstream iStr(config);
		ReadComponentConfiguration(iStr, comp);
		return true;
	}


	bool SimplECircuitCybSys::SetConfiguration(const std::string& config)
	{
		Initialize();
		Circuit->Components.clear();
		bool firstRead = true;
		std::istringstream iStr(config);
		while (iStr.good())
		{
			std::string compConfig;
			std::getline(iStr, compConfig);
			std::string compId;
			std::shared_ptr<ElectronicComponent> comp = CreateComponentFromConfiguration(
				compConfig, compId);
			if (firstRead &&!comp)
			{
				return false;
			}
			firstRead = false;
		}
		Initialize(true);
		return true;
	}


	const std::string SimplECircuitCybSys::GetConfiguration()
	{
		std::ostringstream oStr;
		for (const auto& comp : Circuit->Components)
		{
			oStr << comp.second->GetTypeName()
				<< " " << comp.first << " ";
			oStr << GetComponentConfiguration(comp.first);
			oStr << "\n";
		}
		return oStr.str();
	}


	const std::string SimplECircuitCybSys::ReadEntityConfiguration(const std::string& entityId)
	{
		return GetComponentConfiguration(entityId);
	}


	bool SimplECircuitCybSys::WriteEntityConfiguration(const std::string& entityId, const std::string& config)
	{
		return SetComponentConfiguration(entityId, config);
	}


	bool SimplECircuitCybSys::ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config)
	{
		std::shared_ptr<ElectronicComponent> comp;
		if (Circuit->Components.find(entityId) != Circuit->Components.end())
		{
			comp = Circuit->Components[entityId];
			if (comp->GetTypeName() != entityType)
			{
				Circuit->Components.erase(entityId);
				comp = nullptr;
			}
		}

		if (!comp)
		{
			comp = MakeComponent(entityType);
			if (!comp)
			{
				return false;
			}
			Circuit->Components[entityId] = comp;
		}
		std::istringstream iStr(config);
		ReadComponentConfiguration(iStr, comp);
		return true;
	}


	bool SimplECircuitCybSys::RemoveEntity(const std::string& entityId)
	{
		if (Circuit->Components.find(entityId) != Circuit->Components.end())
		{
			std::shared_ptr<ElectronicComponent> comp = Circuit->Components[entityId];
			Circuit->Disconnect(entityId);
			Circuit->Components.erase(entityId);
			return true;
		}
		return false;
	}


	const std::string SimplECircuitCybSys::GetSystemInfo(const std::string& infoId) const
	{
		std::ostringstream oStr;
		if (infoId == "" || infoId == "CircuitSchema")
		{
			// create a schematic representation of the circuit in one text line

			std::shared_ptr<PowerSupplyDC> powerSupply = Circuit->GetPowerSupply();
			if (!powerSupply) return "";
			std::shared_ptr<ElectronicComponentLead> posLead = powerSupply->GetPositiveLead();
			std::shared_ptr<ElectronicComponentLead> negLead = powerSupply->GetNegativeLead();
			if (!posLead->Connected() || !negLead->Connected()) return "";
			int connectedComponents = 0;
			for (const auto& comp : Circuit->Components)
			{
				if (comp.second->GetConnectedLeadsCount() > 1) connectedComponents++;
			}

			std::vector<std::string> foundComponents;
			bool closedCircuit = Circuit->GetCircuitComponents(foundComponents);
			oStr << " (+)-";
			for (std::string compName : foundComponents)
			{
				const auto& comp = Circuit->Components[compName];
				const auto& sw = AsSwitch(comp);
				const auto& led = AsLed(comp);
				oStr << "-{" << compName;
				if (led && led->LitUp) oStr << "*";
				if (sw && sw->Position == SwitchPosition::POS0)	oStr << "o";
				if (sw && sw->Position == SwitchPosition::POS1)	oStr << "*";
				oStr << "}-";
			}

			if (closedCircuit) oStr << "-(-) {" << connectedComponents << "}";
		}
		else if (infoId == "CircuitInfo")
		{
			// List circuit components and their state

			for (const auto& compPair : Circuit->Components)
			{
				const auto& component = compPair.second;
				if (component->GetConnectedLeadsCount() > 1)
				{
					oStr << compPair.first;
					const auto& sw = AsSwitch(component);
					if (sw)	oStr << "#" << (int)sw->Position;
					const auto& led = AsLed(component);
					if (led && led->LitUp) oStr << "*";
					if (component->BurntOut)
					{
						oStr << "[burnt out]";
					}
					std::vector< std::shared_ptr<ElectronicComponentLead> > leads;
					component->GetLeads(leads);
					if (!leads.empty())
					{
						oStr << "@";
					}
					bool first = true;
					for (const auto& connLead : leads)
					{
						if (connLead->Connected())
						{
							if (!first) oStr << ",";
							first = false;
							oStr << connLead->Name << ">" << connLead->Connections[0].Component
								<< "." << connLead->Connections[0].Lead;
						}
					}
					oStr << " ";
				}
			}
		}

		std::string info = oStr.str();
		return info;
	}


	void SimplECircuitCybSys::ReadComponentConfiguration(std::istringstream& iStr, std::shared_ptr<ElectronicComponent> comp)
	{
		std::shared_ptr<Led> led = AsLed(comp);
		if (led)
		{
			std::string ledType;
			iStr >> ledType;
			led->Type = GetLedType(ledType);
			if (!led->Type)
			{
				LogStream << "Unknown LED type: " << ledType << std::endl;
				FlushLog(LOG_ERROR);
			}

		}
		std::shared_ptr<Resistor> r = AsResistor(comp);
		if (r)
		{
			iStr >> r->Ohm >> r->Max_mW;
		}
		std::shared_ptr<Switch> sw = AsSwitch(comp);
		if (sw)
		{
			iStr >> sw->MaxVoltage_mV >> sw->MaxCurrent_mA;
		}
		std::shared_ptr<PowerSupplyDC> powerSupply = AsPowerSupplyDC(comp);
		if (powerSupply)
		{
			iStr >> powerSupply->Voltage_mV >> powerSupply->MaxCurrent_mA;
		}

	}


	std::shared_ptr<ElectronicComponent> SimplECircuitCybSys::MakeComponent(const std::string& compType)
	{
		std::shared_ptr<ElectronicComponent> comp;
		if (compType == "PowerSupplyDC")
		{
			comp = std::make_shared<PowerSupplyDC>();
		}
		else if (compType == "LED")
		{
			comp = std::make_shared<Led>(nullptr);
		}
		else if (compType == "Resistor")
		{
			comp = std::make_shared<Resistor>();
		}
		else if (compType == "Switch")
		{
			comp = std::make_shared<Switch>();
		}
		return comp;
	}


	extern "C" BOOST_SYMBOL_EXPORT SimplECircuitCybSys CyberSystem;
	SimplECircuitCybSys CyberSystem;


} // namespace simplecircuit_cybsys
