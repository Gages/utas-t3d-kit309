#pragma once


#include "Matrix4x4.h"
#include "Vector3.h"
#include <array>	

//Comment the below line to use BoundingSphere instead of AABB for HBVC.
#define USE_BOUNDING_VOLUME_AABB

namespace T3D {
	class Plane;
	struct BoundingSphere;
	struct BoundingAABB;


	enum BoundingVolumeIntersects {
		Inside, Outside, Overlap
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
		inline bool has_volume() const { return radius != 0; }
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

		friend inline BoundingSphere operator*(const Matrix4x4& m, const BoundingSphere& vol) {
			return BoundingSphere(m * vol.center, vol.radius);
		}

		/*
		inline BoundingSphere transform_by(const Matrix4x4& mat) const {
			return BoundingSphere(mat * center, radius);
		}*/

		//frustum should be in the same space as the BoundingVolume
		BoundingVolumeIntersects intersects(const std::array<Plane, 6>& frustum) const;
	};

	struct BoundingAABB {
		BoundingAABB() : min(0, 0, 0), max(0, 0, 0) {}
		BoundingAABB(Vector3 min, Vector3 max) : min(min), max(max) {}
		Vector3 min;
		Vector3 max;

		static BoundingAABB grow_to_contain(const BoundingAABB& a, const BoundingAABB& b);
		inline bool has_volume() const { return min != max; }

		static BoundingAABB createFromSphere(Vector3 center, float radius) {
			const auto dir = Vector3(1, 1, 1) * radius;
			return BoundingAABB(center - dir, center + dir);
		}

		static BoundingAABB createFromAABB(Vector3 topleft, Vector3 bottomright) {
			return BoundingAABB(topleft, bottomright);
		}

		//inline BoundingAABB transform_by(const Matrix4x4& mat) const;
		friend BoundingAABB operator*(const Matrix4x4& m, const BoundingAABB& vol);

		//frustum should be in the same space as the BoundingVolume
		BoundingVolumeIntersects intersects(const std::array<Plane,6>& frustum) const;
	};
}