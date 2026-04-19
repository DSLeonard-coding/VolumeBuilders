#pragma once
#ifndef DLG4_ASSEMBLY_HPP
#define DLG4_ASSEMBLY_HPP
//
// Created by Douglas S. Leonard on 6/8/25.  All rights Reserved
// See related files for license, if any is provided.
//
// Assemblies for treating a collection of builders as a single unit.
//#include "StructureBuilder.hh"
#include "Linkable.hh"
#include "i_shared_ptr.hh"
#include <stdexcept>
//#include <VolumeBuilder.hh>
#include <VolumeBuildersTypes.hh>
#include <StructureBuilderBase.hh>
#include <BuilderFactories.hh>

// Factory CreateAssembly moved to BuilderFactories.hh

namespace DLG4::VolumeBuilders::_internals_ {
    /**
     *@class AssemblyCore
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
     * @headerfile StructureBuilderBase.hh
     * @see StructureBuilder for inherited methods.
     * @ingroup Builders
     * */
    class AssemblyCore final: public StructureBuilderBase<AssemblyCore> {
        template <typename T>
        friend class VolumeBuilderBase;
        template <typename T>
        friend class StructureBuilderBase;
        friend Assembly VB::CreateAssembly(G4String name);

        friend class i_shared_ptr<AssemblyCore>;

    public:
        Assembly AddStructure(const StructureBuilder &other);

    private:
        //ctor used by factory.  Will construct an assembly from a builder actually.
        template <typename T>
        AssemblyCore(i_shared_ptr<T> other, // NOLINT(*-explicit-constructor)
            std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>,
                int>  = 0) : StructureBuilderBase<AssemblyCore>(other, SET_LINK) {
        }

        [[noreturn]]
        G4VSolid *SolidConstructor(const G4String &name) override {
            throw std::runtime_error(
                "Error in StructureBuilderCore::SolidConstructor(const G4String &name) "
                + this->builder_configs_->name + " \n" +
                "SolidConstructor(const G4String &name) is not implemented.");
        }

        template <typename T>
        friend class i_shared_ptr;


        AssemblyCore(const AssemblyCore &other);
        AssemblyCore() = default;

    protected:
        // Clone impl, this returns a type-erased ISolidPtr
        // But in reality must bee a Builder here to be downcast by Clone().

    public:
        AssemblyCore &operator=(const AssemblyCore &other) = delete;
    };
}


#endif
