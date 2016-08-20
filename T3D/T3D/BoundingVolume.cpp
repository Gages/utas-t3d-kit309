#include "BoundingVolume.h"
#include "Math.h"
/*
Both the Spherical and AABB implementations are defined in this file.
*/


namespace T3D {

/****************** AAAB SPHERE SECTION ***************************/

BoundingAABB BoundingAABB::transform_by(const Matrix4x4& m)  const
{
	//assume m has uniform scaling
	//assume m is affine

	//this algorithm was inspired by
	//http://clb.demon.fi/MathGeoLib/nightly/docs/AABB.cpp_code.html#471
	// See Christer Ericson's Real-time Collision Detection, p. 87, or
	// James Arvo's "Transforming Axis-aligned Bounding Boxes" in Graphics Gems 1, pp. 548-550.
	// http://www.graphicsgems.org/
	//another example implementation
	//bullet3: https://github.com/bulletphysics/bullet3/blob/master/src/Bullet3Geometry/b3AabbUtil.h#L182

	const Vector3 center = (topleft + bottomright) * 0.5f;
	const Vector3 extents = center - topleft;
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


template <float(*M)(float, float)>
static Vector3 merge(Vector3 left, Vector3 right) {
	return Vector3(
		M(left.x, right.x),
		M(left.y, right.y),
		M(left.z, right.z));
}

BoundingAABB BoundingAABB::grow_to_contain(const BoundingAABB& a, const BoundingAABB& b)
{
	return BoundingAABB(
		merge<std::fminf>(a.topleft, b.topleft),
		merge<std::fminf>(a.bottomright, b.bottomright));
}

BoundingVolumeIntersects BoundingAABB::intersects(Plane plane) const
{
	//This implementation is derived from this page
	//http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
	//from the section Geometric Approach - Testing Boxes II

	Vector3 p = topleft, n = bottomright;
	if (plane.normal.x >= 0) {
		p.x = bottomright.x;
		n.x = topleft.x;
	}
	if (plane.normal.y >= 0) {
		p.y = bottomright.y;
		n.y = topleft.y;
	}
	if (plane.normal.z >= 0) {
		p.z = bottomright.z;
		n.z = topleft.z;
	}



}


/************* BOUNDING SPHERE SECTION ***************************/



}