// =========================================================================================
// KXG363 - Advanced Games Programming, 2012
// =========================================================================================
//
// Author: Robert Ollington
//
// main.cpp
//
// Main entry point. Creates and runs a T3DApplication

//#include "T3DTest.h"
//#include "Tutorial1_Baseline.h"
#include "Tutorial4.h"
//#include "ShaderTest.h"
//#include "GLTestApplication.h"

using namespace T3D;

int main(int argc, char* argv[]){
	//T3DApplication *theApp = new T3DTest();
//	T3DApplication *theApp = new Tutorial1_Baseline();
	//T3DApplication *theApp = new Tutorial2();
	T3DApplication *theApp = new Tutorial4();
	//T3DApplication *theApp = new GLTestApplication();
	//T3DApplication *theApp = new ShaderTest();

	theApp->run();
	delete theApp;

	return 0;
}