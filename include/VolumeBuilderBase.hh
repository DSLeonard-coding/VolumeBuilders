#pragma once
#ifndef VOLUMEBUILDER_HH
#define VOLUMEBUILDER_HH
#include <optional>
#include "Linkable.hh"
#include "IVolumeBuilder.hh"
#include "ISolidBuilder.hh"
#include "VolumeBuilderConfigs.hh"
/*

 * VolumeMaker.hh
 *
 * Created on: Jun 19, 2024
 *      Author: D. S. Leonard
 * Implicit or explicit misrepresentation authorship is not protected under any license.
 * See RZBuilder.hh for main documentation.
 *
 *s

 */
//#define SHARED_WRAPPER  DLG4::i_shared_ptr
#include <vector>
#include "i_shared_ptr.hh"
#include <VolumeBuildersTypes.hh>


// #define FIXME(msg) static_assert(false, "FIXME: " msg)

//#include "DetectorConstruction_includes.hh"
#include "PlacementNameRegistry.hh"
#include <G4Material.hh>
#include "G4Colour.hh"  // why?  It's in above.
#include "G4VPhysicalVolume.hh"
#include "G4UnionSolid.hh"
#include <G4ThreeVector.hh>
#include <mutex>
#include <memory>
#include <unordered_map>
#include "AssemblyCore.hh"

//#include "VolumeBuilderCore.hh"

namespace DLG4::VolumeBuilders::_internals_ {

    class VolumeBuilderCore;
    template <typename U>
    class VolumeBuilderBase;

