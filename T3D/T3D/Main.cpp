// =========================================================================================
// KIT309 - 3D Games Programming, 2016
// =========================================================================================
//
// Author: Phillip Warren
//
// main.cpp
//
// Assignment 1:
// An implementation of HBVC with both Bounding Sphere and Axis Aligned Bounding Box.
// The code supports either, however the choice is static (can not be changed or mixed at runtime)

// ******* IMPORTANT *******
// See the file BoundingVolume.h to control which is used. AABB is on by default.
// ******* IMPORTANT *******

// Also see related code in the following files
// Critera	Relevant Files
// 1a, 2a   Tutorial4.cpp, GLRenderer.cpp, PerfLogTask.cpp
//			NOTE	The same scene is used both for the extension and the first part of the tutorial as
//					they are both HBVC implementations. The textual information displayed on screen
//					is controlled by the latter two files.
//
// 1b		BoundingVolume.h, BoundingVolume.cpp, Mesh.cpp, Sphere.cpp, Cube.cpp
//			NOTE	The last three contain the specialised methods to calculate a BoundingVolume from
//					a mesh, sphere and cube.
//
// 2b		BoundingVolume.(h|cpp), Renderer.(h|cpp), Transform.(h|cpp), Camera.(h|cpp)
//			NOTE	Camera defines the view frustum. Renderer and Transform work together to implement
//					HBVC using the methods in BoundingVolume
// =========================================================================================

#include "Assignment1.h"

using namespace T3D;

int main(int argc, char* argv[]){

	//Note: There is only one Application here as the AABB extension is switched on (by default)
	//with a compile-time mechanism. See BoundingVolume.h to switch it off.

	//The below application sets up a basic scene with some moving spheres, cubes and
	//a cog shape (which was written by me for KIT307)

	//Run the application and press F2 to turn on axis drawing,
	//F10 to turn on informational display.
	
	//The number of polys in the scene is constant,
	//while the number of polys being drawn is counted at a low level by GLRenderer (which doesn't know about HBVC)
	//This number will change based on the camera orientation if HBVC is working.
	//Also there is a number "HBVC Depth" which shows how many layers into the the scenegraph the renderer had to go to finish
	//culling the scene.

	//Known bug: the position of objects sometimes "jumps" once when the application starts.
	//I don't know what causes this at the moment, but it doesn't seem to affect the HBVC.

	auto *theApp = new Assignment1();

	theApp->farclip = 500.0f; //Default farclip distance.
	//theApp->farclip = 50.0f; //Alternative close farclip distance. Uncomment if required.

	theApp->run();
	delete theApp;

	return 0;
}