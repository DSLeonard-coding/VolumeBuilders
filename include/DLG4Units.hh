// DLG4Units.hh
// Created by D.S. Leonard on 5/10/26.
//
#ifndef DLG4MODUSIM_DLG4UNITS_HH
#define DLG4MODUSIM_DLG4UNITS_HH
#include "G4Types.hh"
#include "G4ExceptionSeverity.hh" // For FatalException, JustWarning, etc.
#include "globals.hh"             // For G4Exception and G4double
#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ThreeVector.hh>
#include <optional>
//#include <VolumeBuilderConfigs.hh>


/**
 * @file DLG4Units.hh Typesafe units, for VolumeBuilders, Geant, ETC !! No accidental mixing values and units.
     These are type-safe, non-implicitly convertible units/measures.

     Motivation:
    In geant you cannot distinguish between a length and a unitless value.
    You can multiply mm * mm and treat that as a length. You can have the Native number 12 and
    treat that as a length.  At some low level the computer only knows numbers
    but we can do better at the API and avoid these bugs.
     For positive values, a Unit Length is the same a Length.
     And in VB  a Length is in fact(derives from) a Unit<Length>  !!
     Unitless doubles are things that multiply lengths to describe
     other lengths in reference to that Unit<Length>.   The result is also a Unit<Lenght>.

     ## Usage:
     ### Setting a Dimensioned length:
     And this is how VB works... You can only consruct a Length from an existing Length and a double
     All of these are equivalent:
     ```cpp
     #include <DLG4Units.hh>
     using DLG4::Units;
     Length x = 5.0 * Length::mm;    // No different from Geant syntax! but we used a typed unit (not double) !!
     Length x = 5.0 * Unit<Length>::mm;  //  Same thing.  Length is a Unit<Length>
     Length x = Length( 5.0, Length::mm );  // constructor version.
     ```
     ### -------------THE SAFETY  NET -------------------
     ```cpp
     Length x = 5.0 * Length::mm;    // No different from Geant syntax! but we used a typed unit (not double) !!
     Length y = (5.0   +  x ) * Length::mm;     // ******* THIS WON'T COMPILE!!!  x is ALREADY A LENGHT!!!!********
     Length y = x * Length::m;   //   Even this won't.  Even if we later support this multiplication, it wouldn't assign to a Length.
     ```

     ### Setting with Native/legacy/system values:
     The one escape hatch is you can explicitly define a Length in system units in any of these
     explicit ways, ordered by preference:
     ```cpp
     Length x.Native = some_geant_double;
     Length x = some_geant_double * Length::native;
     Length x = Length::FromNative(some_geant_double);
     Length x = Length( some_geant_double, Length::native );
     ```
     Where one_geant_double may have passed from other code as 5.0 * CLHEP::mm for instance,
     but it's still a double, not a DLG4::Units::Length until you make it one.
     So simply assinging to x.Native allows interfacing with legacy code. You almost CANNOT mess this up.
     If you try to assign legacy values (doubles) to x directly it will fail.

     ### Retrieving Values in designated units:
     ```cpp
     x = 5.0 * Length::mm;
     G4double y= x.InUnits(length::cm);
     ```
     y is now 0.5
     ```cpp
     SetGlobalDefaultUnit(Length::mm);
     x = 5.0 * Length::mm;
     G4double y= x.InUnits(length::cm);
     G4double z= x.InDefaultUnits();
     ```
     y is no 0.5 and z is 5.0

     ### Retrieving/Passing Values as/to system units:
     for the same x as above:
     ```cpp
    G4Box* box = new G4Box("MyBox", x.Native(), x.Native(), x.Native());
     ```
     That's it, and again  you almost cannot mess this up. If you try to pass x, it will fail.
     You'd have to intentionally use x.InUnits(...) to mess it up.


     ### Type safety:
     In VolumeBuilders you cannot pass a Length to a double parameter or a double to a Length parameter
     and a Units::Length::mm * Units::Lentgh::mm  is not a length (or presently even valid)
     And so you cannot accidentally multiply by the unit twice, OR forget to multiply and still
     assign to your target Length.  Ex:      10*length_obj  is assignable to a length, but length_obj*lenght_obj is not.
     moss/volume is presently assignable to a density.

     ### Avoiding CLHEP units
    The .Native interface is necessary for interface withing shared variables and Geant calls,
    but can also be abused, causing loss of type safety
    ```
     Length x.Native =  5.0 *  CLHEP::mm );  // this IS still valid and correct, but poor form now.
     //  *******THIS COMPILES JUST FINE  BUT IS WRONG**********
     Length y.Native  = (5.0   +  x.Native ) * CLHEP::mm;  // And this is why using CLHEP will lead to bugs.
     ```
     So just don't.   Type .Native only when needed, and never use CLHEP units.

    ###  Vectors
    The Unit3Vec (typdefed Length3Vec) behaves almost exactly the same.  It provides standard G4ThreeVector through .Native
    , including calls like .Native().x()   and it is typed and can be multiplied with Length objects.
    It has a few extra convenience constructors that are self explanitory.

    #Usage in VolumeBuilders
    DLG4::VolumeBuilders takes units and values natively and distinguishes them, so no .Native use
    is needed except for direct geant calls.
    If you're working with raw numbers and default units of mm, you will just
    need to translate shared (context/global) values with .InDefaultUnits() after setting your the default for your local method.
    Shared variables can be of type Length and used in either VB or traditional code.
    */

