#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include <limits>

namespace T3D {
	class BoundingSphere
	{
	public:
		//BindingSphere is a Monoid.

		//The identity value for the BoundingSphere monoid.
		//forall a : BindingSphere, a.growToContain(BindingSphere::Identity) == a.
		//forall a : BindingSphere, BindingSphere::Identity.growToContain(a) == a.
		static BoundingSphere Identity(); //labelled constructor
		//test: BoundingSphere::Identity().isIdentity() = true.

		//The commutative binary operator
		//forall a b : BindingSphere, a.growToContain(b) == b.growToContain(a).
		BoundingSphere growToContain(BoundingSphere other) const;

		//Warning!! if radius is zero, this sphere will actually contains no points! (i.e it will be equivalent to the identity value)
		//to create a small sphere which contains its center, use createfromPoint.
		//to explicitly create an empty bounding sphere (that contains no points), use Identity().
		BoundingSphere(Vector3 position, float radius);
		//~BoundingSphere();

		static BoundingSphere createFromMesh(Mesh const*);
		//What if we want to create a point-sphere that is really small, but not the identity value (which contains no points)?
		static BoundingSphere createFromPoint(Vector3 position) { return BoundingSphere(position, std::numeric_limits<float>::epsilon()); }
		//test: forall p : Vector3, BoundingSphere::createFromPoint(p).contains(p) == true.
		//test: forall p : Vector3, BoundingSphere::Identity().contains(p) == false.
		//test: forall p : Vector3, BoundingSphere::createFromPoint(p).isIdentity() == false.
		
		//forall a : Vector3, b c : BoundingSphere, b.contains(a) || c.contains(a) -> b.growToContain(c).contains(a).
		bool           contains(Vector3) const;
		bool		   isIdentity() const; //returns true when the bounding sphere is the identity value.

		Vector3 getPosition() const { return _position; }
		float   getRadius() const { return sqrt(_radiusSqr); }

		bool operator==(const BoundingSphere& rhs) {
			return (rhs.isIdentity() && isIdentity()) || (rhs._position == _position && rhs._radiusSqr == _radiusSqr);
		}

	private:

		BoundingSphere(); //unlabelled identity constructor

		Vector3 _position;

		//We use a radius of zero to mark the identity value, rather than keeping a seperate boolean flag, to save space.
		//This means that, conceptually, the BoundingSphere with radius zero
		//does not even contain the point which lies exactly over the _position.
		//I actually think this is neat; the radius is zero, not infinitesimally small.
		//This interpretation is motivated by looking at the BoundingSphere as a monoid.
		float _radiusSqr; 
		//const bool is_identity; 
	};

	//ignore the problem of scaling the radius. Only the position of the sphere is transformed.
	inline BoundingSphere operator * (const Matrix4x4& mat, const BoundingSphere& sphere)
	{
		return BoundingSphere(mat * sphere.getPosition(), sphere.getRadius());
	}
}