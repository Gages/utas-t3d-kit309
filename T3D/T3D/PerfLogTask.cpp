// =========================================================================================
// KXG363 - Advanced Games Programming, 2012
// =========================================================================================
//
// Author: Robert Ollington
//
// perflogtask.cpp
//
// Simple class to track framerates and write to file when app closes

#include <sstream>
#include <fstream>
#include <iomanip>
#include "perflogtask.h"
#include "GLRenderer.h"
#include "BoundingVolume.h"

namespace T3D{

	PerfLogTask::PerfLogTask(T3DApplication *app) : Task(app)
	{		
		setName("PerfLogTask");

		reset();

		diagDisplay = false;
		diagOverlay = NULL;
	}


	PerfLogTask::~PerfLogTask(void)
	{
		log();
	}

	void PerfLogTask::reset()		// reset elasped time, frame count, min/max frames times
	{
		frameCount = 0;
		elapsedTime = 0;

		minFrameRate = 1000;		// start big, it will quickly get smaller
		maxFrameRate = 0;

		sampleElapsed = 0;
		sampleFrames = 0;

		sampleCount = 0;
		frameRateTotal = 0;
	}

	void PerfLogTask::log(){		
		std::ofstream logfile;
		logfile.open ("perflog.txt", fstream::in | fstream::out | fstream::app);
		logfile.precision(1);
		logfile << std::fixed;
		logfile << "frames: " << frameCount << "\n";
		logfile << "elapsed time: " << elapsedTime << "\n";
		logfile.precision(1);
		logfile << "frame rate (min/avg/max): " << minFrameRate << " / " << frameCount/elapsedTime << " / " << maxFrameRate << "\n";
		logfile.close();
	}

	//The number of polygons in the scene.
	//A polygon is a triangle or a quad on a mesh object
	//To calculate this, we need to visit every mesh
	//which can be found by visiting every game object
	//which can be found by visiting every transform.
	unsigned int count_polys(Transform* root) {
		unsigned int count = 0;
		Mesh* m;
		if (root->gameObject && (m = root->gameObject->getMesh())) {
			count += m->getNumTris() + m->getNumQuads();
		}

		for (auto child : root->children) {
			count += count_polys(child);
		}
		return count;
	}

	void PerfLogTask::update(float dt){
		frameCount++;
		elapsedTime += dt;
		sampleFrames++;
		sampleElapsed += dt;

		if (sampleElapsed > PERF_SAMPLING_PERIOD)			// update every quarter second
		{
			double currentFrameRate = sampleFrames/sampleElapsed;
			double averageFrameRate = frameCount/elapsedTime;

			
			unsigned int polygons_in_scene = count_polys(app->getRoot());
			unsigned int polys_recently_rendered = ((GLRenderer*)app->getRenderer())->polys_last_frame;
		
			if (elapsedTime > 3 * PERF_SAMPLING_PERIOD)		// allow some settling time
			{
				// Alternate way of calculating average frame rate
				// This is mainly used to allow a settling time by avoiding frameCount and elapsedTime 
				sampleCount++;
				frameRateTotal += currentFrameRate;
				avgFrameRate = frameRateTotal / sampleCount;

				if (currentFrameRate > maxFrameRate)
					maxFrameRate = currentFrameRate;
				if (currentFrameRate < minFrameRate)
					minFrameRate = currentFrameRate;
			}

			if (diagDisplay)
			{
				font *f = app->getFont("resources/FreeSans.ttf", 16);
				if (f != NULL)
				{
					std::ostringstream ss;
					ss.precision(1);
					ss << std::fixed;
					ss << "frames: " << frameCount;
				//	ss << ", elapsed time: " << elapsedTime;
				//	ss << ", frame rate: min=" << minFrameRate << ", avg=" << averageFrameRate << ", max=" << maxFrameRate << ", cur=" << currentFrameRate << " (avg=" << avgFrameRate << ")";
					ss << ", frame rate: " << "cur= " << currentFrameRate << ", avg = " << avgFrameRate;
					ss << ", polys: in_scene=" << polygons_in_scene << ", in_frame=" << polys_recently_rendered;

					if (std::is_same<DefaultBoundingVolume, BoundingAABB>()) {
						ss << ", (using BoundingAABB)";
					}
					else {
						ss << ", (using BoundingSphere)";
					}

					ss << ", HBVC depth: " << app->getRenderer()->HBVC_MaxDepth;

					int w = 1024;		// texture width, should be large enough for most diagnostics
					int h = 32;			// should be enough for single line (text wrap is not supported)
					if (diagOverlay == NULL)
						diagOverlay = new Texture(w,h);
					diagOverlay->clear(Colour(0,0,0,255));
					diagOverlay->writeText(0, 0, ss.str().c_str(), Colour(255,255,255,255), f->getFont());
					app->getRenderer()->loadTexture(diagOverlay, false);

					if (!app->getRenderer()->exists2DOverlay(diagOverlay))
						app->getRenderer()->add2DOverlay(diagOverlay, 0, 0);
				}
			}
			else if (diagOverlay != NULL)
			{
				app->getRenderer()->remove2DOverlay(diagOverlay);
				app->getRenderer()->unloadTexture(diagOverlay);
				delete diagOverlay;
				diagOverlay = NULL;
			}

			sampleElapsed = 0;
			sampleFrames = 0;
		}
	}


}