//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFwConfig.h>
#include <discenfw/scen/CommonData.h>
#include <discenfw/scen/ScenarioData.h>

#include <functional>
#include <string>

namespace discenfw
{
	namespace ve
	{

		/*!
		Application Programming Interface for a generic Virtual Environment.
		Each member must be set according to the specific implementation.
		*/
		struct DISCENFW_API VirtualEnvironmentAPI
		{

			/*!
			VE API level, used to select a subset of the API.
			*/
			enum VeApiLevel
			{
				LOGGING = 0x01, //!< Logger defined (see MessageLogger).
				SCENARIO = 0x02, //!< Scenario management API defined.
				SIMULATION = 0x04, //!< Simulation API defined.
				NAVIGATION = 0x08, //!< Navigation API defined.
				ALL_API = 0xFF //!< All API defined.
			};


			/*!
			Callback to get the VE project root directory.
			*/
			std::function<std::string(void)> ProjectDir;

			/// Synchronize the Virtual Environment with scenario
			/// @{

			/*!
			Synchronization callback (from VE scene to scenario data).
			*/
			std::function<void()> SyncScenario;

			/*!
			Synchronization callback (from scenario data to VE scene).
			*/
			std::function<void()> SyncScene;

			/// @}

			/// Synchronize the Virtual Environment objects with scenario elements
			/// @{

			/*!
			Synchronization callback (from VE object to scenario element).
			*/
			std::function<void(const std::string& entityId)> SyncElementTransform;

			/*!
			Synchronization callback (from scenario element to VE object).
			*/
			std::function<void(const std::string& entityId)> SyncSceneObjectTransform;

			/*!
			Linear interpolation callback for an element between two transformations.
			*/
			std::function<void(const std::string& elemId,
				const LocalTransform& transform1,
				const LocalTransform& transform2,
				float trim)>
				LerpElementTransform;

			/// @}

			/// Navigation management
			/// @{

			/*!
			Save a screenshot as image file at the given path.
			*/
			std::function<void(const std::string& filePath)> TakeScreenshot;

			/*!
			Move the viewpoint to the given position.
			*/
			std::function<void(Vector3D position)> MoveViewPointTo;

			/*!
			Rotate the viewpoint toward the given position.
			*/
			std::function<void(Vector3D position)> LookAt;

			/*!
			Completely define the viewpoint position and orientation.
			*/
			std::function<void(CoordSys3D coordSys)> SetViewPointCoordSys;

			//! Set the viewpoint direction vector.
			std::function<void(Vector3D direction)> SetViewPointDir;

			/// @}

			/*!
			Validate a specific VE API level,
			subset definition is checked on demand.
			*/
			bool Validate(int apiLevel = ALL_API);

			/*!
			Check if a specific VE API level was validated,
			subset definition is checked on demand.
			*/
			bool Validated(int apiLevel = ALL_API) const { return (ValidatedApiLevel&apiLevel) != 0; }

			//! Check if the VE API is completely defined (all subsets).
			bool Defined() const { return AllDefined; }

		private:

			int ApiLevel = 0;
			int ValidatedApiLevel = 0;
			bool AllDefined = false;
			std::string ErrorList;
		};
	}
}
