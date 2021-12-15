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
	Vector in 3D space.
	@note Structure fields are ordered like in a right-handed coordinate system (Z up).
    In a right-handed XYZ/Z-up coordinate system: Right = X, Forward = Y, Up = Z.
	*/
	struct Vector3D
	{
        /*!
		Position along the right axis.
		*/
		float Right = 0;

        /*!
		Position along the forward axis.
		*/
		float Forward = 0;

        /*!
		Position along the up axis.
		*/
		float Up = 0;

		/// @name Constructors
		///@{

		Vector3D()
		{
		}

		Vector3D(float rt, float fw, float up)
		{
			Right = rt;
			Forward = fw;
			Up = up;
		}

		///@}

	};


	inline bool operator == (const Vector3D& vec1, const Vector3D& vec2)
	{
		return vec1.Right == vec2.Right && vec1.Forward == vec2.Forward && vec1.Up == vec2.Up;
	}

	inline Vector3D& operator += (Vector3D& vec, const Vector3D& inc)
	{
		vec.Right += inc.Right;
		vec.Up += inc.Up;
		vec.Forward += inc.Forward;
		return vec;
	}

	inline Vector3D operator + (const Vector3D& vec1, const Vector3D& vec2)
	{
		Vector3D vec = vec1;
		vec += vec2;
		return vec;
	}

	inline Vector3D& operator -= (Vector3D& vec, const Vector3D& inc)
	{
		vec.Right -= inc.Right;
		vec.Up -= inc.Up;
		vec.Forward -= inc.Forward;
		return vec;
	}

	inline Vector3D operator - (const Vector3D& vec1, const Vector3D& vec2)
	{
		Vector3D vec = vec1;
		vec -= vec2;
		return vec;
	}

	///@}

	/*!
	Location (position, rotation) of an object.
	*/
	struct Location3D
	{
		//! Position along each axis.
		Vector3D Position;

		//! Rotation in degrees along Up, Right, Forward axes.
		Vector3D Rotation;
	};


	/// @name Overridden operators for Location3D
	///@{

	inline Location3D& operator += (Location3D& loc, const Location3D& inc)
	{
		loc.Position += inc.Position;
		loc.Rotation += inc.Rotation;
		return loc;
	}


	inline Location3D operator + (const Location3D& loc1, const Location3D& loc2)
	{
		Location3D loc = loc1;
		loc += loc2;
		return loc;
	}

	///@}

	/*!
	Coordinate system in 3D space.
	*/
	struct CoordSys3D
	{
		/*!
		Vector of the right axis for this coordinate system.
		*/
		Vector3D RightAxis = { 1,0,0 };

		/*!
		Vector of the forward axis for this coordinate system.
		*/
		Vector3D ForwardAxis = { 0,1,0 };

		/*!
		Vector of the up axis for this coordinate system.
		*/
		Vector3D UpAxis = { 0,0,1 };

		/*!
		Origin of this coordinate system.
		*/
		Vector3D Origin;
	};


	/*!
	Transformation in 3D space.
	*/
	struct Transform3D
	{
		/*!
		Use coordinate system instead of Location and Euler angles.
		*/
		bool UseCoordSys = false;

		/*!
		Coordinate system (Z-up, right-handed).
		*/
		CoordSys3D CoordSys;

		/*!
		Position relative to parent transform.
		*/
		Vector3D Location;

		/*!
		Euler angles (relative to parent transform).
		*/
		Vector3D EulerAngles;

		/*!
		Scaling (1 = no rescaling).
		*/
		Vector3D Scale = { 1,1,1 };

		/// @name Constructors
		///@{

		Transform3D() {}

		Transform3D(const Vector3D& pos)
			: Location(pos) {}

		Transform3D(const Vector3D& pos, const Vector3D& rot)
			: Location(pos), EulerAngles(rot) {}

		Transform3D(const Vector3D& pos, const Vector3D& rot, const Vector3D& scale)
			: Location(pos), EulerAngles(rot), Scale(scale) {}

		Transform3D(const CoordSys3D& coordSys)
			: UseCoordSys(true), CoordSys(coordSys) {}

		///@}

	};


	//! Local transformation with a reference identifier to a parent element.
	struct LocalTransform : public Transform3D
	{
		//! Identifier of the entity to which this transform is relative.
		std::string ParentId;

		/// @name Constructors.
		///@{

		LocalTransform() {}

		LocalTransform(const Vector3D& pos, const std::string& parentId = "")
			: Transform3D(pos), ParentId(parentId) {}

		LocalTransform(const Vector3D& pos, const Vector3D& rot, const std::string& parentId = "")
			: Transform3D(pos, rot), ParentId(parentId) {}

		LocalTransform(const Vector3D& pos, const Vector3D& rot, const Vector3D& scale, const std::string& parentId = "")
			: Transform3D(pos, rot, scale), ParentId(parentId) {}

		LocalTransform(const CoordSys3D& coordSys, const std::string& parentId = "")
			: Transform3D(coordSys), ParentId(parentId) {}

		///@}

	};

}

