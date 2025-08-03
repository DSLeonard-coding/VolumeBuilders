## Building the library 
Copy the vb_user_setup_general.sh script to your home directory and edit the geant and root paths.  

Then just do 
```bash
> mkdir build
> cd build
> cmake ..
> make
```
to build the library and   
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
build options and you should log into your X windows with X.Org, not Wayland (usually an option on a settings wheel at the login.)

The library will be in **build/libVolumeBuilders.a**  and the demo will be in **build/bin/Demo**

The project is also precofigured for building and developing with JetBrains Clion (free version available).   Your vb_user_setup_general.sh must be in your home directory and must have exactly the same file name as shipped in the project root (if it has changed).  Start clion and select File->Open and selec the repository root directory.

Now you can build, run, and debug, from the top right toolbar build window. Select VolumeBuilders or Demo from the drop-down and the hammer and play button to build and run or the bug to debug.

In clione Demo is setup to take demo/debug/input.txt as input to  
```
Demo gui
```
By default it this is setup to run the assembly demo.  

## Building the library for your project

Again, to use the library in another project you will need some environment variables setup.  Projects using this should include the provided script or their own expanded version of it.  Otherwise, copy and edit the included one from VolumeBuilders as before.

Assume you install the library repository into your project in subtrees/VolumeBuilders.
(I use a subtree workflow for that and specifically git-alltrees -- that I wrote.  But you can install it anywhwere, anyhow, and change the path below)  

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
That's it. Build and run your project as usual for your project.

