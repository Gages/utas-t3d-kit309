#pragma once
#include "WinGLApplication.h"

namespace T3D {
	class Assignment1 :
		public WinGLApplication
	{
	public:
		float farclip = 500.0f;
		typedef WinGLApplication super;

		bool init();

		Assignment1();
		~Assignment1();
	};
}

