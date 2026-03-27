#pragma once
#ifndef VolumeReference_HPP
#define VolumeReference_HPP
//
// Created by Douglas S. Leonard on 6/8/25.  All rights Reserved
// See related files for license, if any is provided.
//
// A type erasing class to hold all kinds of SolidBuilders and VolumeBuilders
// Use is indirect,  by passing to a VolumeReferencePtr parameter or by emplace_back to vector<VolumeReferencePtr().
// If explicit use is needed  VolumeReferencePtr(builder_object), object.VolumeReferencePtr() work.
//
//
//
//#include "VolumeBuilderReference.hh"
#include "Linkable.hh"
#include "i_shared_ptr.hh"
#include <stdexcept>
#include <VolumeBuildersTypes.hh>
#include "VolumeBuilder.hh"
#include "ISolidBuilder.hh"
#include <BuilderFactories.hh>

namespace DLG4::VolumeBuilders::_internals_ {
    namespace VB=DLG4::VolumeBuilders;
    using namespace VB;

    template <typename T>
    class VolumeBuilder;
    class RZBuilder;

    /**
     * @brief A polymorphic, type-erased builder referencing any specialized builder.
     * @details Use it to assign a specialized builder to a generic builder.
     * But you don't use it directly.  It has no direct public ctors or factories,
     * but BuilderView(your_other_builder_object) constructs it as
     *  i_shared_ptr<VolumeBuilderReference> ( pointer-wrapped builder).
     * Or pass your builder to something expecting a VolumeBuilderReferencePtr,
     * like ex: a std::vector<VolumeBuilderReferencePtr>, aka a VolumeBuilderRefList.
     *
     * @headerfile VolumeBuilderReference.hh
     * @see VolumeBuilder for inherited methods.
     */
    class VolumeBuilderReference final: public VolumeBuilder<VolumeBuilderReference> {
        template <typename T>
        friend class VolumeBuilder;
        friend AssemblyPtr VB::CreateAssembly(G4String name);
        template <typename T>
        friend class StructureBuilder;

        template <typename T>
        friend class i_shared_ptr;

    public:
        G4VSolid *SolidConstructor(const G4String &name) override;

    private:
        //allow i_shared_ptr access to our converting ctors
        //This way they get instantly wrapped!
        friend class i_shared_ptr<VolumeBuilderReference>;

        /**
         * Main constructor to link to another builder
         * This is a super cheat.  By linking CRTP base data, we don't need forwarders
         * For each and every CRTP method here!!  Just for the (much fewer) derived class overrides!
         * We get FULL CRTP polymorphic tye erasure at a tiny fraction of the price!
         * @param other The builder to reference
         */
        template <typename T>
        VolumeBuilderReference(i_shared_ptr<T> other, // NOLINT(*-explicit-constructor)
            std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>, int>  = 0) : VolumeBuilder<
            VolumeBuilderReference>(other, SET_LINK) {
        }

        /* Constructor to make a builder from an Existing Geant solid
         * This is mostly meant for use in parameter signatures
         * Where you need to get a free solid or Builder with a solid
         * @param G4VSolid pointer
         * @return The builder
        */
        VolumeBuilderReference(G4VSolid *solid);

        /* Constructor to make a builder from an Existing Geant logical_volume
         * This is mostly meant for use in parameter signatures
         * Where you need to get a logical volume or a Builder with a solid
         * @param G4VSolid pointer
         * @return The builder */
        VolumeBuilderReference(G4LogicalVolume *volume);

        /* Constructor to make a builder from an Existing Geant logical_volume.
         * This is mostly meant for use in parameter signatures
         * where you need to get a logical volume or a Builder with a solid
         * @param G4VPhysicalVolume pointer
         * @return The builder */
        VolumeBuilderReference(G4VPhysicalVolume *volume);

        ///////////////  The rest is all boilerplate: ////////////////////////


        VolumeBuilderReference(const VolumeBuilderReference &other) = default;

        VolumeBuilderReference() = default;

    protected:
        SharedPtr<IStructureBuilder> clone_impl() const override;

        BuilderView ToBuilderView() const override {
            // we're already type erased, so just return ourselves.
            return shared_mutable_this(this);
        }

    public:
        VolumeBuilderReference &operator=(const VolumeBuilderReference &other) = delete;
    };
}
#endif  //VolumeReference_HPP
