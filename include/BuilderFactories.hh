#include "VolumeBuilderTypes.hh"
//
// Code by Douglas S. Leonard on 6/8/25. File creation (refactor) 3/2/2026 All rights Reserved
// See related files for license, if any is provided.
//
// Builder Factories
//
// Note Presently only factories that need to be in separate headers to break header loops are here.
// Others are with their related classes. They may all move here some day.

#ifndef VOLUMEBUILDERS_BUILDERFACTORIES_HH
#define VOLUMEBUILDERS_BUILDERFACTORIES_HH

namespace DLG4::VolumeBuilders {
    //    class StructureBuilder;

    /**
     * @brief Assembly of strucures, ie builders and/or other assemblies
     * \dotfile builder_graph.dot
     * @ingroup Factories
     * */
    AssemblyPtr CreateAssembly(G4String name);

    /**
     * @brief Constructor to make a builder from an Existing Geant solid
     * @param solid G4VSolid pointer
     * @return
     * @ingroup Factories
     * */
    FromG4VSolidPtr CreateFromG4VSolid(G4VSolid *solid);

}
#endif  //VOLUMEBUILDERS_BUILDERFACTORIES_HH
