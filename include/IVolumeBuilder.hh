/**
* @file
* Created by @author Douglas S. Leonard on @date 6/19/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

#ifndef IVOLUMEBUILDER_HH
#define IVOLUMEBUILDER_HH
#include <G4LogicalVolume.hh>
#include <G4Transform3D.hh>
#include <G4VSolid.hh>

namespace DLG4::VolumeBuilders::_internals_ {
    /// @brief Might make this public.
    /// Generally you should prefer use of VolumeReferencePtr
    /// as it provides full type erasure for all VolumeBuilder methods.
    /// This Interface MAY DISAPPEAR as VolumeReferencePtr becomes more established.
    ///
    /// But this is a lightweight polymorphic class to easily allow access to
    /// the base-CRTP class non-fluent (non-templated) getters.
    /// This gets used for now at least by i_shared_ptr to create conversions from VolumeBuilder classes.
    class IVolumeBuilder {
    public:
        virtual G4VSolid *GetBaseSolid() = 0;
        /// @ingroup products
        virtual G4VSolid *GetFinalSolid() = 0;
        /// @ingroup products
        virtual G4LogicalVolume *GetLogicalVolume() = 0;
        /// @ingroup products
        virtual G4VPhysicalVolume *GetPlacement() = 0;
        /// @ingroup products
        [[nodiscard]] virtual G4Transform3D GetPhysTransform() const = 0;

        virtual ~IVolumeBuilder() = default;
    };
}
#endif //IVOLUMEBUILDER_HH
