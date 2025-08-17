#pragma once

/*
 * BoxBuilder.hh
 *
 *  Created on: Jun 19, 2024
 *      Author: D. S. Leonard
 *
 *
 *
 */

/** Class by D. S. Leonard to define (offset) box solids
 * Internally just a normal solid, but offset is carried around and added to the transform stack.
 * Inherits ability to configure and make the logical volume too.
 *  ... and to add/subtract volumes.
 */
#include "VolumeBuilderTypes.hh"
#include "VolumeBuilder.hh"
#include "StructureBuilderReference.hh"
// ReSharper disable once CppUnusedIncludeDirective
#include <memory>

namespace DLG4::VolumeBuilders {
    class BoxBuilder;
    template <typename T>
    class VolumeBuilder;
    using BoxBuilderPtr = SharedPtr<BoxBuilder>;
    // See other Derived classes, or better yet the Factories "Topic" in the Doxygen html manual,
    // For Factories for other types of shapes or sources.

    /** @addtogroup Factories  Volume/Structure Builder Factories
     *  @brief Factories for creating VolumeBuilder and StructureBuilder objects.
     *  @{
    /**


    /**
    * @brief Create an unconfigured Box Solid, for use with SetXEdges() etc...
    * Uses the global default unit unless you call SetDefaultUnit() before setting sizes.
    * @param name  Base name to derive geometry object names from.
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateBoxBuilder(const G4String &name);

    /**
    * @brief Create an unconfigured Box Solid, for use with SetXEdges() etc...
    * @param name  Base name to derive geometry object names from.
    * @param unit  unit for this call only (ex: CLHEP::mm)
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double unit);

    /**
     * @brief Create a Box solid.
     * Uses the global default unit
     * @param name  Base name to derive geometry object names from.
     * @param x_full_size  FULL X size
     * @param y_full_size  FULL Y size
     * @param z_full_size  FULL Z size
     * @param name
     * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
     *     See VolumeBuilder for inherited public methods including Union/Subtraction.
     *     @ingroup BoxBuilder
     * */
    BoxBuilderPtr CreateCenteredBoxBuilder(
        const G4String &name, G4double x_full_size, G4double y_full_size, G4double z_full_size);

    /**
    * @brief Create a Box solid.
    * @param unit  The unit to use for this call only (ex: CLHEP::mm)
    * @param name  Base name to derive geometry object names from.
    * @param x_full_size  FULL X size
    * @param y_full_size  FULL Y size
    * @param z_full_size  FULL Z size
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateCenteredBoxBuilder
    (G4double unit, const G4String &name, G4double x_full_size, G4double y_full_size,
        G4double z_full_size);

    /**
    * @brief Create a Box solid.
    * Uses the global default unit.
    * @param name  Base name to derive geometry object names from.
    * @param x_full_size  FULL X size
    * @param y_full_size  FULL Y size
    * @param edge_z  Full Z size
    * @param z_delta  Full Z size
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateZDeltaBoxBuilder(
        const G4String &name, G4double x_full_size, G4double y_full_size, G4double edge_z,
        G4double z_delta);

    /**
    * @brief Create a Box solid.
    * @param unit  The unit to use for this call only (ex: CLHEP::mm)
    * @param name  Base name to derive geometry object names from.
    * @param x_full_size  FULL X size
    * @param y_full_size  FULL Y size
    * @param edge_z  Full Z size
    * @param z_delta  Full Z size
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateZDeltaBoxBuilder
    (G4double unit, const G4String &name, G4double x_full_size, G4double y_full_size,
        G4double edge_z, G4double z_delta);


    /**
    * @brief Create a Box solid.
        * Uses the global default unit
 * @param name  Base name to derive geometry object names from.
    * @param edge_x  The x reference of one end (either one)
    * @param x_delta  FULL X size, + or - from end_x.
    * @param edge_y  The y reference of one end (either one)
    * @param y_delta  FULL Y size. + or - from end_y.
    * @param edge_z  The z reference of one end (either one)
    * @param z_delta  Height, can be positive or negative (from z reference)
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateDeltasBoxBuilder(
        const G4String &name, G4double edge_x, G4double x_delta, G4double edge_y, G4double y_delta,
        G4double edge_z, G4double z_delta);

    /**
    * @brief Create a Box solid.
    * @param unit  The unit to use for this call only (ex: CLHEP::mm)
    * @param name  Base name to derive geometry object names from.
    * @param edge_x  The x reference of one end (either one)
    * @param x_delta  FULL X size, + or - from end_x.
    * @param edge_y  The y reference of one end (either one)
    * @param y_delta  FULL Y size. + or - from end_y.
    * @param edge_z  The z reference of one end (either one)
    * @param z_delta  Height, can be positive or negative (from z reference)
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateDeltasBoxBuilder(G4double unit,
        const G4String &name, G4double edge_x, G4double x_delta, G4double edge_y, G4double y_delta,
        G4double edge_z, G4double z_delta);

    /**
     * @brief Create a Box solid from edge coordinates.
     * Uses the global default unit
     * @param name Base name to derive geometry object names from.
     * @param x_edge1 One end of the box in the x-dimension.
     * @param x_edge2 The other end of the box in the x-dimension.
     * @param y_edge1 One end of the box in the y-dimension.
     * @param y_edge2 The other end of the box in the y-dimension.
     * @param z_edge1 One end of the box in the z-dimension.
     * @param z_edge2 The other end of the box in the z-dimension.
     * @return The builder. Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
     * See VolumeBuilder for inherited public methods including Union/Subtraction.
     * @ingroup BoxBuilder
     * */
    BoxBuilderPtr CreateEdgesBoxBuilder(const G4String &name, G4double x_edge1, G4double x_edge2,
        G4double y_edge1,
        G4double y_edge2, G4double z_edge1, G4double z_edge2);

