#include "AxisAlignedBoundingBox.h"
#include <algorithm>
namespace T3D {
	AxisAlignedBoundingBox::AxisAlignedBoundingBox(Vector3 topleft, Vector3 bottomright)
		: topleft(topleft), bottomright(bottomright)
	{
	}

	Vector3 AxisAlignedBoundingBox::center() const {
		return (topleft + bottomright) / 2;
	}

	AxisAlignedBoundingBox AxisAlignedBoundingBox::growToContain(Vector3 point) const {
		using namespace std;
		return AxisAlignedBoundingBox(
			//top left is min of each dimension
			Vector3(
				min(topleft.x, point.x),
				min(topleft.y, point.y),
				min(topleft.z, point.z)),
			//bottom right is max of each dimension
			Vector3(
				max(bottomright.x, point.x),
				max(bottomright.y, point.y),
				max(bottomright.z, point.z))
			);
	}
}