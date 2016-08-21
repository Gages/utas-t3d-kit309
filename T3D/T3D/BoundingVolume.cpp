#include "BoundingVolume.h"
#include "Math.h"
/*
Both the Spherical and AABB implementations are defined in this file.
*/


namespace T3D {

/****************** AAAB SPHERE SECTION ***************************/

static BoundingAABB FAST_AABB_FROM_AABB(const Matrix4x4& m, const BoundingAABB& vol) {
	//assume m has uniform scaling
	//assume m is affine

	//this algorithm was inspired by
	//http://clb.demon.fi/MathGeoLib/nightly/docs/AABB.cpp_code.html#471
	// See Christer Ericson's Real-time Collision Detection, p. 87, or
	// James Arvo's "Transforming Axis-aligned Bounding Boxes" in Graphics Gems 1, pp. 548-550.
	// http://www.graphicsgems.org/
	//another example implementation
	//bullet3: https://github.com/bulletphysics/bullet3/blob/master/src/Bullet3Geometry/b3AabbUtil.h#L182

	const Vector3 center = (vol.min + vol.max) * 0.5f;
	const Vector3 extents = center - vol.min;
	Vector3 newCenter = m * center;
	Vector3 newExtents = Vector3(
		abs(m[0][0] * extents.x) + abs(m[0][1] * extents.y) + abs(m[0][2] * extents.z),
		abs(m[1][0] * extents.x) + abs(m[1][1] * extents.y) + abs(m[1][2] * extents.z),
		abs(m[2][0] * extents.x) + abs(m[2][1] * extents.y) + abs(m[2][2] * extents.z)
		);

	return BoundingAABB(
		newCenter - newExtents,
		newCenter + newExtents
		);
}

static BoundingAABB BASIC_AABB_FROM_AABB(const Matrix4x4& m, const BoundingAABB& vol) {
	//A simplified implementation (for debugging purposes).
	//does not exploit symmetry properties of the bounding volume.
	//transform each corner and return the extrema.

	Vector3 A = vol.min, B = vol.max;
	Vector3 corners[] = {
		//A, B,
		Vector3(A.x, A.y, B.z),
		Vector3(A.x, B.y, A.z),
		Vector3(B.x, A.y, A.z),
		Vector3(B.x, B.y, A.z),
		Vector3(B.x, A.y, B.z),
		Vector3(A.x, B.y, B.z)
	};

	for (Vector3& p : corners) {
		p = m * p;
	}

	Vector3 min = corners[0], max = corners[0];
	for (Vector3& p : corners) {
		min = Math::minvec(p, min);
		max = Math::maxvec(p, max);
	}

	return BoundingAABB(min, max);
}

BoundingAABB operator*(const Matrix4x4& m, const BoundingAABB& vol)
{
	return FAST_AABB_FROM_AABB(m, vol);
}


BoundingAABB BoundingAABB::grow_to_contain(const BoundingAABB& a, const BoundingAABB& b)
{
	//if either A or B have no area,
	//return the other AABB.
	if (!a.has_volume()) return b;
	if (!b.has_volume()) return a;

	return BoundingAABB(
		Math::minvec(a.min, b.min),
		Math::maxvec(a.max, b.max));
}

inline static void getVerticesPN(const Vector3& normal, const Vector3& min, const Vector3& max, Vector3& P, Vector3& N) {
	P = min, N = max;
	if (normal.x >= 0) {
		P.x = max.x;
		N.x = min.x;
	}
	if (normal.y >= 0) {
		P.y = max.y;
		N.y = min.y;
	}
	if (normal.z >= 0) {
		P.z = max.z;
		N.z = min.z;
	}
}

BoundingVolumeIntersects BoundingAABB::intersects(const std::array<Plane, 6>& frustum) const
{
	//This implementation is derived from this page
	//http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
	//from the section Geometric Approach - Testing Boxes II
	//however the frustum normals have opposite signs in this
	//implementation, which results in the code below.

	Vector3 P, N;
	BoundingVolumeIntersects result = Inside;

	//if the bounding volume has no area, then the volume can not be "inside" the view frustum.
	if (!has_volume()) return Outside;

	for (auto plane : frustum) {
		getVerticesPN(plane.normal, min, max, P, N);
		if (plane.getDistance(N) > 0)
			return Outside;
		else if (plane.getDistance(P) > 0) 
			result = Overlap;
	}
	
	return result;
}


/************* BOUNDING SPHERE SECTION ***************************/

BoundingSphere BoundingSphere::grow_to_contain(const BoundingSphere& a, const BoundingSphere& b)
{
	//if either A or B have no area,
	//return the other Sphere.
	if (!a.has_volume()) return b;
	if (!b.has_volume()) return a;

	Vector3 p1 = a.center, p2 = b.center;
	double   r1 = a.radius, r2 = b.radius;

	//ensure that r1 is the bigger sphere
	if (r1 < r2) {
		std::swap(r1, r2);
		std::swap(p1, p2);
	}
	Vector3 vdiff = p2 - p1;
	//two cases
	//case 1: one sphere is completely inside the other.
	//consider two spheres (a,b)
	//let a be the sphere with the bigger radius.
	//if b is completely inside a, the distance from center(a) to center(b) + radius(b)
	//must be less than radius(a)
	if (vdiff.squaredLength() < (r1 - r2) * (r1 - r2)) {
		return BoundingSphere(p1, r1);
	}
	else {
		//case 2: spheres are not completely nested
		//based on the formulas from
		//http://graphicsandgames.com/tiki-index.php?page=KIT309+Bounding+Volumes

		Vector3 v = vdiff.normalised();
		return BoundingSphere(
			((p1 - r1 * v) + (p2 + r2 * v)) / 2,
			(r1 + r2 + vdiff.length()) / 2
			);
	}
}

//frustum should be in the same space as the BoundingVolume
BoundingVolumeIntersects BoundingSphere::intersects(const std::array<Plane, 6>& frustum) const {

	//if the bounding volume has no area, then the volume can not be "inside" the view frustum.
	if (!has_volume()) return Outside;

	BoundingVolumeIntersects result = Inside;
	for (auto plane : frustum) {
		float distance = plane.getDistance(center);
		if (abs(distance) < radius) //if the sphere is close to the plain, it overlaps
			result = Overlap;
		else if (distance > radius) //if the sphere is on the positive side, and further than the radius, it is outside.
			return Outside;
	}
	return result;
}


}