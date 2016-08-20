#pragma once

/*
	Header file for "bounding volume" class.
	This header should be reusable for both the BoundingSphere and AABB implementations
	Only one implementation should be compiled into the project.
	This is not an abstract base class because we do not
	want run time polymorphism; all bounding volumes should be the same type.
*/


#include "Matrix4x4.h"
#include "Vector3.h"
//controls whether to use AABB or spherical BoundingVolume
#define USE_BOUNDING_VOLUME_AABB

namespace T3D {
	class Plane;
	class BoundingSphere;
	class BoundingAABB;


	enum BoundingVolumeIntersects {
		Positive, Negative, Overlap, Undefined //undefined for the identity value
	};

#ifdef USE_BOUNDING_VOLUME_AABB
	typedef BoundingAABB DefaultBoundingVolume;
#else
	typedef BoundingSphere DefaultBoundingVolume;
#endif

	struct BoundingSphere {
		BoundingSphere(Vector3 center, float radius) : center(center), radius(radius) {}
		Vector3 center;
		float radius;
	
		static BoundingSphere grow_to_contain(const BoundingSphere& a, const BoundingSphere& b);
		inline bool has_area() const { return radius != 0; }
		BoundingSphere() : center(0, 0, 0), radius(0) {} //The identity value (for grow_to_contain) is the default constructor

		static BoundingSphere createFromSphere(Vector3 center, float radius) {
			return BoundingSphere(center, radius);
		}
		
		static BoundingSphere createFromAABB(Vector3 topleft, Vector3 bottomright) {
			//the center is the average of the two corner vectors
			//the radius is the distance from the center to either corner.
			Vector3 center = (topleft + bottomright) / 2;
			return BoundingSphere(center, topleft.distance(center));
		}

		inline BoundingSphere transform_by(const Matrix4x4& mat) const {
			return BoundingSphere(mat * center, radius);
		}

		BoundingVolumeIntersects intersects(Plane p) const;
	};

	struct BoundingAABB {
		BoundingAABB(Vector3 topleft, Vector3 bottomright) : topleft(topleft), bottomright(bottomright) {}
		Vector3 topleft;
		Vector3 bottomright;

		static BoundingAABB grow_to_contain(const BoundingAABB& a, const BoundingAABB& b);
		inline bool has_area() const { return topleft != bottomright; }
		BoundingAABB() : topleft(0, 0, 0), bottomright(0, 0, 0) {}

		static BoundingAABB createFromSphere(Vector3 center, float radius) {
			const auto dir = Vector3(1, 1, 1) * radius;
			return BoundingAABB(center - dir, center + dir);
		}

		static BoundingAABB createFromAABB(Vector3 topleft, Vector3 bottomright) {
			return BoundingAABB(topleft, bottomright);
		}

		inline BoundingAABB transform_by(const Matrix4x4& mat) const;

		BoundingVolumeIntersects intersects(Plane p) const;
	};

	template <class BoundingVolume>
	inline BoundingVolume operator * (const Matrix4x4& mat, const BoundingVolume& vol) {
		return vol.transform_by(mat);
	}
}