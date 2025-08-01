#pragma once

/*
 * RZBuilder.hh
 *
 *  Created on: Jun 19, 2024
 *      Author: D. S. Leonard
 *
 *      The main user class is presently RZBuilder
 *      Which creates polycones and polyhedrals From z plane definitions.
 *      VolumeMaker which it inherits from provides more generic extended functionality (syntax helpers) to it.
 *      Such as logical volume creation, unions/subtractions, and vis attribute settings.
 *      Features/Benefits
 *      -Unit is separate and remembered for cleaner/simpler dimensioning
 *                        (no * mm everwhere, no double unit application, no forgotten unit).
 *      -z=0 point can be outside of object, allowing many objects to reference from a single z.
 *      -Much simpler than using unions for multi-plane objects
 *         - No overlap/z-shift/half-height/sign confusions for every union part.
 *      -no half height math everywhere (or doubled or forgotten application of it).
 *      -Simplified union syntax when still needed.
 *      -Simplified vis_att syntax
 *      -Simplified logical volume construction.
 *      -Polymorphic methods could help in automation in principle.
 *
 *
 */

/** Class by D. S. Leonard to help define z-r planes to build poly-objects
 * Applies units from GeantPlane automatically.
 * Inherits ability to configure and make the logical volume too.
 *  ... and to add/subtract volumes.
 */
#include "VolumeBuilderTypes.hh"
#include "VolumeBuilder.hh"
#include "StructureBuilderReference.hh"
// ReSharper disable once CppUnusedIncludeDirective
#include <memory>

namespace DLG4::VolumeBuilders {
    /**
     * Struct for adding planes to GeantMultiPlane  --DSLeonard 2024
     *  Overloads make this not strictly needed.
     *  */
    struct RZPlane {
        G4double unit;          // unit
        G4double IR;           // inner radius
        G4double OR;           // outer radius
        G4double z;
        RZPlane() = default;
        RZPlane(G4double u, G4double ir, G4double or_, G4double z_)
     : unit(u), IR(ir), OR(or_), z(z_) {}
    };

    /**
     * RZPlane for use with global or preset units.
     * */
    struct RZPlaneUnitless {
        G4double IR{0};           // inner radius
        G4double OR{0};           // outer radius
        G4double z{0};
    };

    class RZBuilder;
    template <typename T>
    class VolumeBuilder;
    using RZBuilderPtr = SharedPtr<RZBuilder>;
    // See other Derived classes, or better yet the Factories "Topic" in the Doxygen html manual,
    // For Factories for other types of shapes or sources.

    // Parameters after name are only needed for polyhedra or angular information:
    /**
     * @defgroup Factories Builder/Structure Factories
     * @addtogroup Factories  Builder/Structure Factories
     * @details Factories for creating VolumeBuilder and StructureBuilder objects.
     *  @{ */
     /**
     * @brief Create a builder for associated IR,OR,Z defined object.
     * @param name  Base name to derive geometry object names from.
     * @param phi_start  Optional Start angle
     * @param phi_tot  Optional Total degrees
     * @param sides Optional number of sides, for CreatePoyhedra only, (can set it later, dflt = 4 )
     * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
     *     See VolumeBuilder for inherited public methods including Union/Subtraction.
     */
    RZBuilderPtr CreatePolyhedraBuilder(
        const G4String &name, int sides, G4double phi_start = 0., G4double phi_tot = 360);

    /// @copydoc VolumeBuilders::CreatePolyhedraBuilder
    RZBuilderPtr CreatePolyconeBuilder(
        const G4String &name, G4double phi_start = 0., G4double phi_tot = 360);

    /**
     * Create a simple cylinder builder
     * @param name  Name used to derive all product names.
     * @param unit Unit applied to all values.
     * @param endz  The z reference of one end (either one)
     * @param height  Height, can be positive or negative (from z reference)
     * @param OR Outer radius.
     * @param IR Optional inner radius.  Default=0
     * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
     *     See VolumeBuilder for inherited public methods including Union/Subtraction.
     */
    RZBuilderPtr CreateCylinderBuilder(G4double unit, const G4String &name,
        G4double endz, G4double height, G4double OR, G4double IR = 0);

