#pragma once
//
// Created by Douglas S. Leonard on 6/18/25.  All rights Reserved
// See related files for license, if any is provided.
//
// Types for VolumeBuilder classes

#ifndef VOLUMEBUILDERSTYPES_HH
#define VOLUMEBUILDERSTYPES_HH
#include <G4ThreeVector.hh>
#include <memory>
#include <optional>
#include <vector>

#include "disableable_shared_from_this.hh"
#include "i_shared_ptr.hh"

#define ONCE_MUTABLE mutable  // for linkable objects

namespace DLG4::VolumeBuilders::_internals_ {
    // a shorthand out to the public namespace:
    namespace VB = DLG4::VolumeBuilders;

    // Base template alias
    template<typename T>
    using SharedPtr = i_shared_ptr<T>;
    class ISolidBuilder;
    class IStructureBuilder;
    class IVolumeBuilder;
    template<typename T>
    class VolumeBuilder;
    class BuilderViewCore;
    class RZBuilder;
    class BoxBuilder;
    class FromG4VSolid;
    template<typename T>
    class StructureBuilder;
    class StructureViewCore;
    class Assembly;

    /**
     * True polymorphic class base view for all builders
     * Mostly for internal use.
     */
    using ISolidPtr = SharedPtr<ISolidBuilder>;

    /**
     * True polymorphic class base view for all structures
     * Mostly for internal use.
     */
    using IStructurePtr = SharedPtr<IStructureBuilder>;


    /**
     * @typedef BuilderView
     * @brief Common interface for all volume builder types
     * @details A shared pointer to \_internals\_::BuilderViewCore
     * All specific builder pointer types (RZBuilderPtr, FromG4VSolidPtr, etc.)
     * are implicitly convertible to this type and provide polymorphic access
     * to volume operations.
     */
    using BuilderView = SharedPtr<BuilderViewCore>;
    /**
     * @typedef StructureView
     * @brief Common interface for all structure types. \n
     * Includes builders and assemblies, and methods for LogicalVolume rendering and placement.
     * see \_internals\_::StructureViewCore for the common interface.
     */
    using StructureView = SharedPtr<StructureViewCore>;

    /// Shared pointer to \_internals\_::RZBuilder
    using RZBuilderPtr = SharedPtr<RZBuilder>;

    /// Shared pointer to \_internals\_::FromG4VSolid
    using FromG4VSolidPtr = SharedPtr<FromG4VSolid>;

    /// Shared pointer to \_internals\_::BoxBuilder
    using BoxBuilderPtr = SharedPtr<BoxBuilder>;

    /**
     * @typedef AssemblyPtr
     * @brief Pointer to Assembly type to hold Structures, ie other assemblies or buidlers.
     * See \_internals\_::Assembly.
     */
    using AssemblyPtr = SharedPtr<Assembly>;

    /**
     * @brief a user type to hold many builders
     * @details You can add to them directly with ->EmplaceBackInto(yourlist)
     * But you should be able to add builders to a vector of, for example,
     * 4LogicalVolume * as well since type converting operators are defined.!!!
     */
    using BuilderViewList = std::vector<BuilderView>;

    /**
     * @brief a user type to hold many structures
     * @details You can add to them directly with ->EmplaceBackInto(yourlist)
     * But you should be able to add builders to a vector of, for example,
     * 4LogicalVolume * as well since type converting operators are defined.!!!
     */
    using StructureViewList = std::vector<StructureView>;

    // For the template base class
    template<typename U>
    using VolumeBuilderPtr = SharedPtr<U>;

    template<typename U>
    using ENABLE_SHARED_WRAPPER = Utilities::disableable_shared_from_this<U>;

    using UnitlessG4Transform3D = G4Transform3D;
    using Unitless3Vec = G4ThreeVector;
    /**
     * @brief A 3D vector that carries its own unit information
     * @details Stores unit, x, y, z as separate values.
     * Can be constructed from 3 values (gets default unit) or 4 values (explicit unit first)
     */
    class Unit3Vec {
    public:
        double x, y, z;
        std::optional<double> units;

        // 3-value constructor - no units specified, raw values
        Unit3Vec(double x, double y, double z)
            : x(x), y(y), z(z), units(std::nullopt) {
        }

        // 4-value constructor - units specified, scale the values
        Unit3Vec(double u, double x, double y, double z)
            : x(x), y(y), z(z), units(u) {
        }

        Unit3Vec(double u, const Unitless3Vec &vec)
            : x(vec.getX()), y(vec.getY()), z(vec.getZ()), units(u) {
        }

        [[nodiscard]] G4ThreeVector apply_units(const double dflt_unit) const {
            auto vec = G4ThreeVector(0, 0, 0);
            auto unit = this->units.value_or(dflt_unit);
            vec.setX(this->x * unit);
            vec.setY(this->y * unit);
            vec.setZ(this->z * unit);
            return vec;
        }
    };

    template<typename T>
    std::shared_ptr<T> shared_mutable_this(const std::enable_shared_from_this<T> *obj) {
        // Call shared_from_this() (returns shared_ptr<const T> if obj is const)
        auto sp_const = obj->shared_from_this();

        // Cast away constness safely
        return std::const_pointer_cast<std::remove_const_t<T> >(sp_const);
    }
}

//Export names for API usage/tab-completion.
///@namespace DLG4::VolumeBuilders
namespace DLG4::VolumeBuilders {
    /// @copydoc _internals_::RZBuilderPtr
    using RZBuilderPtr = _internals_::RZBuilderPtr;
    /// @copydoc _internals_::BoxBuilderPtr
    using BoxBuilderPtr = _internals_::BoxBuilderPtr;
    /// @copydoc _internals_::AssemblyPtr
    using AssemblyPtr = _internals_::AssemblyPtr;
    /// @copydoc _internals_::FromG4VSolidPtr
    using FromG4VSolidPtr = _internals_::FromG4VSolidPtr;
    /// @copydoc _internals_::VolumeBuilderPtr
    template<typename T>
    using VolumeBuilderPtr = _internals_::VolumeBuilderPtr<T>;
    /// @copydoc _internals_::BuilderView
    using BuilderView = _internals_::BuilderView;
    /// @copydoc _internals_::BuilderViewList
    using BuilderViewList = _internals_::BuilderViewList;
    /// @copydoc _internals_::StructureView
    using StructureView = _internals_::StructureView;
    /// @copydoc _internals_::StructureViewList
    using StructureViewList = _internals_::StructureViewList;
    /// @copydoc _internals_::Unit3Vec
    using Unit3Vec = _internals_::Unit3Vec;
    /// @copydoc _internals_::Unitless3Vec
    using Unitless3Vec = _internals_::Unitless3Vec;
}

namespace DLG4::VolumeBuilders::Builders {
    using _internals_::VolumeBuilder;
    using _internals_::StructureBuilder;
    using RZBuilder = _internals_::RZBuilder;
    using BoxBuilder = _internals_::BoxBuilder;
    using FromG4VSolid = _internals_::FromG4VSolid;
    using Assembly = _internals_::Assembly;
    using BuilderViewCore = _internals_::BuilderViewCore;
    using StructureViewCore = _internals_::StructureViewCore;
}
#endif  //VOLUMEBUILDERSTYPES_HH
