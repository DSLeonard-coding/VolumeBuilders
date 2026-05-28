#pragma once
#ifndef STRUCTUREBUILDER_HPP
#define STRUCTUREBUILDER_HPP
//
// Created by Douglas S. Leonard on 6/8/25.  All rights Reserved
// See related files for license, if any is provided.
//
// Assemblies for treating a collection of builders as a single unit.
//#include "StructureBuilder.hh"
#include "Linkable.hh"
#include "i_shared_ptr.hh"
#include <stdexcept>
#include <StructureBuilderBase.hh>
#include <VolumeBuildersTypes.hh>


namespace DLG4::VolumeBuilders::_internals_ {
    template <typename T>
    class VolumeBuilderBase;
    //    class StructureBuilder;


    /**
     *@class StructureBuilderCore
     * @brief A type-erased (data shared view) view of a builder or
     * assembly, ie a "structure."
 * \dotfile builder_graph.dot
     * @details Can hold other structures, creating placement hierarchies.
     * Behaves like a buidler, but only for logical volume properties
     * and placement related commands. You can set VisAtt on a structure,
     * and can set offsets and rotations of the whole structure for placement.
     *
     * While this base view only supports limited methods, it is fully polymorphic
     * and builder objects internally can trigger their full build chain.
     *
     * This derived clas really exists for parallelism with VolumeBuilderCore.
     *
     * @headerfile StructureBuilderBase.hh
     * @see VolumeBuilder for inherited methods.
     * */
    class StructureBuilderCore final: public StructureBuilderBase<StructureBuilderCore> {
        template <typename T>
        friend class VolumeBuilderBase;
        template <typename T>
        friend class StructureBuilderBase;

        template <typename T>
        friend class i_shared_ptr;

    private:
        friend class i_shared_ptr<StructureBuilderCore>;

        template <typename T>
        StructureBuilderCore(i_shared_ptr<T> other, // NOLINT(*-explicit-constructor)
            std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>,
                int>  = 0) : StructureBuilderBase<StructureBuilderCore>(other, SET_LINK) {
        }

        G4VSolid *SolidConstructor(const G4String &name) override {
            throw std::runtime_error(
                "Error in StructureBuilderCore::SolidConstructor(const G4String &name) "
                + this->builder_configs_->name + " \n" +
                "SolidConstructor(const G4String &name) is not implemented.");
        }

        StructureBuilderCore(const StructureBuilderCore &other) = default;

    protected:
        // Clone impl, this returns a type-erased ISolidPtr
        // But in reality must bee a Builder here to be downcast by Clone().
        SharedPtr<IStructureBuilder> clone_impl() const override;

        StructureBuilder ToStructureView() const override;

    public:
        StructureBuilderCore &operator=(const StructureBuilderCore &other) = delete;
    };
}


#endif