    /**
     * Create a simple cylinder builder using global default unit.
     * @param name  Name used to derive all product names.
     * @param endz  The z reference of one end (either one)
     * @param height  Height, can be positive or negative (from z reference)
     * @param OR Outer radius.
     * @param IR Optional inner radius.  Default=0
     * @return The builder.  Set configurations and then call .MakeSolid() and .MakeLogicalVolume() on it.
     *     See VolumeBuilder for inherited public methods including Union/Subtraction.
     */
    RZBuilderPtr CreateCylinderBuilder(const G4String &name,
        G4double endz, G4double height, G4double OR, G4double IR = 0);
    /** @}*/

    /** @addtogroup Builders Builder Classes
     * @brief Sub-typed Builder classes constructed by the Factories.
     * @{ */
    /**
     * @class RZBuilder
     * @brief Builder class for RZ mult-plane defined solids.
     * @details Use the CreateXXX() factory methods to create the buiders.
     * @headerfile RZBuilder.hh
     * @see VolumeBuilder<U> for inherited methods.
     */
    class RZBuilder: public VolumeBuilder<RZBuilder> {
    /** @} */
    public:
        friend class VolumeBuilderReference;
        friend class VolumeBuilder<RZBuilder>; // shouldn't be needed, maybe isn't now.
        template <typename T>
        friend class i_shared_ptr;             // needed in principle, but maybe not for this class.

        // Friend all the factories.  Keeping them external is easier for users, but more boilerplate.
        friend RZBuilderPtr CreatePolyhedraBuilder(
            const G4String &name, int sides, G4double phi_start, G4double phi_tot );
        friend RZBuilderPtr CreatePolyconeBuilder(const G4String &name, G4double phi_start,
            G4double phi_tot );
        friend RZBuilderPtr CreateCylinderBuilder(G4double unit, const G4String &name ,
            G4double endz, G4double height, G4double OR, G4double IR );
        friend RZBuilderPtr CreateCylinderBuilder(const G4String &name,
            G4double endz, G4double height, G4double OR, G4double IR );

        /** @defgroup RZBuilder RZbuilder Configurations
         * Solid Configurations for RZBuiler
         * @{ */

        /**
             * Set number of sides.  Only relevant for things with sides.
             * @param N Number of Sides
             */
        virtual RZBuilderPtr SetNumSides(G4double N);

        /**
         * Adds a plane defining one IR,OR,Z triplet in the volume design
         * @param plane // the unit, IR, OR, Z data for this "plane"
         */
        RZBuilderPtr AddPlane(const RZPlane &plane);
        /**
         * Adds a plane defining one IR,OR,Z triplet in the volume design
         * Uses preset unit from default (mm) or SetUnit(unit);
         * @param IR Inner radius
         * @param OR Outer radius
         * @param z Z position
         */
        RZBuilderPtr AddPlane(G4double IR, G4double OR, G4double z);

        /**
         * Adds a plane defining one IR,OR,Z triplet in the volume design
         * Uses preset unit from default (mm) or SetUnit(unit);
         * @param unit The unit to use for THIS plane only (ex: CLHEP::mm)
         * @param IR Inner radius
         * @param OR Outer radius
         * @param z Z position
         */
        RZBuilderPtr AddPlane(G4double unit, G4double IR, G4double OR, G4double z);


        /**
         * Adds multiple RZ planes each defining one unit,IR,OR,Z set in the volume design
         * @param planes {{unit,IR, OR, Z},{unit,IR,OR,Z},...}, unit ex: CLHEP::mm
         */
        RZBuilderPtr AddPlanes(const std::vector<RZPlane>& planes);
        /**
         * Adds multiple planes each defining one IR,OR,Z triplet in the volume design
         * Uses preset unit from default (mm) or SetUnit(unit);
         * @param planes {{IR, OR, Z},{IR,OR,Z},...}
         */
        RZBuilderPtr AddPlanes(const std::vector<RZPlaneUnitless>& planes);
        /**
         * Adds multiple planes each defining one IR,OR,Z triplet in the volume design
         * Uses preset unit from default (mm) or SetUnit(unit);
         * @param unit The unit to use for THESE planes only (ex: CLHEP::mm)
         * @param planes {{IR, OR, Z},{IR,OR,Z},...}
         */
        RZBuilderPtr AddPlanes(G4double unit, const std::vector<RZPlaneUnitless>& planes);

