#include "Environment.h"

using namespace Pengine;

Environment& Environment::GetInstance()
{
	static Environment environment;
	return environment;
}
