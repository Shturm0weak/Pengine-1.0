#include "Core/EntryPoint.h"

#include "Examples/Animation3D.h"
#include "Examples/Rotator.h"

using namespace Pengine;

int main()
{
	EntryPoint entrypoint;
	Application application;

#ifdef STANDALONE
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	entrypoint.SetApplication(&application, true);
#else
	entrypoint.SetApplication(&application, false);
#endif
	
	return 0;
}