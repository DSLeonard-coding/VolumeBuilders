# DLG4::GeoModules
> Zero-scaffolding Compile-time pluggable geometry modules, with run-time selectable recipes for Geant4.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Overview
GeoModules is designed for frequently modified detector geometries, especially for HPGe where samples and supporters may change frequently.

It now uses zero app-side scaffolding, no customization of DetectorConstruction.hh/cc is usually needed.
You only need to use the provided one for most uses, and add content in your own pluggable modules. 

Geometries are modular with runtime recipe selection.   Materials are selected by user defined name or NIST name.

It is now packaged with [DLG4::VolumeBuilders](README.md) and uses features from it.

## Modules

The system uses compile-time pluggable, runtime selectable .cc and .inc (essentially headers) to add materials, modular geometry parts, and "context" variables that connect them.   

### Recipes
At present individual modules are not runtime-selectable, in part because not all define a world volume.  Instead pre-defined recipes are runtime selectable.  Most samples will define a short recipe that simply lists the materials module, detector module, and supporter modules it needs.  Run-time definable recipes could be added if there's a motivation.

### GeometryModules

There is no difference between module types.  They are just code, but most are meant to define a geometry element.   They simply define a constructor MyModuleGeoModule::MyModuleGeoModule()

and are available to all modules via the context object.
```cpp
context->myvar
```
provides access to myvar.  This allows global interface variables for aligning modules.

### Materials and Elements
#### Local materials
Materials can be defined in any module, such as MaterialsGeoModule.cc/inc, and are now accessed in other modules via their Geant global material names through VBHelpers::GetMaterial("some_name")
#### NIST materials
Nist materials are also retrieved the same way using the NIST material name.  Often this can

Every .cc gets a .inc that declares its module name.  
Every .inc can optionally declare a recipe that can be selected at runtime.
Recipes are just a small bit of code, but generally just call module constructoin to construct 


The interface has been completely refactored and, for the user, simplified.

You can define:

- matierals in src/core/MaterialsGeoModules.cc
- Shared geometry variables in src/core/GeoModulesContextVariablesCore.cc
- Colors in src/core/GeoModulesContextVariablesColor.cc

In the geometry modules (the .cc files)  there is a "context" variable passed.
The geometries have access to the shared variable names and world pointer through that.

context->your_shared_variable
context->Colors.white  
context->SetWorldVolume(logical_volume_ptr)
context->GetWorldVolume()

You can also call other modules through
context->ConstructGeoModule("SomeModule")

Methods are made available via the VolumeBuilders libraries,
particularly through the VBHelpers alias:

VBHelpers::GetMaterial("copper")
VBHelpers::GetElement("Pb")
VBHelpers::CopyMaterial("old","new",density,unit)


GetMaterial now operates by name and accepts NIST material names.

Methods are made available via the VolumeBuilders libraries,
particularly through the VBHelpers alias:

### Builing a new project 

There are a few details to the format of the module file, particularly how they include their own .inc files.

While the library is very modular now, still the best way to use it is just to copy the demo folder to your project and change the VolumeBuilders and GeoModules directories in the CMakeLists.txt to point back to the respective sources.

The provided demo has no physics or tracking included, just geometry, but demo/src/core/TheSimulation.cc (and .hh)  is ready to add them and comes with examples commented out for CupSim usage.

### Create a module fast and easy using the clion templates
You can copy and .cc and .inc in the demo/src/Geometries directories and just rename things. I'll go over that below.  But easiest is to just open the demo/CMakeLists.txt as a project in Clion (it's already configured for Clion, which has free versions)  and go to 
```
File->New->VBDemo Geometry
```
You will get a box asking for three inputs which you can fill out like this example:

1) MODULE_BASE_NAME     :     TestBox
2) GEORECIPE_NAME       :     TestBox
3) MATERIAL             :     G4_CONCRETE

The material can be one you defined in the materials file or a NIST one like here.  No quotes.

Click ok and you're done.  You can modify the shape to your desire.
This just creates code for a simple one-material starter shape, but you can customize from there.  That part is your job.  The recipe loads materials, a world geometry and your shape.

Choose VBDemo command_line from the build panel on the upper right and click play to compile and run it. (Be sure to read the BuildInstructions first to setup your environment variables.)

Alternatively, select VBDemo gui < demo/input.txt
This takes input from the mentioned file.
By default it this is setup to run the assembly demo.


### Building the world

We'll get to details of modules, but at least one must call
```
context->SetWorldVolume(some_G4VPhysicalVolume_pointer)
```
The default build as a WorldGeoModule included so you just have to modify it and include it in your recipes.

