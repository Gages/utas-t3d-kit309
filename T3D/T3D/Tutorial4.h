#pragma once
#include "WinGLApplication.h"

namespace T3D {
	class Tutorial4 :
		public WinGLApplication
	{
	public:

		typedef WinGLApplication super;

		bool init();

		Tutorial4();
		~Tutorial4();
	};
}

