#pragma once
#include "Vector3.h"
namespace T3D{
	class AxisAlignedBoundingBox
	{
	public:
		AxisAlignedBoundingBox(Vector3 topleft, Vector3 bottomright);
		AxisAlignedBoundingBox(Vector3 point) : AxisAlignedBoundingBox(point, point) {}
		//~AxisAlignedBoundingBox();

		Vector3 center() const;
		AxisAlignedBoundingBox growToContain(Vector3 point) const;

	private:
		Vector3 topleft;
		Vector3 bottomright;
	};
}