/**
 * Typesafe units!! No accidental mixing values and units.
 */

// TODO, doxygen all this.
//  TODO document usage like this
// if (!(context->GetExtraOpt("z_gap_mm") >> context->z_gap.InUnits(VB::Length::mm))) {

namespace VB = DLG4::VolumeBuilders;
namespace DLG4::Units {
    template<typename T>
    class Unit;
    class Length;
    class Mass;
    class Volume;
    class Density;
    class Unit3Vec;
    /// @copydoc DLG4::Units::Unit3Vec
    using Length3Vec = Unit3Vec;
    class ThreeVecDimensioner;


    template <typename T>
    inline G4double global_default_unit = 1.0;

    // Set the default value for the global default unit.
    //  (The default for the default for the default).
    template <> inline G4double global_default_unit<Units::Length> = CLHEP::mm;
    template <> inline G4double global_default_unit<Units::Mass> = CLHEP::g;
    template <> inline G4double global_default_unit<Units::Volume> = CLHEP::mL;

    /// Tag type for inheritance
    struct UnitTag {
    };

    /**
     * CRTP base methods for Unit classes.
     * @tparam T
     */
    template<class T>
    class Unit : public UnitTag {
        using Derived = T;
        // --- THE ACCESS PORT (The Translator) ---
    public:
        struct PropertySetter {
            Unit<T>& parent;
            G4double scale;

            // Still non-copyable for safety
            PropertySetter(const PropertySetter&) = delete;
            PropertySetter& operator=(const PropertySetter&) = delete;
        
            // This is the "magic" for z_gap.Native = 10
            void operator=(G4double val) { parent.NativeValue_ = val * scale; }

            // Extraction and implicit conversion
            G4double operator()() const { return parent.GetScaled(1.0 / scale); }
            operator G4double() const { return (*this)(); }

            // Stream support
            friend std::istream& operator>>(std::istream& is, const PropertySetter& ap) {
                G4double val;
                if (is >> val) ap.parent.NativeValue_ = val * ap.scale;
                return is;
            }
        };

        // --- THE PROPERTY OBJECTS ---
        PropertySetter Native;
        PropertySetter InUnits(const Unit<T>& u) { return {*this, u.NativeValue()}; }
        PropertySetter InDefaultUnits() { return {*this, GetGlobalDefault().NativeValue()}; }

        // Const factory: Returns a raw double (Read-Only)
        G4double InUnits(const Unit<T>& u) const { return GetScaled(1.0 / u.NativeValue()); }
        G4double InDefaultUnits() const { return InUnits(GetGlobalDefault()); }

