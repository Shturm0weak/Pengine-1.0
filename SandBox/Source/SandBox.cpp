#include "Core/EntryPoint.h"
#include "Core/Input.h"
#include "Core/Environment.h"
#include "Core/Visualizer.h"
#include "Core/Viewport.h"
#include "Components/Renderer2D.h"
#include "EventSystem/EventSystem.h"
#include "Components/Rigidbody2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Animator2D.h"
#include "UI/Gui.h"
#include "Core/Window.h"
#include "Core/Viewport.h"

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