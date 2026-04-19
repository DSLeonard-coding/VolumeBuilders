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
#include "VolumeBuildersTypes.hh"
#include "VolumeBuilderBase.hh"
#include "StructureBuilderCore.hh"
// ReSharper disable once CppUnusedIncludeDirective
#include <memory>

namespace DLG4::VolumeBuilders {
    // See other Derived classes, or better yet the Factories "Topic" in the Doxygen html manual,
    // For Factories for other types of shapes or sources.

    /** @addtogroup Factories  Volume/Structure Builder Factories
     *  @brief Factories for creating VolumeBuilder and StructureBuilder objects.
     *  @{
    **/


    /**
    * @brief Create an unconfigured Box Solid, for use with SetXEdges() etc...
    * Uses the global default unit unless you call SetDefaultUnit() before setting sizes.
    * @param name  Base name to derive geometry object names from.
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    **/
    BoxBuilder CreateBoxBuilder(const G4String &name);

    /**
    * @brief Create an unconfigured Box Solid, for use with SetXEdges() etc...
    * @param name  Base name to derive geometry object names from.
    * @param unit  unit for this call only (ex: CLHEP::mm)
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilder CreateBoxBuilder(const G4String &name, G4double unit);

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
    BoxBuilder CreateCenteredBoxBuilder(
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
    BoxBuilder CreateCenteredBoxBuilder
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
    BoxBuilder CreateZDeltaBoxBuilder(
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
    BoxBuilder CreateZDeltaBoxBuilder
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
    BoxBuilder CreateDeltasBoxBuilder(
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
    BoxBuilder CreateDeltasBoxBuilder(G4double unit,
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
    BoxBuilder CreateEdgesBoxBuilder(const G4String &name, G4double x_edge1, G4double x_edge2,
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
    BoxBuilder CreateEdgesBoxBuilder(G4double unit, const G4String &name, G4double x_edge1,
        G4double x_edge2,
        G4double y_edge1, G4double y_edge2, G4double z_edge1, G4double z_edge2);

    /** End Factories
     * @} */
}

namespace DLG4::VolumeBuilders::_internals_ {

    /**
     * @class BoxBuilderCore
     * @brief Builder class for simple Box solids.
     * @details Use the CreateXXX() factory methods to create the buiders.
     * In particular we can create boxes that are not referenced to the center!!
     * @headerfile BoxBuilder.hh
     * @see VolumeBuilder<U> for inherited methods.
     * @ingroup Builders
     */
    class BoxBuilderCore final: public VolumeBuilderBase<BoxBuilderCore> {
    public:
        friend class VolumeBuilderCore;
        friend class VolumeBuilderBase<BoxBuilderCore>; // shouldn't be needed, maybe isn't now.
        template <typename T>
        friend class i_shared_ptr; // needed in principle, but maybe not for this class.

        // Friend all the factories.  Keeping them external is easier for users, but more boilerplate.
        friend BoxBuilder VB::CreateBoxBuilder(const G4String &name);
        friend BoxBuilder VB::CreateBoxBuilder(const G4String &name, G4double unit);
        friend BoxBuilder VB::CreateDeltasBoxBuilder(const G4String &name, G4double edge_x,
            G4double x_delta, G4double edge_y, G4double y_delta, G4double edge_z, G4double z_delta);
        friend BoxBuilder VB::CreateDeltasBoxBuilder(G4double unit, const G4String &name,
            G4double edge_x, G4double x_delta, G4double edge_y, G4double y_delta, G4double edge_z,
            G4double z_delta);
        friend BoxBuilder VB::CreateZDeltaBoxBuilder(const G4String &name, G4double x_full_size,
            G4double y_full_size, G4double edge_z, G4double z_delta);
        friend BoxBuilder VB::CreateZDeltaBoxBuilder(G4double unit, const G4String &name,
            G4double x_full_size, G4double y_full_size, G4double edge_z, G4double z_delta);
        friend BoxBuilder VB::CreateCenteredBoxBuilder(const G4String &name, G4double x_full_size,
            G4double y_full_size, G4double z_full_size);
        friend BoxBuilder VB::CreateCenteredBoxBuilder(G4double unit, const G4String &name,
            G4double x_full_size, G4double y_full_size, G4double z_full_size);
        friend BoxBuilder VB::CreateEdgesBoxBuilder(const G4String &name, G4double x_edge1,
            G4double x_edge2, G4double y_edge1, G4double y_edge2, G4double z_edge1,
            G4double z_edge2);
        friend BoxBuilder VB::CreateEdgesBoxBuilder(G4double unit, const G4String &name,
            G4double x_edge1, G4double x_edge2, G4double y_edge1, G4double y_edge2,
            G4double z_edge1, G4double z_edge2);

        BoxBuilder SetXSize(G4double x_size);
        BoxBuilder SetYSize(G4double y_size);
        BoxBuilder SetZSize(G4double z_size);

        BoxBuilder SetXEdges(G4double x_edge1, G4double x_edge2);
        BoxBuilder SetYEdges(G4double y_edge1, G4double y_edge2);
        BoxBuilder SetZEdges(G4double z_edge1, G4double z_edge2);

        BoxBuilder SetXEdgeDelta(G4double x_edge, G4double x_delta);
        BoxBuilder SetYEdgeDelta(G4double y_edge, G4double y_delta);
        BoxBuilder SetZEdgeDelta(G4double z_edge, G4double z_delta);

        BoxBuilder SetInternalOffset(G4double x, G4double y, G4double z);

        // BoxBuilder method overloads with a leading `unit` parameter.
        // These allow for a per-call unit override.

        BoxBuilder SetXSize(G4double unit, G4double x_size);
        BoxBuilder SetYSize(G4double unit, G4double y_size);
        BoxBuilder SetZSize(G4double unit, G4double z_size);

        BoxBuilder SetXEdges(G4double unit, G4double x_edge1, G4double x_edge2);
        BoxBuilder SetYEdges(G4double unit, G4double y_edge1, G4double y_edge2);
        BoxBuilder SetZEdges(G4double unit, G4double z_edge1, G4double z_edge2);

        BoxBuilder SetXEdgeDelta(G4double unit, G4double x_edge, G4double x_delta);
        BoxBuilder SetYEdgeDelta(G4double unit, G4double y_edge, G4double y_delta);
        BoxBuilder SetZEdgeDelta(G4double unit, G4double z_edge, G4double z_delta);

        BoxBuilder SetInternalOffset(G4double unit, G4double x, G4double y, G4double z);

    private:
        BoxBuilder SetXSizeDimensioned(G4double x_size);
        BoxBuilder SetYSizeDimensioned(G4double y_size);
        BoxBuilder SetZSizeDimensioned(G4double z_size);
        BoxBuilder SetInternalOffsetDimensioned(G4double x, G4double y, G4double z);

    protected:
        ///The polymorphic Solid constructor
        G4VSolid *SolidConstructor(const G4String &name) override;

    private:
        explicit BoxBuilderCore(const G4String &name);

        BoxBuilderCore(const BoxBuilderCore &other);

        // offset of box from center.  This effectively redefines the center of the solid
        // as far as the builder behavior.
        G4double z_size_{};
        G4double x_size_{};
        G4double y_size_{};
        G4double unit_{};


        BoxBuilderCore() = default;
        BoxBuilderCore(BoxBuilderCore &&) noexcept = delete;

        // BoxBuilder Clone() const override {
        //     return BoxBuilder(new BoxBuilderCore(*this));
        // }
    };
}