    protected:
        // Required for internal static unit definitions (mm, cm, etc)
        [[nodiscard]] G4double NativeValue() const { return GetScaled(1.0); }
        explicit operator G4double() const { return GetScaled(1.0); }

    public:
        static inline T native{1.0};
        static T FromNative(G4double);

        virtual ~Unit() = default;

        Unit(const Unit &other)
            : Native{*this, 1.0}, NativeValue_(other.NativeValue_) {
        }

        Unit &operator=(const Unit &other) {
            if (this != &other) {
                this->NativeValue_ = other.NativeValue_;
            }
            return *this;
        }

        Unit() : Native{*this, 1.0}, NativeValue_(std::nullopt) {}

        explicit Unit(double f, Derived u)
            : Native{*this, 1.0}, NativeValue_(f * u.NativeValue()) {
        }

    private:
        virtual G4double GetScaled(G4double scalar) const {
            if (NativeValue_.has_value()) {
                if (scalar > 0.0) {
                    return NativeValue_.value() * scalar;
                } else {
                    G4Exception("Unit::GetScaled", "InvalidScale", FatalException,
                            "\n Attempted to scale a Measure by a negative Unit\n");
                    exit(1);
                }
            } else {
                G4Exception("Unit::GetScaled", "InvalidScale", FatalException,
                        "\n Attempted to read a Unit (or measure) before setting it;\n");
                exit(1);
            }
        }

        virtual void ScaleSet(G4double value, G4double scale) {
            NativeValue_ = value * scale;
        };

    protected:
        // The public can only make units from existing units, not doubles!!
        explicit Unit<T>(G4double f)
            : Native{*this, 1.0}, NativeValue_(f) {
        };

        static Derived GetGlobalDefault();

        std::optional<G4double> NativeValue_;
    };


    // Delete all those duplicate FromNative blocks and use this:
    template<typename T>
    inline T DLG4::Units::Unit<T>::FromNative(G4double native_value) {
        // This calls the protected explicit Unit(double) constructor
        return T(native_value);
    }


    //#########################################################################//
    //*********************************Length***************************#
    //#########################################################################//
    class Length : public Unit<Length> {
    friend Unit<Length>;
    private:
        explicit Length(double Native)
            : Unit(Native) {
        }

    public:
        explicit Length(double raw, Length u)
            : Unit<Length>(raw, u) {
        }

        Length() { NativeValue_ = GetGlobalDefault().Native(); };

        // Deeclare essentially named singleton factories:
        static const Length fermi;
        static const Length angstrom;
        static const Length nm;
        static const Length pm;
        static const Length um;
        static const Length micron;
        static const Length mm;
        static const Length cm;
        static const Length dm;
        static const Length m;
        static const Length km;

        // Imperial
        static const Length mil;
        static const Length inch;
        static const Length foot;
        static const Length feet;

    };


    template <typename T>
    inline T Unit<T>::GetGlobalDefault() { return T(global_default_unit<T>); }


    //#########################################################################//
    // ******************Mass Class
    //#########################################################################//
    class Mass : public Unit<Mass> {
    friend Unit<Mass>;
    private:
        explicit Mass(double Native)
            : Unit(Native) {
        }

    public:
        Mass() { NativeValue_ = GetGlobalDefault().Native(); };

        explicit Mass(double raw, Mass u)
            : Unit(raw, u) {
        }

        static const Mass g, mg, kg;
    };

    //#########################################################################//
    // ******************Volume Class
    //#########################################################################//
    class Volume : public Unit<Volume> {
        friend Unit<Volume>;
    private:
        explicit Volume(double Native)
            : Unit(Native) {
        }

    public:
        Volume() { NativeValue_ = CLHEP::cm3; };

        explicit Volume(double raw, Volume u)
            : Unit(raw, u) {
        }

        static const Volume cm3, mL, L;
    };


    //#########################################################################//
    // ******************Density Class
    //#########################################################################//
    class Density : public Unit<Density> {
        friend Unit<Density>;
    private:
        explicit Density(double Native)
            : Unit(Native) {
        }

