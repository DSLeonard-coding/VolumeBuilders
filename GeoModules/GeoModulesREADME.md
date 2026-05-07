# DLG4::GeoModules
> Zero-scaffolding Compile-time pluggable geometry modules, with run-time selectable recipes for Geant4.

Draft document in progress

## Overview
GeoModules is designed for frequently modified detector geometries, especially for HPGe where samples and supporters may change frequently.

It is moving toward zero scaffolding, no customization of DetectorConstruction.hh/cc is usually needed.
(a few things are still being moved out)

## Modules

The system uses pluggable .cc and .inc (essentially headers) to add materials, modular geometry parts, 
and "context" variables that connect them. 

### GeometryModules

There is no difference between modules.  They are just code, but most are meant to define a geometry element.   They simply define a constructor MyModuleGeoModule::MyModuleGeoModule()


and are available to all modules via the context object.
```
context->myvar
```


Provides access to myvar.  This allows global interface variables for aligning modules.

### Materials and Elements
#### Local materials
Materials can be defined in any module, such as MaterialsGeoModule.cc/inc, and are now accessed in other modules via their Geant global material names through VBHelpers::GetMaterial("some_name")
#### NIST materials
Nist materials are also retrieved the same way using the NIST material name.  Often this can


Every .cc gets a .inc that declares its module name.  
Every .inc can optionally declare a recipe that can be selected at runtime.
Recipes are just a small bit of code, but generally just call module constructoin to construct 
