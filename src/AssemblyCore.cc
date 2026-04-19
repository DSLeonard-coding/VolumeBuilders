/**
* @file
* Created by @author Douglas S. Leonard on @date 6/29/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

#include "VolumeBuildersTypes.hh"
#include "VolumeBuilder.hh"
#include "BuilderViewCore.hh"
#include "StructureBuilder.hpp"
#include "AssemblyCore.hh"

using namespace DLG4::VolumeBuilders::_internals_;
namespace DLG4::VolumeBuilders {
    Assembly CreateAssembly(G4String name) {
        // Haha... assemblies actually are builders!!!
        auto object = Assembly(new AssemblyCore());
        // ... with is_builder set to false.
        object->placement_configs_->is_builder = false;
        //We're storing a pointer to the view in the builder by writing to the builder through that same view!
        BuilderView builder_view = object->ToBuilderView();
        builder_view->StoreBuilderView(builder_view);
        // Then store itself in its new builder (that links to its data).
        //object->builder_configs_->builder_view->StoreIStructurePtr(IStructurePtr(object.get()));    // this is the owning copy.
        // ... and viewed through a reduced StructureView interface:
        object->SetName(name);
        auto return_obj = Assembly(object);
        return return_obj;

        // This let's us reuse builder methods!
    }
}

namespace DLG4::VolumeBuilders::_internals_ {
    AssemblyCore::AssemblyCore(const AssemblyCore &other) :
        StructureBuilder<AssemblyCore>(other) {
        // Call base class copy constructor
        set_shared_from_this_enabled(false);
        set_shared_from_this_enabled(true);
    }

    Assembly AssemblyCore::AddStructure(const StructureView &other) {
        this->placement_configs_->children.emplace_back(other);
        return this->shared_from_this();
    }
}

//
