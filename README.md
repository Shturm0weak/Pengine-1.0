# **Pengine**
A simple 2D game engine.

<img src="https://user-images.githubusercontent.com/48869588/199320150-c399b799-ac89-421d-82c7-663dafc887fc.png">
<img src="https://user-images.githubusercontent.com/48869588/199319824-219f2f5f-06e0-419d-9ba4-f971ad58aafd.png">
<img src="https://user-images.githubusercontent.com/48869588/185961976-8b7d092c-48ac-452a-8b48-54591e173825.png">
<img src="https://user-images.githubusercontent.com/48869588/178849904-b219c9fa-2252-42b0-a201-62ced3646083.png">
<img src="https://user-images.githubusercontent.com/48869588/178851129-857d1916-f737-4a6c-9386-cf22d78816b5.png">
<img src="https://user-images.githubusercontent.com/48869588/178850673-68461c01-7c4f-4715-a887-1538fe7a0052.jpg">

# **Features**

* Simple Components System
* 2D/3D Physics
* Thread Pool
* Audio
* RTTR(Run Time Type Reflection) for user-defined components
* Simple ingame GUI (text, button, slider, bar, list box, panel, image, animation) 
* Event System
* OpenGL 3.3 support
* Lua Scripts for GameObjects (partially available, currently deprecated)
* Directional Lights
* Point Lights
* Spot Lights
* 2D/3D Normal Maps
* 2D Emissive Mask
* Bloom
* Cascade Shadow Maps
* Point Light Shadows
* SSAO
* Deferred Instanced Rendering
* Forward Order Dependent Transparency Rendering
* 2D Batch Renderering

# **Used Libraries**

* GLFW
* GLEW
* ImGui
* Box2D
* RTTR
* Lua
* OpenAL
* BulletPhysics

# **Instalation**

* Download zip archive and unzip it.
* Run premake5.exe via console cmd.exe as premake5.exe vs20(your visual studio version).
* Open PengineEngine.sln, set SandBox as StartUp project.
* Build it twice, first time there will be an error due to non-existent folders.
* If it didn't run, you need to install OpenAl, run OpenAlInst.exe in Solution folder.
* Also you may need to install Visual C++ Redistributable.
