//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonWriterBase.h"
#include <discenfw/scen/ScenarioData.h>

#include <memory>

namespace discenfw
{
	namespace json
	{

		class JsonScenarioWriter : public JsonWriterBase
		{
		public:
			JsonScenarioWriter();

			void WriteScenario(const Scenario& scenarioData, std::string& jsonText);
		protected:

			void WriteSocket(const SocketInfo& socketInfo);
			void WriteAnchor(const Anchor& anchor);
			void WriteConnection(const std::shared_ptr<ConnectionElement>& connection);
			void WriteMutableElement(const std::shared_ptr<MutableElement>& mutableElement);
			void WriteElement(const std::shared_ptr<Element>& element);
			void WriteAggregate(const std::shared_ptr<Aggregate>& sys);
			void WriteGroupElement(const std::shared_ptr<GroupElement>& groupElement);
			void WriteEntityIdentity(const EntityIdentity& entity);
			void WriteEntity(const std::shared_ptr<Entity>& entity);
		};

	}
}