    /** @brief VolumeBuilder: Common functionality for volume builder classes.
    * @details Includes material and attribute setting, and union/subtraction methods.
    * This is Not a user class.
    * Only usable through inheritance to extend class functionality.
    * Return most things by pointer to the derived class
    * So we can chain operations (fluent interface).
    * @headerfile VolumeBuilder.hh
    * @tparam U CRTP paramter for the derived class.
    *
    */
    template <typename U>
    class VolumeBuilderBase: public ENABLE_SHARED_WRAPPER<U>, public ISolidBuilder
                         , public IVolumeBuilder {
        /// Templated type for VolumeBuilder base class.
        /// @see BuilderView for the common interface.
        using DerivedPtr = SharedPtr<U>;

        // limit public(protected) API by using friendship.
        // Much of this can't even be handledby protected.
        // Specifically factories (normal) and the type-erasing pseudo-inheritance (StructureBuilder)
        template <typename>
        friend class VolumeBuilderBase;
        friend class VolumeBuilderCore;
        friend class AssemblyCore;
        template <typename>
        friend class StructureBuilderBase;
        friend class StructureBuilderCore;
        friend FromG4VSolid VB::CreateFromG4VSolid(G4VSolid *solid);
        friend Assembly VB::CreateAssembly(G4String names);
        friend class RZBuilderCore;
        friend class BoxBuilderCore;
        friend class FromG4VSolidCore;

    public:
        /**
         * Gets the (last) placed physical volume.  Constructs it if not built yet.
         * Unlike MakePlacement(), this will NOT auto-trigger consecutive placments from copied builders.
         * But you can call it again after MakePlacement() to get those.
         * @return Raw visitor pointer to physical volume.  (DON'T DELETE IT).
        * @ingroup Products
         */
        [[nodiscard]] G4VPhysicalVolume *GetPlacement() final;

        /**
         * Make a placed physical volume.
         * Retrieve it or make and retrieve with GetPlacement()
         * Fails (throws) if already built.  Make a copy first.
         * @ingroup products
         *
         * @return The builder
         */
        DerivedPtr MakePlacement();


        /**
         * An alias for MakePlacement and ForkForPlacement
         * Used for one or many unaltered placments in sequence,
         * usually with auto naming and/or numbering
         * @return The builder
         * @ingroup products
        * @ingroup PlacementConfigs
        */
        DerivedPtr PlaceAndFork() {
            this->MakePlacement();
            auto clone = this->ForkForPlacement(); // default/preset options
            return clone;
        }

        /** @defgroup Products Geant Volume Products
         * */
        /**
         * Gets a built LogicalVolume, builds default if not built yet.
         *
         * @return Raw visitor pointer to the logical volume (DON'T DELETE IT!)
         * @ingroup Products
         */
        [[nodiscard]] G4LogicalVolume *GetLogicalVolume() final;

        /**
         * You probably want GetSolid() instead!!!
         * This Gets a built Solid, BUT NOT final Boolean.
         * Builds default if not built yet.
         *
         * Even if you aren't doing boolean now, if you add it later,
         * this method will miss it.  GetFinalSolid() is more general.
         * @return Raw visitor pointer to the solid.  (DON'T DELETE IT!)
         * @ingroup Products
         */
        [[nodiscard]] G4VSolid *GetBaseSolid() final;

        /**
         * Gets a complete final built Solid, including configured Boolean operaions if any.
         * Builds default if not built yet.
         * @return Raw visitor pointer to the solid.  (DON'T DELETE IT!)
         * @ingroup Products
         */
        [[nodiscard]] G4VSolid *GetFinalSolid() final;


        /**
         * Basically never needed now.\n
         * Builds the logical volume and returns the builder.
         *     GetLogicalVolume(), or passing the builder to any consumer
         *     will trigger the build anyway.
         *
         * @param material Pointer to material to use.
         * @param name Optional explicit name for logical volume.
         *      Normally already derived from builder/solid name.
         * @return This builder.
         */
        DerivedPtr MakeLogicalVolume(
            G4Material *material = nullptr, G4String name = std::string());

        /**
             * Make the G4VSolid. Returns the builder.
             * Use GetBaseSolid() to retrieve or make and retrieve it.
             * @return The builder
             */
        DerivedPtr MakeSolid();

        /**
         * SetName  Sets name used for solid and derived product names
         * @param name The name
         * @return The builder
         */
        DerivedPtr SetName(const G4String &name);

        /**
         * @defgroup LogicalVolumeConfigs Logical Volume Configuration Methods.
         * These all return the builder for fluent interface.
         */


        /**@defgroup booleans
         * Define combination another volume with present one,
         * @{ */

        /**
         * Define combination another volume with present one,
         * This does NOT immediately trigger a build on a passed-in builder, just pre-sets the relationship.
         * @param other  A VolumeBuilder, G4VSolid, or anything constructable by a BuilderView
         * @param offset  Offset applied to solid
         * @param rotation Applied to solid
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         * @ingroup booleans
         */
        DerivedPtr AddUnion(const VolumeBuilder &other,
            const Unit3Vec &offset = {CLHEP::mm, 0, 0, 0},
            G4RotationMatrix *rotation = nullptr);
        ///@copydoc AddUnion
        DerivedPtr AddSubtraction(const VolumeBuilder &other,
            const Unit3Vec &offset = {CLHEP::mm, 0, 0, 0},
            G4RotationMatrix *rotation = nullptr);
        ///@copydoc AddUnion
        DerivedPtr AddIntersection(const VolumeBuilder &other,
            const Unit3Vec &offset = {CLHEP::mm, 0, 0, 0},
            G4RotationMatrix *rotation = nullptr);
        /** @} */

        /**
         * Add a boolean operation.
         * This does NOT immediately trigger a build on a passed-in builder, just pre-sets the relationship.
         * @param other  A VolumeBuilder, G4VSolid, or anything constructable by a BuilderView
         * @param is_subtraction true for subtraction, false for union
         * @param is_intersection true for intersection, false for union
         * @param offset  Offset applied to solid
         * @param rotation Applied to solid
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         */
        DerivedPtr AddBoolean(
            const VolumeBuilder &other,
            bool is_subtraction = false,
            bool is_intersection = false,
            const Unit3Vec &offset = {CLHEP::mm, 0, 0, 0},
            G4RotationMatrix *rotation = nullptr
            );
        /** @} *

        /**
         *
         * @param material
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         */
        DerivedPtr SetMaterial(G4Material *material);

        /**
         *
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         */
        DerivedPtr SetColor(double r = 0.5, double g = 0.5, double b = 0.5, double alpha = 1.0);

        /**
         *
         * @param color
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         */
        DerivedPtr SetColor(const G4Colour &color);

        /**
         *
         * @param alpha
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         */
        DerivedPtr SetAlpha(G4double alpha);

        /**
         *
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         */
        DerivedPtr ForceSolid(bool x = true); // default if uncalled is false.

        /**
         *
         * @param x
         * @return The builder
         * @ingroup LogicalVolumeConfigs
         */
        DerivedPtr SetVisibility(bool x = true);

        // ... inside the VolumeBuilder class declaration ...

        /**
         * @defgroup PlacementConfigs
         * Placement Configuration Setters
         * These all return the builder for fluent interface.
         */

        /**
         * Directly Provide a Logical Volume
         * Instead of using the builder.
         *
         * @param logical_volume
         * @ingroup PlacementConfigs
         */
        DerivedPtr SetLogicalVolume(G4LogicalVolume *logical_volume);

        /**
         * Pre-set base name for logical and physical volumes.
         * _L and _P will be appended, respectively.
         *
         * If not set, the builder will take it from the builder name.
         * But this is useful for unions where the starting solid name may be more specific.
         * @param name
         * @ingroup PlacementConfigs
         */
        DerivedPtr SetBooleanName(const G4String &name);

        /**
         * Set the rotation matrix for placement.
         * Using Set, rotation applies before translation, regardless of order set, just as in G4PVPlacement()
         * @param rot Pointer to the rotation matrix.
         * @return This builder for chaining.
         * @ingroup PlacementConfigs
         */
        DerivedPtr SetPhysRotation(const G4RotationMatrix &rot);

        /**
         * Like SetPhysRotation but stacks with previous transformations in order applied.
        * Unlike with G4Transform multiplication, stacking is done in a way that allows
         * mutiple objects to move together.  The first transformations can set individual
         * objects, and a uniform transformation can be applied to them all to create a
         * rigid motion of the collection.
         *
         * @param stacked_rot
         * @return The builder
         */
        DerivedPtr StackPhysRotation(const G4RotationMatrix &stacked_rot);

        /**
         * Set the translation vector for placement.
         * Using Set, rotation applies before translation, regardless of order set, just as in G4PVPlacement()
         * You can pass (unit, x, y, z) or use the default unit with (x, y, z).
         *
         * Examples:
         *   SetPhysOffset({CLHEP::cm, 1, 2, 3})     // 1cm, 2cm, 3cm
         *   SetPhysOffset({1, 2, 3})                // Uses default default unit (usually mm)
         *   SetDefaultUnit(CLHEP::cm); SetPhysOffset({1, 2, 3})  // 1cm, 2cm, 3cm
         *   SetDefaultUnit(1); SetPhysOffset({10*CLHEP::mm, 20*CLHEP::mm, 30*CLHEP::mm})  // Values already have units
         *
         * @param offset Translation vector.  See method description for details.
         * @return This builder for chaining.
         * @ingroup PlacementConfigs

         */
        DerivedPtr SetPhysOffset(const Unit3Vec &offset = {CLHEP::mm, 0, 0, 0});
        /**
         * Like SetPhysOffset but stacks with previous transformations in order applied.
         * Unlike with G4Transform multiplication, stacking is done in a way that allows
         * mutiple objects to move together.  The first transformations can set individual
         * objects, and a uniform transformation can be applied to them all to create a
         * rigid motion of the collection.
         *
         * @param offset
         * @return The builder
         */
        DerivedPtr StackPhysOffset(const Unit3Vec &offset = {CLHEP::mm, 0, 0, 0});

        /**
         * Set the G4Transform3D for placment.
         * The tranformation provided is meant to be in UNITLESS values.
         * Ie the unit is already pre-configured but not included in the values.
         * So passing 5 means 5cm if SetDefault(CLHEP::cm) was set.
         * Otherwise you SetDefaultUnit(1), and pass a value that was already multipled by units.
         * To supply different units for each G4Tranform3D separately,
         * just call SetDefaultUnit(unit) before each call.
         * The call will overwite/replace rotation and translation.
         * @param new_transform Translation vector.
         * @return This builder for chaining.
         * @ingroup PlacementConfigs

         */
        DerivedPtr SetPhysTransform(const UnitlessG4Transform3D &new_transform);
        /**
         * Like SetPhysTransform but stacks with previous transformations in order applied.
         *
         * @param new_transform
         * @return The builder
         */
        DerivedPtr StackPhysTransform(const UnitlessG4Transform3D &new_transform);

        /**
         * A helper to get the Physical Volume transform with units applied
         * This is built from rotation and translatin if they were provided,
         * or from a transform if it was provided, with units applied as specified.
         * Basically allows you to get the unit-multiplied version for use in direct
         * Geant calls.
         * @ingroup PlacementConfigs
         *
         * @return The Physical volume G4Transform3D with units applied.
         */
        G4Transform3D GetPhysTransform() const final {
            return {placement_configs_->rotation, placement_configs_->translation};
        }


        /**
         * Set the placement name for the volume. \n
         * @param pName Name for the placement.
         * @return This builder for chaining.
         * @ingroup PlacementConfigs
         */
        DerivedPtr OverridePlacementName(const G4String &pName);


        /**
         * Set the copy number for the placement. \n
         * @param pCopyNo Copy number.
         * @return This builder for chaining.
         * @ingroup PlacementConfigs
         */
        DerivedPtr SetCopyNo(G4int pCopyNo);

        /**
         * Set the surface check flag for the placement. \n
         * @param pSurfChk Boolean flag for surface check.
         * @return This builder for chaining.
         * @ingroup PlacementConfigs
         */
        DerivedPtr SetSurfaceCheck(G4bool pSurfChk);


        /**
         * Set the mother volume with a builder, or G4VPhysicalVolume
         * or G4VLogicalVolume through implicit conversion. \n
         * This does NOT immediately force a build the mother builder, just pre-sets the relationship.
         *
         *
         * @param mother BuilderVew parameter accepts
         *           a builder or a G4VSolid through implicit ctor
         * @return This builder for chaining.
         * @ingroup PlacementConfigs
         */
        DerivedPtr SetMother(const VolumeBuilder &mother);

        /**
         * Enable auto Physical Volume naming. \n
         * Adds a copy number to every name.
         * By default copy numbers auto increment. \n
         * This will turn that off.  Both makes no sense. \n
         * Or you can set explicit names and numbers in ClonePlacement.
         * @ingroup PlacementConfigs
         *
         * @param set False before setting.
         * @return
         */
        DerivedPtr SetAutoPlacementNaming(bool set);


        /**
         * Enable (disable for false) auto Physical Volume numbering, on by default. \n
         * Adds a copy number to every name.
         * By default copy numbers auto increment.
         * Or you can set explicit names and numbers in ClonePlacement.
         * @ingroup PlacementConfigs
         *
         * @param set True before setting.
         * @return
         */
        DerivedPtr SetAutoCopyNo(bool set);

        /**
         * @defgroup Forks Builder Copiers
         * Copy Builders from various build steps
         *
         * These allow you to base placement builders on previously constructed solids etc.
         */


        /** @defgroup Transforms  Shape transformers
         * Modify shape configurations.
         * */

        /**
         * Configure final solid to be flipped in z.
         * @ingroup Transforms
        */
        DerivedPtr ReflectZFinalSolid();

        /**
         * Configure base solid to be flipped BEFORE applying booleans.
         *
         * @see ReflectZFinalSolid() to flip entire solid with or without booleans.
         * @ingroup Transforms
         *
         * @return The same builder (allows chaining)
         */
        DerivedPtr ReflectZBaseSolid();

        /**
         * @brief Copy a configured/unbuilt Builder, with a new name.
         * This is the most generic all-purpose copier,
         * but can result in some duplication of constituent solids, etc.
         *
         * Copies all configurations. You can modify and rebuild all products.
         * Copying-Transformations are not remembered.
         * Base implementation is in VolumeBuilder, but can be overridden
         * for Solid-Builder specific needs.
         *
         * @param new_name
         * @ingroup Forks
         * @return
         */
        DerivedPtr ForkAndReset(const G4String &new_name) const;

        /**
         * Calls MakeSolid()  and copies builder with products cleared for further construction.
         * Configurations from ALL stages (including later) are kept.
         *
         * @param new_name The base name for the new builder's Geant products.
         * @return A shared pointer to the new builder instance.
         * @ingroup Forks
         *
         */
        DerivedPtr ForkForFinalSolid(const G4String &new_name);


        /**
         * Makes final solid (including booleans) if unbuilt and copies builder with products
         * cleared for further construction.
         * Configurations from ALL stages (including later) are kept.
         *
         * This one seems pretty specific, but could be useful
         * for extending a boolean build more easily possibly
         *
         * @param new_name The base name for the new builder's logical and physical volumes.
         * @return A shared pointer to the new builder instance.
         * @ingroup Forks
         */
        DerivedPtr ForkForLogicalVolume(const G4String &new_name);


        /**
         * Calls MakeLogicalVolume() and creates a new builder instance with a
         * copy of all placement-related configs.\n
         * This will be called automatically by consecutive calls to MakePlacement().
         * All physical volume and (future?) derived products are reset
         * to be rebuilt.  Copy_no and placement name can be auto incremented (number only by default)
         * or overridden.
         *
         * The copy_no will be incremented.
         * @param copy_no  no arguments or std::nullopt to leave as auto. Must set on every use to get manual numbering.
         * @param name_override An optional name override for the physical volume.  Leave blank to use auto controls instead.
         * @return A shared pointer to the new builder instance.
         * @ingroup Forks
         */
        DerivedPtr ForkForPlacement(
            std::optional<int> copy_no = std::nullopt
            , const G4String &name_override = ""
            , bool parent_name_was_set = false
            // in assembly/hierarchies, name change derives through parent
            );

        /**
         * @defgroup CopyConfigs Configuration Copiers
         * Copy configurations from other builders
         */

        /**
         * This may help if reusing placement configs for a different shape.
         * Copies the placement configuration (rotation, translation, mother, etc.)
         * from another builder.  Does NOT copy any naming, as that would conflict.
         * Also does NOT copy any products.  Use CopyPlacement Builder for that.
         * Names will derive from present object, or you can override with Setters as usual.
         *
         * @param other The builder instance to copy placement configuration from.
         * @return This builder for chaining.
         * @ingroup CopyConfigs
         */
        DerivedPtr CopyPlacementConfigsFrom(const VolumeBuilder &other);

        /**
         * Copies the LogicalVolume configuration (material, VisAttributes, etc.)
         * from another builder.
         * This DOES NOT include copying any booleans.
         * This is useful for setting up multiple placements with similar configurations.
         * @param other The builder instance to copy placement configuration from.
         * @return This builder for chaining.
         * @ingroup CopyConfigs
         */
        DerivedPtr CopyVolumeConfigsFrom(const VolumeBuilder &other);

        /**
         * @defgroup Units
         * @brief Methods to set units used in geometry methods.
         * @details Methods are available for both VolumeBuilder and StructureBuilder (ie builders and assemblies).
         * */

        /**
         * @brief Set the per-Builder default unit for all later non-factory offsets.
         * Use DLG4::VolumeBuilders::SetGlobalDefaultUnit() to set a default for all builders,
         * or fall back to the initial default of CLHEP::mm. \n
         * - May not apply to values set before this is called. \n
         * - Does not apply to factory units like CreateCenteredBoxBuilder();\n
         * But should not be set and changed. Just use it once, early. \n
         * @param unit The unit to set, ex: CLHEP::mm
         * @return The builder (allows chaining)
         * @ingroup Units
         */
        DerivedPtr SetDefaultUnit(G4double unit);

        /**
         * Get the builder default unit or global if not set.
         * @return The active default unit.
         * @ingroup Units
         */
        G4double GetEffectiveDefaultUnit() const;
        /** @defgroup Batch collection
         *  @brief methods to collect builders for batch operation.
        /**
         * @brief Add the builder a list for group/batch manipulations
         *
         * @param list The list to add to.
         * @return the builder
         * @ingroup Batch collection
         */
        DerivedPtr AddTo(BuilderViewList &list) const;

        /**
         * @brief Add the builder a list for group/batch manipulations
         *
         * @param list The list to add to.
         * @return the builder
         * @ingroup Batch collection
         */
        DerivedPtr AddTo(StructureViewList &list) const;

        /**
         * @brief Add the builder an assembly geometrically joined manipulation.
         *
         * @param assembly The assembly to add to.
         * @return the builder
         * @ingroup Batch collection
         */
        DerivedPtr AddTo(Assembly &assembly) const;

        /**
         * Destructor
         * Normally does not delete volume objects.  Geant takes care of that.
         */
        ~VolumeBuilderBase() override;


        G4String GetBuilderName() const;

    protected:
        /// polymorphic access to solid construction
        G4VSolid *SolidConstructor(const G4String &name) override = 0;

        // base implementation of non-fluent clone.
        SharedPtr<IStructureBuilder> clone_impl() const override;

    private:
        //Methods with basically protected intent
        // But for now, keep them private and require explicit friend access
        // ctors only useable through explicitly granted (friendship) inheritance...

        VolumeBuilder ToVolumeBuilder() const override;
        StructureBuilder ToStructureView() const override;

        VolumeBuilderBase();
        DerivedPtr SetSolid(G4VSolid *solid);

        DerivedPtr Clone() const {
            // This call is NOT DIRECTLY polymorphic.  It calls the VolumeBuilder base clone_impl().
            // Internally THAT calls polymorphic clone to create the structure_ptr and recreate the view.
            // We have to re-cast the result.
            i_shared_ptr<IStructureBuilder> base_cloned_ptr = this->clone_impl();
            return std::static_pointer_cast<U>(base_cloned_ptr);
        }


        template <typename T, typename std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>,
            int>  = 0>
        VolumeBuilderBase(const SharedPtr<T> &other, SET_LINK_TYPE);

        VolumeBuilderBase &operator=(const VolumeBuilderBase &other) = delete;
        DerivedPtr MakeFinalSolid(G4String boolean_name = "");
        VolumeBuilderBase(const VolumeBuilderBase &other);

        VolumeBuilderBase(VolumeBuilderBase &&) noexcept = default;

        G4VSolid *GetSolidPtr() const {
            return this->solid_ptr_.get_mutable();
        };

        //Configs (re-linked by copy-ctors)
        ONCE_MUTABLE Linkable<BuilderConfigs> builder_configs_{BuilderConfigs{}};
        ONCE_MUTABLE Linkable<BooleanConfigs> boolean_configs_{BooleanConfigs{}};
        ONCE_MUTABLE Linkable<VolumeConfigs> lv_configs_{VolumeConfigs{}};
        ONCE_MUTABLE Linkable<PlacementConfigs> placement_configs_{PlacementConfigs{}};

        //Products (sometimes copied by copy methods)
        ONCE_MUTABLE Linkable<G4VSolid> solid_ptr_{}; // child can and should create the solid
        ONCE_MUTABLE Linkable<G4VSolid> final_solid_ptr_{};
        ONCE_MUTABLE Linkable<G4LogicalVolume> logicvol_ptr_{};
        ONCE_MUTABLE Linkable<G4VPhysicalVolume> placement_{};


        /** take ownership of the pointed-to erased isolid_ptr
         * One time only!
         */
        void StoreIStructurePtr(const IStructurePtr &istructure_ptr);

        void StoreBuilderView(const VolumeBuilder &builder_view);


        //Unsaved data, left out of configs to be reset on all copy operations.
        bool explicit_copyno_set_ = false;
        bool explicit_physical_copy_name_set_ = false;
        G4String placement_name_override_ = ""; // Will be derived from boolean_name_ by default

        void SetSolid_impl(G4VSolid *solid);
        void SetLogicalVolume_impl(G4LogicalVolume *logical_volume);

        //Some hard baked settings
        // let's pretend like we'd ever care about resource management:
        static constexpr bool has_ownership_{false}; // for future resource control
        static constexpr bool enable_full_lazy_builds{true};
        // Allows full chain lazy building.  Off for now.

        //Methods with truly private intent.

        // Propagate transforms
        void PropagateTransform();


        void ApplyAttributes_();
        G4String GetPlacementBaseName() const;

        [[nodiscard]] G4String GetLogicVolName() const; // should this be pulic?

        // these validators may trigger lazy build of buildable products needed for the build:
        void ValidateForPVBuild(std::string const &site = std::string());
        void ValidateForVolumeBuild(std::string const &site = std::string());
        void ValidateForBooleanBuild(std::string const &site = std::string());
        // These helper validators just check that the product isn't already made:
        void ValidateSolidNotBuilt(const std::string &operation) const;
        void ValidateBooleanNotBuilt(const std::string &operation) const;
        void ValidateLogicalNotBuilt(const std::string &operation) const;
        void ValidatePlacementNotBuilt(const std::string &operation) const;

        // For Geant, we need things left persistent, but this is a poor way even for this hack:
        static void make_persistent(const std::shared_ptr<void> &obj);

        G4ThreeVector ProvisionUnits(const Unit3Vec &vec) const;

        // Private static helper
        // method(s) used by the instance methods

        static void NoNameCheck(const std::string &name, const std::string &site) {
            if (name.empty()) {
                throw std::runtime_error("Error in " + site + " ,"
                                         "for builder named: " + name + "\n"
                                         " Must provide a new name");
            }
        }
    };
} // namespace VolumeBuilder


//The implementation:
#include "VolumeBuilderBase.hpp"
#endif

/* TODO
 * Need more derived classes for boxes and stuff!
 * [] Propagate validators into all config methods.
 * []  Validators needed in RZBuilder still.
 * [] review make_persistent vs no-op deleters, and add upfront protection.  make_persistent may need templating.
 * []   use self-owned shared_ptr trick (intentional cycle) instead of make_peristent.
 * [] A fill and copy would great.
 * [] Structure builder lacks recursive clone of placement_configs_.children

FromG4VSolid (rename?-> SolidAdapterBuilder)
Bonus
*
* [] consider adding overlap check option to command line for easier use.
* */
