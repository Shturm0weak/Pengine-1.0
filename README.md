# **Pengine**
A simple 2D/3D game engine.

https://www.youtube.com/watch?v=vcoNo1o5r8s

<img src="https://user-images.githubusercontent.com/48869588/199320150-c399b799-ac89-421d-82c7-663dafc887fc.png">
<img src="https://user-images.githubusercontent.com/48869588/199319824-219f2f5f-06e0-419d-9ba4-f971ad58aafd.png">
<img src="https://user-images.githubusercontent.com/48869588/227362595-bbbab415-c5c0-4746-beb9-9f475c6466cd.png">
<img src="https://user-images.githubusercontent.com/48869588/216528363-bf508043-6cd3-40e2-bca3-bf1ab18edaf8.png">
<img src="https://user-images.githubusercontent.com/48869588/216690655-83fc98ee-bcd3-4424-97ac-579672773e04.png">
<img src="https://user-images.githubusercontent.com/48869588/216691633-b4b5bb73-6f70-4a79-9cdd-2a306e33eae4.png">
<img src="https://user-images.githubusercontent.com/48869588/185961976-8b7d092c-48ac-452a-8b48-54591e173825.png">
<img src="https://user-images.githubusercontent.com/48869588/178851129-857d1916-f737-4a6c-9386-cf22d78816b5.png">
<img src="https://user-images.githubusercontent.com/48869588/178850673-68461c01-7c4f-4715-a887-1538fe7a0052.jpg">
<img src="https://user-images.githubusercontent.com/48869588/224151173-8b7a9008-14f2-49e7-b148-4d57797174f7.mp4">
<img src="https://user-images.githubusercontent.com/48869588/224153932-243ba01b-8fbc-446f-8afd-7c86c84835ea.mp4">

# **Features**

* Simple Components System
* Box2D Physics
* Bullet Physics
* Thread Pool
* Audio
* RTTR(Run Time Type Reflection) for user-defined components
* Simple ingame GUI (text, button, slider, bar, list box, panel, image, animation) 
* Event System
* OpenGL 4.6 support
* Lua Scripts for GameObjects (partially available, currently deprecated)
* Directional Lights
* Point Lights
* Spot Lights
* 2D/3D Normal Maps
* Bloom
* Cascade Shadow Maps
* Point Light Shadows
* Spot Light Shadows
* SSAO
* Deferred Instanced Rendering
* Forward Order Dependent Transparency Rendering
* 2D Batch Renderering
* Scriptable Materials

# **Used Libraries**

* GLFW
* GLEW
* ImGui
* Box2D
* Lua
* OpenAL
* BulletPhysics
* Assimp

# **Instalation**

* Download zip archive and unzip it.
* Go to Vendor/Assimp and build this project with cmake, run it and build it
* Go to the top folder and run premake5.exe via console cmd.exe as premake5.exe vs20(your visual studio version).
* Open PengineEngine.sln, set SandBox as StartUp project.
* Build it twice, first time there will be an error due to non-existent folders.
* If it didn't run, you need to install OpenAl, run OpenAL.exe in Vendor/OpenAL.
* Also you may need to install Visual C++ Redistributable.