        /**
         * Flip Solid Configuration.
         * For use on builders with UNBUILT solids ONLY, preserving the name.\n
         *   Useful for conditional configuration. Ex:
         * \code
         *  BuilderRefList builders;;  // array of builders to store intermediates or result.
         *  for (int i=0; i<10; i++) {
         *      auto builder = CreateCylinderBuilder("x"+std::to_string(i) // SET INDEXED NAME
         *                                            ,0,10,20,0)          // use global units
         *                 ->SetMaterial("G4_AIR")->SetMother(other_solid)
         *                 ->SetPhysOffset(i*5,0,0);  // position for an array of cylinders.
         *      if (i>4) {
         *          builder->ReflectZSolidConfig(); // half of them are upside down. NO NAME CHANGE.
         *      }
         *      // make and optionally store the physical volumes
         *      builders.emplace_back(builder->MakePlacement());
         *
         *  }
         * \endcode
         * If you use ReflectZCopy(newname) in the loop you'd need to provide a copy name,
         * making your final solid naming harder (still possible) to set up right.
         * You can skin this cat multiple other ways, but this provides options.
         *
         *
         * @param new_name
         * @return The same builder (allows chaining)
         */
        RZBuilderPtr ReflectZSolidConfig();

        /**
         * Modifies a Solid CONFIGURATION to set all inner diameters (IDs) to 0.\n
         * This does NOT copy the solid and does preserve the name.\n\n
         * Let's say you have a hollow cylinder, but you want a mother volume to put it in.\n
         * \code
         * auto cryostat_bot = cryostat_vac_bot->ForkAndReset("cryostat_vac_bot")
         *                                 ->FillSolidConfig()
         *                                 ->SetMaterial(_vacuum);
         * cryostat_bot->SetMother(cryostat_vac_bot);
         * \endcode
         * Done.
         * @see ReflectZSolidConfig() for a detailed example of a similar method.
         * @param new_name Name for new copy.
        */
        RZBuilderPtr FillSolidConfig();



        /** @}*/

    protected:
        ///The polymorphic Solid constructor
        G4VSolid* SolidConstructor(const G4String &name) override {
            return (this->*MakeSolidFunctionPtr_)(name);
        }


    private:
        explicit RZBuilder(const G4String &name, G4double init_phi_start = 0.,
            G4double init_phi_tot = 360, int init_sides = 4);

        //These will become public as MakeSolid() in derived classes.
        //RZBuilder(const RZBuilder &other, const G4String &new_name);
        G4VSolid* MakePolycone(const G4String &name);

        G4VSolid* MakePolyhedra(const G4String &name);

        //There is no MakeCylinder() because it's a special case of Polycone'

        RZBuilder(const RZBuilder &other);


        G4double sides_{4}; // only for polyhedra
        // private raw copy ctor
        // values and checks...
        G4double phi_start_deg_{0};
        G4double phi_tot_deg{360};

        G4VSolid* (RZBuilder::*MakeSolidFunctionPtr_)(const G4String &name) = nullptr;

        // plane data:
        int num_planes_{0};
        std::vector<G4double> z_;
        std::vector<G4double> IR_;
        std::vector<G4double> OR_;

        RZBuilder() = default;
        RZBuilder(RZBuilder &&) noexcept = default;

        // RZBuilderPtr Clone() const override {
        //     return RZBuilderPtr(new RZBuilder(*this));
        // }
    };
}

// technical notes
//  You cannot have polymorphic clone in C++ with reference return, so we need a pointer return.
//  Then the copies use -> where originals use .  (dot) which is annoying.  So we use Create() factory methods to
//  Create the originals always as pointers too.
//  But then we make ctors private, to simplify the interface.  And might as well use smart pointers while we're at it.
//  Seems this is a common pattern, but that's C++ :P
//
// * return from a base class Clone() returns a base class type and slices derived class members.
//   It's actually possible to have a simple Clone(): Derived(Base) {}   in Derived, especially if derived has no data
//   to reconstruct, but smart pointer return and factories are the "usual" way.
//
//   In C# all of this would be simple.