    public:
        Density() { NativeValue_ = GetGlobalDefault().Native(); };

        explicit Density(double raw, Density u)
            : Unit(raw, u) {
        }

        static const Density g_per_cm3;
        static const Density g_per_L; // Same as mg_cm3
        static const Density mg_per_cm3;
    };


    inline Density operator/(Mass m, Volume v) {
        Density x = Density::FromNative(m.Native / v.Native);
        return x;
    }


    // Definitions
    inline const Length Length::fermi{CLHEP::fermi};
    inline const Length Length::angstrom{CLHEP::angstrom};
    inline const Length Length::nm{CLHEP::nanometer};
    inline const Length Length::pm{1e-9 * CLHEP::mm};
    inline const Length Length::um{CLHEP::micrometer};
    inline const Length Length::micron{CLHEP::micrometer}; // Safe alias to um
    inline const Length Length::mm{CLHEP::mm};
    inline const Length Length::cm{CLHEP::cm};
    inline const Length Length::m{CLHEP::m};
    inline const Length Length::km{CLHEP::kilometer};

    // Imperial Logic: 1 inch = 25.4 mm
    inline const Length Length::mil{0.0254}; // 1/1000 of an inch
    inline const Length Length::inch{25.4}; // CLHEP::inch
    inline const Length Length::foot{304.8}; // 12 inches
    inline const Length Length::feet{304.8}; // 12 inches

    inline const Mass Mass::mg{CLHEP::mg};
    inline const Mass Mass::g{CLHEP::g};
    inline const Mass Mass::kg{CLHEP::kg};

    inline const Volume Volume::mL{CLHEP::mL};
    inline const Volume Volume::cm3{CLHEP::mL};
    inline const Volume Volume::L{CLHEP::L};

    inline const Density Density::g_per_cm3{CLHEP::g / CLHEP::cm3};
    inline const Density Density::g_per_L{CLHEP::g / CLHEP::liter};
    inline const Density Density::mg_per_cm3{CLHEP::mg / CLHEP::cm3};

    /**
     * Type-erases unit or value to just a value. BYPASSES TYPE SAFETY!!
     */
    template<class T>
    struct UnitOrValue {
        double val;
        UnitOrValue(const Unit<T> &u) : val(u.Native()) {}
        // Catch any numeric scalar (int, float, etc.)
        template<typename Scalar, typename = std::enable_if_t<std::is_arithmetic_v<Scalar>>>
        UnitOrValue(Scalar d) : val(static_cast<double>(d)) {}
    };


    // Mutliplication and division of scalar and unit create Unit.
    // Which one is the Unit?
    template<typename T, typename U>
    using ResultType = std::conditional_t<std::is_base_of_v<UnitTag, T>, T, U>;

    // --- Multiplication: (Unit * Scalar) or (Scalar * Unit) ---
    template<typename T, typename U,
        typename = std::enable_if_t<std::is_base_of_v<UnitTag, T> != std::is_base_of_v<UnitTag, U>> >
    inline auto operator*(const T &a, const U &b) {
        using R = ResultType<T, U>;
        return R(UnitOrValue<T>(a).val * UnitOrValue<U>(b).val, R::native);
    }

    // --- Division:  (Unit / Scalar) ---
    template<typename T, typename U,
        typename = std::enable_if_t<std::is_base_of_v<UnitTag, T> && !std::is_base_of_v<UnitTag, U>> >
    inline auto operator/(const T &a, const U &b) {
        // T is the Unit, U is the Scalar
        return T(UnitOrValue<T>(a).val / static_cast<double>(b), T::native);
    }

    // --- Addition: T + T (where T inherits from UnitTag) ---
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<UnitTag, T>>>
    inline T operator+(const T& a, const T& b) {
        // Uses the protected constructor/FromNative to return a new dimensioned object
        return T::FromNative(a.Native() + b.Native());
    }

    // --- Subtraction: T - T (where T inherits from UnitTag) ---
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<UnitTag, T>>>
    inline T operator-(const T& a, const T& b) {
        return T::FromNative(a.Native() - b.Native());
    }

