#pragma once
#ifndef STRUCTUREBUILDER_HH
#define STRUCTUREBUILDER_HH
#include <optional>
#include "Linkable.hh"
#include "ISolidBuilder.hh"
#include "VolumeBuilderConfigs.hh"
#include <G4Transform3D.hh>

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
#include "VolumeBuilderTypes.hh"


// #define FIXME(msg) static_assert(false, "FIXME: " msg)

//#include "DetectorConstruction_includes.hh"
#include "G4Colour.hh"
#include "G4VPhysicalVolume.hh"
#include <G4ThreeVector.hh>
#include <mutex>
#include <memory>
#include <stdexcept>
//#include "StructureBuilder.hh"

    namespace DLG4::VolumeBuilders {



        /**
         *     *@class StructureBuilder
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
        template <typename U>
        class StructureBuilder: public ENABLE_SHARED_WRAPPER<U>, public IStructureBuilder {
            using DerivedPtr = SharedPtr<U>;

            // we need access to the other templates for copy
            template <typename>
            friend class VolumeBuilder;
            friend class StructureBuilderReference;
            friend class Assembly;
            friend AssemblyPtr CreateAssembly(G4String name);
            template <typename>
            friend class StructureBuilder;
            //allow i_shared_ptr access to our converting ctors
            //This way they get instantly wrapped!

            friend class i_shared_ptr<StructureBuilder<U>>;

        public:

            //Methods specific to structures/assemblies....




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
                , const G4String &name_override = "") ;

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
            DerivedPtr ForkLogicalVolume(const G4String &new_name);


            /**
             * An alias for MakePlacement and ClonePlacement
             * Used for one or many unaltered placments in sequence,
             * usually with auto naming and/or numbering
             * @return The builder
             * @ingroup products
            * @ingroup PlacementConfigs
            */
            DerivedPtr PlaceAndFork() ;



            /**
             * SetName  Sets name used for solid and derived product names
             * @param name The name
             * @return The builder
             */
            DerivedPtr SetName(const G4String &name);

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
             * @param is_visible
             * @return The builder
             * @ingroup LogicalVolumeConfigs
             */
            DerivedPtr SetVisibility(bool is_visible = true);



            // ... inside the StructureBuilder class declaration ...

            /**
             * @defgroup PlacementConfigs
             * Placement Configuration Setters
             * These all return the builder for fluent interface.
             */


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
             * Note: Structures/Assebnlies stack transformations hierarchichicaly automatically.\n
             * This method is for manual
             * hierarchical stacking calculations.  It's probably redundant on assemblies in 99% of caases \n
             *
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
             * Note: Structures/Assebnlies stack transformations hierarchichicaly automatically.\n
             * This method is for manual
             * hierarchical stacking calculations.  It's probably redundant on assemblies in 99% of caases \n
             *
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
             * Set the G4Transform3D for placment
             * The tranformation provided is meant to be UNITLESS
             * unless you SetDefaultUnit(1).  Otherwise ex: SetDefaultUnit(CLHEP::cm)
             * and proivde a UNITLESS transform.
             * To supply units for each G4Tranform3D separately,
             * just call SetDefaultUnit(unit) before each call.
             * Will overwite/replace rotation and translation.
             * @param new_transform Translation vector.
             * @return This builder for chaining.
             * @ingroup PlacementConfigs

             */
            DerivedPtr SetPhysTransform(const UnitlessG4Transform3D &new_transform);
            /**
             * Like SetPhysTransform but stacks with previous transformations in order applied.
             * Note: Structures/Assebnlies stack transformations hierarchichicaly automatically.\n
             * This method is for manual
             * hierarchical stacking calculations.  It's probably redundant on assemblies in 99% of caases \n
             *
             * @param stacked_transform
             * @return The builder
             */
            DerivedPtr StackPhysTransform(const UnitlessG4Transform3D &stacked_transform);

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
            [[nodiscard]] G4Transform3D GetPhysTransform() const {
                return G4Transform3D(placement_configs_->rotation, placement_configs_->translation);
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
             * @param mother olumeReferencePtrparameter accepts
             *           a builder or a G4VSolid through implicit ctor
             * @return This builder for chaining.
             * @ingroup PlacementConfigs
             */
            DerivedPtr SetMother(const BuilderView &mother);

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
             */






            /**
             * @defgroup CopyConfigs Configuration Copiers
             * Copy configurations from other builders
             */

            /**
             * This may help if reusing placement configs for a different structure.
             * Copies the placement configuration (rotation, translation, mother, etc.)
             * from another structure.  Does NOT copy any naming, as that would conflict.
             * Also does NOT copy any products.
             *
             * @param other The builder instance to copy placement configuration from.
             * @return This builder for chaining.
             * @ingroup CopyConfigs
             */
            DerivedPtr CopyPlacementConfigsFrom(const BuilderView &other);


            /**
               * @brief Set the per-Structure default unit for all later non-factory offsets.
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
             * Get the structure default unit or global if not set.
             * @return The active default unit.
             */
            G4double GetEffectiveDefaultUnit() const;


            /**
             * Destructor
             * Normally does not delete volume objects.  Geant takes care of that.
             */
            ~StructureBuilder() override;

            G4String GetBuilderName() const;

        protected:
            /// polymorphic access to solid construction
            virtual G4VSolid* SolidConstructor(const G4String &name) override = 0;

        private:
            //Methods with basically protected intent
            // But for now, keep them private and require explicit friend access
            // ctors only useable through explicitly granted (friendship) inheritance...

            friend class RZBuilder;
            friend class StructureBuilderReference;
            friend class FromG4VSolid;


            StructureBuilder();

            DerivedPtr Clone() const {
                i_shared_ptr<IStructureBuilder> base_cloned_ptr = this->clone_impl();
                // really is a static cast in principle, but just to catch programming bugs:
                auto retval = i_dynamic_pointer_cast<U>(base_cloned_ptr);
                return retval;
            }

            SharedPtr<IStructureBuilder> clone_impl() const ;

            StructureView ToStructureView() const override ;

            BuilderView ToBuilderView() const override ;



            template <typename T, typename std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>, int> = 0>
            StructureBuilder(const SharedPtr<T> &other, SET_LINK_TYPE );


            StructureBuilder &operator=(const StructureBuilder &other) = delete;
            DerivedPtr MakeFinalSolid(G4String boolean_name = "");
            StructureBuilder(const StructureBuilder &other);

            StructureBuilder(StructureBuilder &&) noexcept = default;

            G4VSolid *GetSolidPtr() const {
                return this->solid_ptr_.get_mutable();
            };

            //Configs (re-linked by copy-ctors)
            ONCE_MUTABLE Linkable<BuilderConfigs> builder_configs_ {BuilderConfigs{}};
            ONCE_MUTABLE Linkable<BooleanConfigs> boolean_configs_ {BooleanConfigs{}};
            ONCE_MUTABLE Linkable<VolumeConfigs> lv_configs_ {VolumeConfigs{}};
            ONCE_MUTABLE Linkable<PlacementConfigs> placement_configs_ {PlacementConfigs{}};

            //Products (sometimes copied by copy methods)
            ONCE_MUTABLE Linkable<G4VSolid> solid_ptr_{}; // child can and should create the solid
            ONCE_MUTABLE Linkable<G4VSolid> final_solid_ptr_{};
            ONCE_MUTABLE Linkable<G4LogicalVolume> logicvol_ptr_{};
            ONCE_MUTABLE Linkable<G4VPhysicalVolume> placement_{};
            bool was_placed_ = false; // set when assembly is placed.

            // Interface pointer for type erased class access (Copied in linking ctor only)


        private:
            //Unsaved data, left out of configs to be reset on all copy operations.
            bool explicit_copyno_set_             = false;
            bool explicit_physical_copy_name_set_ = false;
            G4String placement_name_override_ =   "";      // Will be derived from boolean_name_ by default


            //Some hard baked settings
            // let's pretend like we'd ever care about resource management:
            static constexpr bool has_ownership_{false}; // for future resource control
            static constexpr bool enable_full_lazy_builds{true};
            // Allows full chain lazy building.  Off for now.

            //Methods with truly private intent.

            [[nodiscard]] G4String GetLogicVolName() const; // should this be pulic?

            void ValidateForPVBuild(std::string const &site = std::string());
            void ValidatePlacementNotBuilt(const std::string &operation) const;


// Nothing below here but  implementation for some private static helper
// methods used by the instance methods

            static void NoNameCheck(std::string name, std::string site){
                if (name.empty()) {
                    throw std::runtime_error("Error in "+site+ " ,"
                                             "for builder named: " + name + "\n"
                                             " Must provide a new name");
                }
            }

        };


    } // namespace StructureBuilder


//#include "StructureBuilder.hpp"

//The implementation:
#endif

