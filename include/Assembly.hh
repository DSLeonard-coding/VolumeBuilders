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
#include <VolumeBuilder.hh>
#include <VolumeBuilderTypes.hh>
#include <StructureBuilder.hh>

namespace DLG4::VolumeBuilders {
    template <typename T>
    class VolumeBuilder;
    //    class StructureBuilder;

    /**
     * @brief Assembly of strucures, ie builders and/or other assemblies
     * \dotfile builder_graph.dot
     * @ingroup Factories
    * */
    AssemblyPtr CreateAssembly(G4String name);

    /**
     *@class Assembly
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
     * @headerfile StructureBuilder.hh
     * @see StructureBuilder for inherited methods.
     * @ingroup Builders
     * */
    class Assembly final: public StructureBuilder<Assembly> {
        template <typename T>
        friend class VolumeBuilder;
        template <typename T>
        friend class StructureBuilder;
        friend AssemblyPtr CreateAssembly(G4String name);

        friend class i_shared_ptr<Assembly>;

    public:
        AssemblyPtr AddStructure(const StructureView &other);

    private:
        //ctor used by factory.  Will construct an assembly from a builder actually.
        template <typename T>
        Assembly(i_shared_ptr<T> other, // NOLINT(*-explicit-constructor)
            std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>,
                int>  = 0) : StructureBuilder<Assembly>(other, SET_LINK) {
        }

        [[noreturn]]
        G4VSolid *SolidConstructor(const G4String &name) override {
            throw std::runtime_error(
                "Error in StructureBuilderReference::SolidConstructor(const G4String &name) "
                + this->builder_configs_->name + " \n" +
                "SolidConstructor(const G4String &name) is not implemented.");
        }

        template <typename T>
        friend class i_shared_ptr;


        Assembly(const Assembly &other);
        Assembly() = default;

    protected:
        // Clone impl, this returns a type-erased ISolidPtr
        // But in reality must bee a BuilderPtr here to be downcast by Clone().

    public:
        Assembly &operator=(const Assembly &other) = delete;
    };
}


#endif
