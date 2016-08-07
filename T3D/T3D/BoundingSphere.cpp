#include "BoundingSphere.h"
#include "AxisAlignedBoundingBox.h"
#include "Mesh.h"
namespace T3D{
	BoundingSphere::BoundingSphere(Vector3 pos, float radius)
		: _radiusSqr(radius * radius), _position(pos)
	{ }

	BoundingSphere::BoundingSphere() 
		: _radiusSqr(0), _position(Vector3(0,0,0))
	{}

	BoundingSphere BoundingSphere::Identity() { return BoundingSphere(); }

	/*BoundingSphere BoundingSphere::createFromMesh(Mesh const* mesh) {

		//Degenerate case: mesh has no vertices.
		//return the identity bounding sphere
		if (mesh->getNumVerts() == 0) {
			return BoundingSphere::Identity();
		}

		Vector3 center;
		float squaredRadius = 0;
		{
			//pass 1: find the center using the AABB method.
			const int numVerts = mesh->getNumVerts();
			AxisAlignedBoundingBox box = AxisAlignedBoundingBox(mesh->getVertex(0));
			
			for (int i = 1; i < numVerts; i++) {
				box = box.growToContain(mesh->getVertex(i));
			}
			center = box.center();

			//pass 2: find the vertex with the greatest distance from the center.
			for (int i = 0; i < numVerts; i++) {
				squaredRadius = std::max(mesh->getVertex(i).squaredDistance(center), squaredRadius);
			}
		}

		return BoundingSphere(center, sqrt(squaredRadius));
	}*/

	inline bool BoundingSphere::isIdentity() const {
		return _radiusSqr == 0;
	}

	bool BoundingSphere::contains(Vector3 point) const {
		//the identity value does not contain any points.
		//a radius of zero marks the identity value.
		
		//the below version might be more forgiving (includes points lying along the radius,
		//but still excludes all when the radius is zero
		//return !isIdentity() && _position.squaredDistance(point) <= _radiusSqr;

		//however this version works (and still excludes the origin point when the radius is zero)
		return _position.squaredDistance(point) < _radiusSqr;
	}

	BoundingSphere::IntersectTest BoundingSphere::intersects(Plane p) const {
		if (isIdentity()) return IntersectTest::Undefined;
		else {
			float distance = p.getDistance(_position);
			if (distance * distance < _radiusSqr) return Overlap;
			else {
				return distance > 0 ? Positive : Negative;
			}
		}
	}

	BoundingSphere BoundingSphere::growToContain(BoundingSphere other) const {
		//if this is the identity value, return other.
		if (isIdentity()) return other;

		//if other is the identity value, return this.
		if (other.isIdentity()) return *this;

		Vector3 p1 = _position, p2 = other._position;
		double   r1 = sqrt(_radiusSqr), r2 = sqrt(other._radiusSqr);
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
		} else {
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
}