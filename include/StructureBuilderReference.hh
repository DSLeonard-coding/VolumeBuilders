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
#include <StructureBuilder.hh>
#include <VolumeBuilderTypes.hh>


namespace DLG4::VolumeBuilders {
    template <typename T>
    class VolumeBuilder;
    //    class StructureBuilder;


    /**
     *@class StructureBuilderReference
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
     * This derived clas really exists for parallelism with VolumeBuilderReference.
     *
     * @headerfile StructureBuilder.hh
     * @see VolumeBuilder for inherited methods.
     * */
    class StructureBuilderReference: public StructureBuilder<StructureBuilderReference> {
        template <typename T>
        friend class VolumeBuilder;
        template <typename T>
        friend class StructureBuilder;

        template <typename T>
        friend class i_shared_ptr;

    private:
        friend class i_shared_ptr<StructureBuilderReference>;

        template <typename T>
        StructureBuilderReference(i_shared_ptr<T> other, // NOLINT(*-explicit-constructor)
                std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>,
                    int> = 0 ) : StructureBuilder<StructureBuilderReference>(other, SET_LINK) {
        }

        void SolidConstructor() override {
            throw std::runtime_error("Error in StructureBuilderReference::SolidConstructor() "
                                     + this->builder_configs_->name + " \n" +
                                     "SolidConstructor() is not implemented.");
        }

        StructureBuilderReference(const StructureBuilderReference &other) = default;

    protected:
        // Clone impl, this returns a type-erased ISolidPtr
        // But in reality must bee a BuilderPtr here to be downcast by Clone().
        SharedPtr<IStructureBuilder> clone_impl() const override;

        StructureView ToStructureView() const override;

    public:
        StructureBuilderReference &operator=(const StructureBuilderReference &other) = delete;
    };
}


#endif