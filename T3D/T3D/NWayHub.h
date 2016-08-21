#pragma once

#include "MeshExtra.h"
namespace T3D {
	class NWayHub :
		public MeshExtra
	{
	public:
		NWayHub(int density, int numspokes, float hubRadius, float filletRadius, float depth);
		virtual ~NWayHub();

		static void SetupNWayHub(MeshExtra* mesh, int density, int numspokes, float hubRadius, float filletRadius, float depth);
	};
}
