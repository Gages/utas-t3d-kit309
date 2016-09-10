#include "Assign2LogTask.h"
//#include <sstream>
#include <fstream>
//#include <iomanip>

using namespace T3D;

Assign2LogTask::Assign2LogTask(T3DApplication *app) : Task(app)
{
	setName("Assign2LogTask");
	reset();
	period_count = 0;
}


Assign2LogTask::~Assign2LogTask()
{
}

void Assign2LogTask::update(float dt)
{
	frames_count++;
	elapsed_time += dt;
	hbvc_tests += app->getRenderer()->HBVC_CULLTESTS;

	if (elapsed_time > period) {
		period_count++;
		log();
		reset();
	}
}

void Assign2LogTask::reset()		// reset elasped time, frame count
{
	frames_count = 0;
	elapsed_time = 0;
	hbvc_tests = 0;
}

void Assign2LogTask::log(){
	std::ofstream logfile;
	logfile.open("assign2log.txt", fstream::in | fstream::out | fstream::app);
	logfile.precision(5);
	logfile << std::fixed;
	logfile << period_count << "\t";
	logfile << elapsed_time << "\t";
	logfile << frames_count << "\t";
	logfile << hbvc_tests << std::endl;
	logfile.close();
}