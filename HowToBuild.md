## Building the library 
This is tested on geant-10.7.4-C++17  (sorry, this could use updating) and root 6.30.04, built with C++17.
Some minor changes to the code may be necessary for geant4-11.  It's on the todo list but you can try.

Copy the vb_user_setup_general.sh script to your home directory and edit the geant and root paths.  These should point to the top of the install directory for geant, build directory for root, as in the example file.


Then just do 
```bash
> mkdir build
> cd build
> cmake ..
> make
```
to build the library and then do...

## Building and running the Demo
```
make VBDemo
```
to build the demo.  Then run it with
```
./bin/VBDemo
```

To run the demo, it's best if your geant has OGL and QT support through:  
```
   -DGEANT4_USE_QT=ON \
   -DGEANT4_USE_OPENGL_X11=ON
```

The library will be in **build/libVolumeBuilders.a**  and the demo will be in **build/bin/Demo**

## Development and your own demo

The project is also precofigured for building and developing with JetBrains Clion (free version available).   Your vb_user_setup_general.sh must be in your home directory and must have exactly the same file name as shipped in the project root (if it has changed).  Start Clion and select File->Open and selec the repository root directory.

Now you can build, run, and debug, from the top right toolbar build window. Select VolumeBuilders or Demo from the drop-down and the hammer and play button to build and run or the bug to debug.

In clione Demo is setup to take demo/debug/input.txt as input to  
```
Demo gui
```
By default it this is setup to run the assembly demo.  

### Adding your own demo geometry

- Navigate to demo/src/Geometries  
- Then go to File-New-VBDemo Geometry
- Enter a method name and GEO_NAME key to select your geometry with at startup
- Enter a material variable name... you can use an existing one in demo/src/Materials.inc, like _copper
  or use or own and define the material yourself inside the generated .cc file.
- Press OK, or whatever it is.  

You now have a .cc and a kind of pluggable header  (using a now fairly outdated version of DLG4::GeoModules).  You may need to refresh the CMake in Tools->CMake.  You may also (or even first) need to run build-autoincludes.sh from /demo or run it through the build configuration toolbar (top-right). Then rebuild the VBDemo commandline target and click play.  You should see your still uncustomized geometry.  

## Building the library for your project

Again, to use the library in another project you will need some environment variables script.    Projects using this should include the provided script or their own expanded version of it.  Otherwise, copy and edit the included one from VolumeBuilders as before.  You don't need to run it or source it.  The cmake scripts included do that.  My one biggest annoyance with Clion is that it lacks good pre-build hooks.

Assume you install the library repository into your project in subtrees/VolumeBuilders.
(I use a subtree workflow for that and specifically git-alltrees -- that I wrote.  But you can install it anywhwere, anyhow, and change the path below\*)  

Then In CMakeLists.txt in your project:
```
set(VOLUMEBUILDERS_NO_DEMO ON CACHE BOOL "Disable demo executable build" FORCE)
#Note you can still build DEMO as an explicit target even with the above
add_subdirectory(../subtrees/VolumeBuilders ../subtrees/VolumeBuilders/build)

target_include_directories(yourEXETarget PRIVATE

        ...
        ${base_path}/../subtrees/VolumeBuilders/include
...
target_link_libraries(yourEXETarget PRIVATE
        VolumeBuilders
...
```
That's it. Build and run your project as usual.  You might take a look at the scripts here in cmake/  used by CMakeLists.txt as they are meant to ease Geant and ROOT builds in general, as well to leverage traditional env setup scripts, but they may not be tested on recent versions.


\* I even tried a version setup for vcpkg auto-installation but I found build speeds were much slower, for reasons I couldn't yet figure out.