    /**
     * @brief Create a Box solid from edge coordinates using a specified unit.
     * @param unit The unit to use for this call only (ex: CLHEP::mm)
     * @param name Base name to derive geometry object names from.
     * @param x_edge1 One end of the box in the x-dimension.
     * @param x_edge2 The other end of the box in the x-dimension.
     * @param y_edge1 One end of the box in the y-dimension.
     * @param y_edge2 The other end of the box in the y-dimension.
     * @param z_edge1 One end of the box in the z-dimension.
     * @param z_edge2 The other end of the box in the z-dimension.
     * @return The builder. Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
     * See VolumeBuilder for inherited public methods including Union/Subtraction.
     * @ingroup BoxBuilder
     * */
    BoxBuilderPtr CreateEdgesBoxBuilder(G4double unit, const G4String &name, G4double x_edge1,
        G4double x_edge2,
        G4double y_edge1, G4double y_edge2, G4double z_edge1, G4double z_edge2);

    /** End Factories
     * @} */


    /**
     * @class BoxBuilder
     * @brief Builder class for simple Box solids.
     * @details Use the CreateXXX() factory methods to create the buiders.
     * In particular we can create boxes that are not referenced to the center!!
     * @headerfile BoxBuilder.hh
     * @see VolumeBuilder<U> for inherited methods.
     * @ingroup Builders
     */
    class BoxBuilder final: public VolumeBuilder<BoxBuilder> {
    public:
        friend class VolumeBuilderReference;
        friend class VolumeBuilder<BoxBuilder>; // shouldn't be needed, maybe isn't now.
        template <typename T>
        friend class i_shared_ptr; // needed in principle, but maybe not for this class.

