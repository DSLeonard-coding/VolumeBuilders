## The Distribution

DLG4::VolumeBuilders is included now with DLG4::GeoModules (a modular geometry plugin package) and VBDemo that is built on both of them.  The runtime selection of VB examples is provided by GeoModules.  Building just the VolumeBuilders library is discussed below but the demo itself is the best framework for building a simulation based on it.  You don't need to use the GeoModules features, but they're available.  Building the demo is discussed more in [GeoModulesAndVBDemoREADME.md]([GeoModulesAndVBDemoREADME.md])


## Building the library 
This is tested on geant-10.7.4-C++17  (sorry, this could use updating) and root 6.30.04, built with C++17.
Some minor changes to the code may be necessary for geant4-11.  It's on the todo list but you can try.

Copy the vb_user_setup_general.sh script to your home directory and edit the geant and root paths.  These should point to the top of the install directory for geant, build directory for root, as in the example file.


```bash
> mkdir build
> cd build
> cmake ..
> make
```
But to do anything interesting with it you need a simulation configured to use it.  The demo
is the best template for that.  Building the demo, geometry modules from clion templates, and projects is covered in [GeoModulesAndVBDemoREADME.md](GeoModulesAndVBDemoREADME.md)


## Building and running the Demo

to build the demo, you can run the above commands from the VolumeBuilders directory OR from the demo directory, as both contain a build config that builds it.  In the demo directory you'll get VBDemo by default.  From VolumeBuilders you'll need to do 

```
make VBDemo
```
either way run it after with:

```
./bin/VBDemo
```

To run the demo, it's best if your geant has OGL and QT support through:  
```
   -DGEANT4_USE_QT=ON \
   -DGEANT4_USE_OPENGL_X11=ON
```

The library will be in **build/libVolumeBuilders.a**  and the demo will be in **build/bin/Demo**

## Development, building in CLion, and your own demo

The project is also precofigured for building and developing with JetBrains Clion (free version available).  See [GeoModulesREADME.md](#create-a-module-fast-and-easy-using-the-clion-templates) 

Now you can build, run, and debug, from the top right toolbar build window. Select VolumeBuilders or Demo from the drop-down and the hammer and play button to build and run or debug to debug.


## Building the library for your project

This is covered in [GeoModulesREADME.md](GeoModulesAndVBDemoREADME.md#builing-a-new-project).  Simply put, copy the demo directory, and change a couple of paths in the CMakeLists.txt.

You may need to merge the CMakeLists.txt with the one for your project, but the provided one handles a lot.  The VolumeBuildes and GeoModules cmake files handle a lot of the details for you.

That's it. Build and run your project as usual.  You might take a look at the scripts here in cmake/  used by CMakeLists.txt as they are meant to ease Geant and ROOT builds in general, as well to leverage traditional env setup scripts, but they may not be tested on recent versions yet.


\* I even tried a version setup for vcpkg auto-installation but I found build speeds were much slower, for reasons I couldn't yet figure out.