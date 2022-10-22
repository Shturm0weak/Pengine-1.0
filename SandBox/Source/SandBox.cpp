#include "Core/EntryPoint.h"

#include "Sponza/Sponza.h"

using namespace Pengine;

int main()
{
	EntryPoint entrypoint;
	Sponza application;

#ifdef STANDALONE
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	entrypoint.SetApplication(&application, true);
#else
	entrypoint.SetApplication(&application, false);
#endif
	
	return 0;
}