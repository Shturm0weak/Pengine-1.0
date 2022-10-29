#include "Core/EntryPoint.h"

#include "Sponza/Sponza.h"
#include "Examples/BulletPhysics.h"

using namespace Pengine;

int main()
{
	EntryPoint entrypoint;
	BulletPhysics application;

#ifdef STANDALONE
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	entrypoint.SetApplication(&application, true);
#else
	entrypoint.SetApplication(&application, false);
#endif
	
	return 0;
}