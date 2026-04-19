/**
* @file
* Created by @author Douglas S. Leonard on @date 6/29/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

#include "VolumeBuildersTypes.hh"
#include "ISolidBuilder.hh"
#include "VolumeBuilderBase.hh"
#include "VolumeBuilderCore.hh"
#include "StructureBuilderCore.hh"
#include "StructureBuilderBase.hh"
#include "StructureBuilderBase.hpp"


namespace DLG4::VolumeBuilders::_internals_ {
    SharedPtr<IStructureBuilder> StructureBuilderCore::clone_impl() const {
        if (builder_configs_->istructure_ptr) {
            // have the erased base clone itself ex: new RZBuilder(*this);  :
            IStructurePtr copy = builder_configs_->istructure_ptr->clone_impl();
            // Then have it create a Builder from itself:
            // These are non owning views, won't keep the temp copy:
            VolumeBuilder builder_view = copy->ToVolumeBuilder();
            StructureBuilder structure_view = copy->ToStructureView();
            // Then store itself in its new builder (that links to its data).
            builder_view->StoreIStructurePtr(copy); // this is the owning copy.
            //We're storing the view in the builder by writing to the builder through that same view!
            builder_view->StoreBuilderView(builder_view);
            return structure_view; // and return the builder
        } else {
            // not sure we should get here:
            auto x = static_cast<SharedPtr<IStructureBuilder>>(shared_mutable_this(this));
            return x;
        }
    }

    StructureBuilder StructureBuilderCore::ToStructureView() const {
        // we're already type erased, so just return ourselves.
        auto x = shared_mutable_this(this);
        return x;
    }
}

//