Other modules position parts relative to the shared variables you define in the file described above.
So you can assign values to a vector in your world module and position things based on that in your sample module.

### Recipes and Module Code Example and Details

Here's an example X-header and recipe produced by the present version of the Clion template:
```cpp
//BoxTest GeoModule
//file and template author D.S. Leonard, refactored 2025
//This is an X-header, included in parts

//*********EDIT TO DEFINE A UNIQUE MODULE NAME HERE*****************
//***The class name, for use in the .cc, will be MODULE_BASEGeoModule*****
#define MODULE_BASE BoxTest

#include "GeoModules/GeoModule_Inline1.inl"
#ifdef INCLUDE_GEOMODULE_RECIPES
/////////////Edit the Recipe name and the recipe  ////////////////////////
void CLASSNAME::RegisterRecipes(GeoModulesParserPtr parser) {
    parser->AddRecipe("BoxTest",
        [](GeoModulesContextPtr context) -> void {
            context->ConstructGeoModule("Materials");
            context->ConstructGeoModule("World");
            context->ConstructGeoModule("BoxTest");
    });
}

#endif
#include "GeoModules/GeoModule_Inline2.inl"
```
**These examples can and will change, so best to check the latest demo code.**

The MODULE_BASE defines the base name of the class for your module, which as written because BoxTestGeoModules.  **A recipe** is defined for "BoxTest" (you can name it differently and even add multiple recipes).  It selects a Materials module, the "World" module and the "BoxExample" module that will be provided in the corresponding cc file.  

And here is the cc code for the same:
```
// pre-declared clion template variables.  You can delete these in the generated files.
// BoxTest
// BoxTest
// G4_CONCRETE

// ReSharper disable CppExpressionWithoutSideEffects
//BoxExample GeoModule
#include <GeoModules/GeoModules.hh>
#define INCLUDE_GEOMODULE_RECIPES
#include "BoxTestGeoModule.inc"                                //MUST MATCH MODULE NAME
#undef INCLUDE_GEOMODULE_RECIPES

#include "DetectorConstruction_includes.hh" // common includes
#include <VolumeBuilders.hh>


namespace VB = DLG4::VolumeBuilders; // Geometry builder helpers.

void BoxTestGeoModule::Construct(GeoModulesContextPtr context) {      //MUST MATCH MODULE NAME
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    G4cout << "now building BoxTestGeoModule()  ()\n" << G4endl;

    //make a dedicated sample_material for vertex generation.  Can adjust density with optional 3rd parameter
    G4Material *sample_mat = VBHelpers::CopyMaterial("G4_CONCRETE", "sample_mat");
    VB::SetGlobalDefaultUnit(VB::Length::mm); // set a global unit

    auto box_part = VB::CreateZDeltaBoxBuilder(
                    "BoxTest_box", // name
                    10, // x total size
                    10, // y total size
                    -5, //  z start
                    10) //  z change
            ->SetColor(1, 0, 0) // red
            ->SetPhysOffset(context->sample_center)   // use a context variable to position it!!
            ->SetMother(context->GetWorldVolume())
            ->SetMaterial(sample_mat)              // you could also provide a string name here!
            ->ForceSolid(true)
            //->SetPhysOffset({mm, 0, 0, 0})      // No offset needed often for ofset solid
            ->MakePlacement();
}
```

The preamble, up to the Construct() signature, has a mostly required format.  You can add more includes or such of course.  But BoxTest include and Construct call must be defined correctly.  The Clion template does it though.   

This example uses VolumeBuilders to define the geometry.   It has many advantages, but even if you define it traditionally, VB provides helpers to get the material names etc. 

## Building

Copy the vb_user_setup_general.sh script to your home directory and edit the geant and root paths.  These should point to the top of the install directory for geant, build directory for root, as in the example file.

You do NOT need to source this before running an IDE or such.  The CMakeLists.txt siphons variables from it.  That's (one reason) why it _must_ be in the right place.

From there you can use the Clion instructions above for development.  For building from CLI, for cluster use, etc... the instructions are essentially the same as [for DLG4::VolumeBuilders](HowToBuild.md)  and in fact you can build the demo from either the top VolumeBuilders directory/config or from the demo directory and build config.

From either your VolumeBuilders install or VolumeBuilders/demo do:

```bash
> mkdir build
> cd build
> cmake ..
> make
```
To run the demo, it's best if your geant has OGL and QT support through:
```
   -DGEANT4_USE_QT=ON \
   -DGEANT4_USE_OPENGL_X11=ON
```

The library will be in **build/libVolumeBuilders.a**  and the demo will be in **build/bin/Demo**
