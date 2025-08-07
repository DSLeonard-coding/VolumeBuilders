//VBDEMO geometry include file.  Template by D. Leonard, 2025
//This file is included into parts of the VBDemo class files
//It is not in a standard header format.

#ifdef INCLUDE_SIM_GLOBALS
//First declare the class functions, whithout class qualifier
        void Construct${METHOD_NAME}();

//Then declare other class globals defined in the .cc
//   possibly including materials, but only if defined in the .cc

#endif

#ifdef INCLUDE_SIM_GEOMETRIES
//Here, define runtime geometries selections, see existing files for examples.
//Multiple configurations associated with this file can be defined.
    Geometries["${GEOMETRY_NAME}"] =[](object *c) -> void {
        c->DetGeometryType="demo";
        c->ConstructMaterials();
        c->ConstructWorld();
        c->Construct${METHOD_NAME}();
    };

#endif
