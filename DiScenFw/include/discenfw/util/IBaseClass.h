//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <limits>

namespace discenfw
{
	/*!
	Basic class with RTTI methods.

	Sample implementations:
	@code
		class FirstDerivedClass : public IBaseClass
		{
			public:
			//...

			virtual const char* GetClassName() const override { return "FirstDerivedClass"; }

			virtual bool IsA(const std::string& classType) const override
			{ return classType == FirstDerivedClass::GetClassName(); }
		}
	@endcode
		or (deeper level of inheritance):
	@code
		// DerivedClass can be derived from a descendant of IBaseClass (e.g. FirstDerivedClass)
		class DerivedDerivedClass : public DerivedClass
		{
			public:
			//...

			virtual const char* GetClassName() const override { return "DerivedDerivedClass"; }

			virtual bool IsA(const std::string& classType) const override
			{ return classType == DerivedDerivedClass::GetClassName() || DerivedClass::IsA(classType); }
		}
	@endcode

	@note Note that dynamic_cast requires RTTI enabled, this implementation does not require it.
	*/
	class IBaseClass
	{
	public:
		IBaseClass() {}

		virtual ~IBaseClass() {}

		/*!
		Get the real class name.
		This method must be overridden in each derived class.
		*/
		virtual const char* GetClassName() const = 0;

		/*!
		Check if this class or one of its ancestors has the given class name.
		This method must be overridden in each derived class.
		*/
		virtual bool IsA(const std::string& classType) const = 0;

		/*!
		Check if this instance has the given class name.
		*/
		bool IsExactlyOfType(const std::string& classType) const { return classType == GetClassName(); }
	};

}

