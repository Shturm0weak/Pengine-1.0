# **Pengine**
A simple 2D game engine.
<img src="https://user-images.githubusercontent.com/48869588/157312824-eb6a3cb1-3e2a-4b01-bfdb-d26537dbb858.png">
<img src="https://user-images.githubusercontent.com/48869588/178843475-57a372ee-4f1c-418d-acb3-0a67bec0c039.png" width="503" height="503">
<img src="https://user-images.githubusercontent.com/48869588/178844698-39590fd3-71b1-4d43-b429-13a92e57e40a.png" width="503" height="503">
<img src="https://user-images.githubusercontent.com/48869588/178845117-d035b5e4-fd97-4726-847e-e2a7e71bdde2.png" width="334" height="205">
<img src="https://user-images.githubusercontent.com/48869588/178845155-a12fe03e-0388-4be1-9e1e-51879d3a7ea4.png" width="334" height="205">
<img src="https://user-images.githubusercontent.com/48869588/178845208-2e01fa6a-3563-4972-b489-851bcd856c73.png" width="334" height="205">

# **Features**

* Batch Renderering
* ECS(Entity component system)
* 2D Physics
* Thread Pool
* Audio
* RTTR(Run time type reflection) for user-defined components
* Simple in game GUI (text, button, slider, bar, list box, panel) 
* Event System
* OpenGL 4.6 support
* Scripts in Lua for game objects

# **Used Libraries**

* GLFW
* GLEW
* ImGui
* Box2D
* RTTR
* Lua
* OpenAL

# **Instalation**

* Download zip archive and unzip it.
* Run premake5.exe via console cmd.exe as premake5.exe vs20(your visual studio version).
* Open PengineEngine.sln, set SandBox as StartUp project.
* Build it twice, first time there will be an error due to non-existent folders.
* If it didn't run, you need to install OpenAl, run OpenAlInst.exe in Solution folder.
* Also you may need to install Visual C++ Redistributable.
