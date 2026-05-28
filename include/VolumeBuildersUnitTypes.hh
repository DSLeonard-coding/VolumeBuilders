// VolumeBuilderUnitTypes.hh
// A bridge to bring DLG4Units.hh into VolumeBuilders without a header loop
// Created by D.S. Leonard on 5/11/26.
//

#ifndef HPGESIM_VOLUMEBUIDLERSUNITTYPES_HH
#define HPGESIM_VOLUMEBUIDLERSUNITTYPES_HH
#include "DLG4Units.hh"

namespace DLG4::VolumeBuilders {
    /// @copydoc DLG4::Units::SetGlobalDefaultUnit
    using DLG4::Units::SetGlobalDefaultUnit;
    /// @copydoc DLG4::Units::Length3Vec
    using Length3Vec = DLG4::Units::Length3Vec;
    /// @copydoc DLG4::Units::Unit
    template <class T,class NativeType>
    using Unit = DLG4::Units::UnitBase<T,NativeType>;
    /// @copydoc DLG4::Units::Length
    using Length = DLG4::Units::Length;
    /// @copydoc DLG4::Units::Volume
    using Volume = DLG4::Units::Volume;
    /// @copydoc DLG4::Units::Density
    using Density = DLG4::Units::Density;


        /**
     * @brief A 3D vector that manages unit policy for parameter passing
     * If constructed with a unit, that's it.
     * If not the user has implicitly requested use of the receiver's units.
     * The receiver is expected to call apply_unit(default) to get the reconciled result.
     * @details Stores unit, x, y, z as separate values.
     */

    class ThreeVecDimensioner : public DLG4::Units::Length3Vec {
    public:
        double x, y, z;
        ThreeVecDimensioner() = default;

        ThreeVecDimensioner(const ThreeVecDimensioner &) = delete;

        ThreeVecDimensioner &operator=(const ThreeVecDimensioner &) = delete;

        ThreeVecDimensioner(Length3Vec v)
            : Length3Vec(v) { Sync(); }

        ThreeVecDimensioner(G4ThreeVector v, const Length &u)
            : Length3Vec(v, u) { Sync(); }

        ThreeVecDimensioner(double x, double y, double z, const Length &u)
            : Length3Vec(x, y, z, u) { Sync(); }

        ThreeVecDimensioner(double x, double y, double z): Length3Vec(x,y,z,DLG4::Units::Length::native) {
            is_dimensioned_ = false;
            Sync();
        }

        ThreeVecDimensioner(G4ThreeVector v)
            : ThreeVecDimensioner(v.x(), v.y(), v.z()) {
        }

        [[nodiscard]] G4ThreeVector apply_units(const Length dflt_unit) const {
            auto vec = G4ThreeVector(x, y, z);
            if (!is_dimensioned_) {
                vec *= dflt_unit.Native;
            }
            return vec;
        }

    private:
        void Sync() {
            x = NativeValue_->x();
            y = NativeValue_->y();
            z = NativeValue_->z();
        }

        bool is_dimensioned_ = true;
    };


}



#endif //HPGESIM_VOLUMEBUIDLERSUNITTYPES_HH
