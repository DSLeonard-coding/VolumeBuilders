//HpgeSim geometry include file.  Template by D. Leonard, 2021
//This file is included into parts of the HpgeSim class files
//it is not in a standard header format

#ifdef INCLUDE_SIM_GLOBALS
//First declare the class functions, whithout class qualifier
        void ConstructAssembly();

//Then declare other class globals defined in the .cc
//   possibly including materials, but only if defined in the .cc

#endif

#ifdef INCLUDE_SIM_GEOMETRIES
//Here, define runtime geometries selections, see existing files for examples.
//Multiple configurations associated with this file can be defined.
    Geometries["Assembly"] =[](object *c) -> void {
// Geometries have dependancies and call each other if needed
//  Implicit dependancies are only set where absolutely required anyway.
        c->DetGeometryType="demo";
        c->ConstructMaterials();
        c->ConstructWorld();
        c->ConstructAssembly();
    };

#endif