        // Friend all the factories.  Keeping them external is easier for users, but more boilerplate.
        friend BoxBuilderPtr CreateBoxBuilder(const G4String &name);
        friend BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double unit);
        friend BoxBuilderPtr CreateDeltasBoxBuilder(const G4String &name, G4double edge_x,
            G4double x_delta, G4double edge_y, G4double y_delta, G4double edge_z, G4double z_delta);
        friend BoxBuilderPtr CreateDeltasBoxBuilder(G4double unit, const G4String &name,
            G4double edge_x, G4double x_delta, G4double edge_y, G4double y_delta, G4double edge_z,
            G4double z_delta);
        friend BoxBuilderPtr CreateZDeltaBoxBuilder(const G4String &name, G4double x_full_size,
            G4double y_full_size, G4double edge_z, G4double z_delta);
        friend BoxBuilderPtr CreateZDeltaBoxBuilder(G4double unit, const G4String &name,
            G4double x_full_size, G4double y_full_size, G4double edge_z, G4double z_delta);
        friend BoxBuilderPtr CreateCenteredBoxBuilder(const G4String &name, G4double x_full_size,
            G4double y_full_size, G4double z_full_size);
        friend BoxBuilderPtr CreateCenteredBoxBuilder(G4double unit, const G4String &name,
            G4double x_full_size, G4double y_full_size, G4double z_full_size);
        friend BoxBuilderPtr CreateEdgesBoxBuilder(const G4String &name, G4double x_edge1,
            G4double x_edge2, G4double y_edge1, G4double y_edge2, G4double z_edge1,
            G4double z_edge2);
        friend BoxBuilderPtr CreateEdgesBoxBuilder(G4double unit, const G4String &name,
            G4double x_edge1, G4double x_edge2, G4double y_edge1, G4double y_edge2,
            G4double z_edge1, G4double z_edge2);

        BoxBuilderPtr SetXSize(G4double x_size);
        BoxBuilderPtr SetYSize(G4double y_size);
        BoxBuilderPtr SetZSize(G4double z_size);

        BoxBuilderPtr SetXEdges(G4double x_edge1, G4double x_edge2);
        BoxBuilderPtr SetYEdges(G4double y_edge1, G4double y_edge2);
        BoxBuilderPtr SetZEdges(G4double z_edge1, G4double z_edge2);

        BoxBuilderPtr SetXEdgeDelta(G4double x_edge, G4double x_delta);
        BoxBuilderPtr SetYEdgeDelta(G4double y_edge, G4double y_delta);
        BoxBuilderPtr SetZEdgeDelta(G4double z_edge, G4double z_delta);

        BoxBuilderPtr SetInternalOffset(G4double x, G4double y, G4double z);

        // BoxBuilder method overloads with a leading `unit` parameter.
        // These allow for a per-call unit override.

        BoxBuilderPtr SetXSize(G4double unit, G4double x_size);
        BoxBuilderPtr SetYSize(G4double unit, G4double y_size);
        BoxBuilderPtr SetZSize(G4double unit, G4double z_size);

        BoxBuilderPtr SetXEdges(G4double unit, G4double x_edge1, G4double x_edge2);
        BoxBuilderPtr SetYEdges(G4double unit, G4double y_edge1, G4double y_edge2);
        BoxBuilderPtr SetZEdges(G4double unit, G4double z_edge1, G4double z_edge2);

        BoxBuilderPtr SetXEdgeDelta(G4double unit, G4double x_edge, G4double x_delta);
        BoxBuilderPtr SetYEdgeDelta(G4double unit, G4double y_edge, G4double y_delta);
        BoxBuilderPtr SetZEdgeDelta(G4double unit, G4double z_edge, G4double z_delta);

        BoxBuilderPtr SetInternalOffset(G4double unit, G4double x, G4double y, G4double z);

    private:
        BoxBuilderPtr SetXSizeDimensioned(G4double x_size);
        BoxBuilderPtr SetYSizeDimensioned(G4double y_size);
        BoxBuilderPtr SetZSizeDimensioned(G4double z_size);
        BoxBuilderPtr SetInternalOffsetDimensioned(G4double x, G4double y, G4double z);

    protected:
        ///The polymorphic Solid constructor
        G4VSolid *SolidConstructor(const G4String &name) override;

    private:
        explicit BoxBuilder(const G4String &name);

        BoxBuilder(const BoxBuilder &other);

        // offset of box from center.  This effectively redefines the center of the solid
        // as far as the builder behavior.
        G4double z_size_{};
        G4double x_size_{};
        G4double y_size_{};
        G4double unit_{};


        BoxBuilder() = default;
        BoxBuilder(BoxBuilder &&) noexcept = delete;

        // BoxBuilderPtr Clone() const override {
        //     return BoxBuilderPtr(new BoxBuilder(*this));
        // }
    };
}