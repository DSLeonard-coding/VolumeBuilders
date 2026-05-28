#pragma once
/**
* @file
* Created by @author Douglas S. Leonard on @date 6/22/25.  All rights Reserved
* See related files for license, if any is provided.
*
* These are the non-fluent polymorphic virtual functions for the derived classes of VolumeBuilder
* Generally they need a fluent wrapper in the VolumeBuilder class if they are to be fluent.
* A non-templated base class like this one cannot use the templated fluent return type.
*
* These methods generally also need polymorphic forwarding in the type-erased VolumeBuilderCore.
* through the stored ISolidBuilder* reference_builder_interface_ pointer to the original class.
* Maybe that should really be declared down here.
*
* The main idea is to limit the number of these.
* @brief
*
*/
#ifndef ISOLIDBUIDLER_HH
#define ISOLIDBUIDLER_HH

#include <G4LogicalVolume.hh>
#include <G4Transform3D.hh>
#include <G4VSolid.hh>

#include "Linkable.hh"
#include "VolumeBuildersTypes.hh"


namespace DLG4::VolumeBuilders::_internals_ {
    /// a non-fluent polymorphic class to access the derived solid-builder from a base reference:
    /// This gets used by VolumeBuiderReference to store pointers to derived methods.

    /// a non-fluent polymorphic class to access the derived solid-builder from a base reference:
    /// This gets used by VolumeBuiderReference to store pointers to derived methods.
    class IStructureBuilder {
    protected:
        friend class VolumeBuilderCore;    // for tye erasing polymorphism
        friend class StructureBuilderCore; // for tye erasing polymorphism
        template <typename T>
        friend class StructureBuilderBase;
        friend class AssemblyCore;
        virtual G4VSolid *SolidConstructor(const G4String &name) = 0;
        virtual ~IStructureBuilder() = default;
        // clones and returns a base class pointer
        [[nodiscard]] virtual SharedPtr<IStructureBuilder> clone_impl() const = 0;
        // Returns a type-erased view ptr.
        [[nodiscard]] virtual StructureBuilder ToStructureView() const = 0;
        [[nodiscard]] virtual VolumeBuilder ToVolumeBuilder() const = 0;
        // store type erased views of underlying objects...
        // combined with above we can get a view from an object and store the view
        // in the tye-erased object being copied or constructed.
    };

    //Just a tag class to mark IStructureBuilders that are also ISolidBuilders
    class ISolidBuilder: public IStructureBuilder {
    };
}


//


#endif //ISOLIDBUIDLER_HH
