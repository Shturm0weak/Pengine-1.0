#include "Core/EntryPoint.h"

#include "Examples/2DShooter/Source/2DShooter.h"
#include "Examples/TestRTTRSystem.h"

using namespace Pengine;

int main()
{
	EntryPoint entrypoint;
	TestRTTRSystem application;
	entrypoint.SetApplication(&application);

	return 0;
}