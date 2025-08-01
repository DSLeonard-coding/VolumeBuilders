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
     *  @{ */
    /**
    /**
    * @brief Create a Box solid.
    * @param name  Base name to derive geometry object names from.
    * @param end_x  The x reference of one end (either one)
    * @param x_size  FULL X size, + or - from end_x.
    * @param end_y  The y reference of one end (either one)
    * @param y_size  FULL Y size. + or - from end_y.
    * @param end_z  The z reference of one end (either one)
    * @param z_size  Height, can be positive or negative (from z reference)
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */

    BoxBuilderPtr CreateBoxBuilder(
        const G4String &name, G4double end_x, G4double x_size, G4double end_y, G4double y_size, G4double end_z, G4double z_size);

    /**
    * @brief Create a Box solid.
    * @param unit  The unit to use for this object only (ex: CLHEP::mm)
    * @param name  Base name to derive geometry object names from.
    * @param end_x  The x reference of one end (either one)
    * @param x_size  FULL X size, + or - from end_x.
    * @param end_y  The y reference of one end (either one)
    * @param y_size  FULL Y size. + or - from end_y.
    * @param end_z  The z reference of one end (either one)
    * @param z_size  Height, can be positive or negative (from z reference)
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateBoxBuilder(G4double unit,
        const G4String &name, G4double end_x, G4double x_size, G4double end_y, G4double y_size, G4double end_z, G4double z_size);

    /**
     * @brief Create a Box solid.
     * @param name  Base name to derive geometry object names from.
     * @param x_size  FULL X size
     * @param y_size  FULL Y size
     * @param z_size  Height, can be positive or negative (from z reference)
     * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
     *     See VolumeBuilder for inherited public methods including Union/Subtraction.
     *     @ingroup BoxBuilder
     * */
    BoxBuilderPtr CreateBoxBuilder(
        const G4String &name, G4double x_size, G4double y_size,  G4double end_z, G4double z_size);

    /**
    * @brief Create a Box solid.
    * @param unit  The unit to use for this object only (ex: CLHEP::mm)
    * @param name  Base name to derive geometry object names from.
    * @param x_size  FULL X size
    * @param y_size  FULL Y size
    * @param z_size  Full Z size
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
   BoxBuilderPtr CreateBoxBuilder
        (G4double unit, const G4String &name,  G4double x_size, G4double y_size, G4double end_z, G4double z_size);

    /**
 * @brief Create a Box solid.
 * @param name  Base name to derive geometry object names from.
 * @param x_size  FULL X size
 * @param y_size  FULL Y size
 * @param z_size  FULL Z size
 * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
 *     See VolumeBuilder for inherited public methods including Union/Subtraction.
 *     @ingroup BoxBuilder
 * */
    BoxBuilderPtr CreateBoxBuilder(
        const G4String &name, G4double x_size, G4double y_size,  G4double z_size);

    /**
    * @brief Create a Box solid.
    * @param unit  The unit to use for this object only (ex: CLHEP::mm)
    * @param name  Base name to derive geometry object names from.
    * @param x_size  FULL X size
    * @param y_size  FULL Y size
    * @param z_size  FULL Z size
    * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
    *     See VolumeBuilder for inherited public methods including Union/Subtraction.
    *     @ingroup BoxBuilder
    * */
    BoxBuilderPtr CreateBoxBuilder
         (G4double unit, const G4String &name,  G4double x_size, G4double y_size, G4double z_size);
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
    class BoxBuilder: public VolumeBuilder<BoxBuilder> {
    public:
        friend class VolumeBuilderReference;
        friend class VolumeBuilder<BoxBuilder>; // shouldn't be needed, maybe isn't now.
        template <typename T>
        friend class i_shared_ptr;             // needed in principle, but maybe not for this class.

        // Friend all the factories.  Keeping them external is easier for users, but more boilerplate.
        friend BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double end_x, G4double x_size, G4double end_y, G4double y_size, G4double end_z, G4double z_size);
        friend BoxBuilderPtr CreateBoxBuilder(G4double unit, const G4String &name, G4double end_x, G4double x_size, G4double end_y, G4double y_size, G4double end_z, G4double z_size);
        friend BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double x_size, G4double y_size, G4double end_z, G4double z_size);
        friend BoxBuilderPtr CreateBoxBuilder(G4double unit, const G4String &name, G4double x_size, G4double y_size, G4double end_z, G4double z_size);
        friend BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double x_size, G4double y_size, G4double z_size);
        friend BoxBuilderPtr CreateBoxBuilder(G4double unit, const G4String &name, G4double x_size, G4double y_size, G4double z_size);

        BoxBuilderPtr SetXSize(G4double x_size);
        BoxBuilderPtr SetYSize(G4double y_size);
        BoxBuilderPtr SetZSize(G4double z_size);
        BoxBuilderPtr SetInternalOffset(G4double x, G4double y, G4double z);

    protected:
        ///The polymorphic Solid constructor
        G4VSolid* SolidConstructor(const G4String &name) override;

    private:

        explicit BoxBuilder( const G4String &name);

        BoxBuilder(const BoxBuilder &other);

        // offset of box from center.  This effectively redefines the center of the solid
        // as far as the builder behavior.
        G4double z_size_{};
        G4double x_size_{};
        G4double y_size_{};
        G4double unit_{};


        BoxBuilder() = default;
        BoxBuilder(BoxBuilder &&) noexcept = default;

        // BoxBuilderPtr Clone() const override {
        //     return BoxBuilderPtr(new BoxBuilder(*this));
        // }
    };
}
