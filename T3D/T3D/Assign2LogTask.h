#pragma once
#include "Task.h"
namespace T3D {
	class Assign2LogTask :
		public Task
	{
	public:
		Assign2LogTask(T3DApplication *app);
		~Assign2LogTask();

		virtual void update(float dt);

		void reset();
		void log();

		float period = 5.0f;
		uint32_t period_count = 0;

		uint32_t frames_count = 0;
		uint32_t hbvc_tests = 0;
		float elapsed_time = 0;
	};

}