    /**
     * A 3 vector that is scalable with/to Units.
     */
    class Unit3Vec {
    public:
        Unit3Vec() = default;

        Unit3Vec(const Unit3Vec &other)
            : NativeVec_(other.NativeVec_),
              default_length_(other.default_length_) {
        }

        // Assignment Operator (for completeness)

        Unit3Vec &operator=(const Unit3Vec &other) {
            if (this != &other) {
                NativeVec_ = other.NativeVec_;
                default_length_ = other.default_length_;
            }
            return *this;
        }

        Unit3Vec(double x, double y, double z, const Length &u) {
            NativeVec_ = G4ThreeVector(x, y, z) * u.Native;
        }

        Unit3Vec(const G4ThreeVector &v, const Length &u) {
            NativeVec_ = v * u.Native;
        }

        Unit3Vec FromNative(const G4ThreeVector &v) {
            return Unit3Vec(v, Length::native);
        }

        // components as Length types...
        Length x() const { return Length::FromNative(Native().x()); }
        Length y() const { return Length::FromNative(Native().y()); }
        Length z() const { return Length::FromNative(Native().z()); }

        void x(const Length& val) { EnsureInternalVec().setX(val.Native); }
        void y(const Length& val) { EnsureInternalVec().setY(val.Native); }
        void z(const Length& val) { EnsureInternalVec().setZ(val.Native); }

        Unit3Vec(const Length& x, const Length& y, const Length& z) {
            NativeVec_ = G4ThreeVector(x.Native, y.Native, z.Native);
        }
        // Extraction back to G4ThreeVector
        G4ThreeVector InUnits() const { return InUnits(default_length_); }

        G4ThreeVector InUnits(const Length &u) const {
            if (!NativeVec_.has_value()) {
                G4Exception("Unit3Vec::InUnits", "Uninitialized", FatalException,
                        "Attempted to read Unit3Vec before setting it.");
            }
            return (*NativeVec_) / u.Native;
        }

        // The "Geant4 Internal" accessor
        G4ThreeVector Native() const {
            if (!NativeVec_.has_value()) {
                G4Exception("Unit3Vec::GetRaw", "Uninitialized", FatalException,
                        "Attempted to read Unit3Vec before setting it.");
            }
            return *NativeVec_;
        }

    protected:
        std::optional<G4ThreeVector> NativeVec_;
        Length default_length_;
    private:
        // Helper to ensure the optional is initialized before we write to it
        G4ThreeVector& EnsureInternalVec() {
            if (!NativeVec_.has_value()) {
                NativeVec_ = G4ThreeVector(0, 0, 0);
            }
            return *NativeVec_;
        }
    };

    inline Unit3Vec operator*(const G4ThreeVector& v, const Length& u) {
        return Unit3Vec(v, u);
    }

    inline Unit3Vec operator*(const Unit3Vec& v, G4double scalar) {
        // We use the FromNative-style construction to wrap the scaled vector
        return Unit3Vec(v.Native() * scalar, Length::native);
    }

    inline Unit3Vec operator*(G4double scalar, const Unit3Vec& v) {
        return v * scalar; // Just reuse the one above
    }

    inline Unit3Vec operator/(const Unit3Vec& v, G4double scalar) {
        return Unit3Vec(v.Native() / scalar, Length::native);
    }

    using UnitlessG4Transform3D = G4Transform3D;
    using Length3Vec = Unit3Vec;

    /**
 * @brief Set the default unit for all VolumeBuilder methods.
 * @ingroup Units
 * */
    template <typename T>
    inline void SetGlobalDefaultUnit(T unit) {
        // Compile-time gate
        static_assert(std::is_base_of_v<DLG4::Units::UnitTag, T>,
            "\n\n ERROR: SetGlobalDefaultUnit only accepts DLG4Unit types! \n"
            " You tried to pass a type that does not inherit from UnitTag.\n");
        DLG4::Units::global_default_unit<T> = unit.Native;
    }



}

#endif //HPGESIM_DLG4UNITS_HH
