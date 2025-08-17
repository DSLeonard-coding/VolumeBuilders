#pragma once
#ifndef FromG4VSolid_HPP
#define FromG4VSolid_HPP
/// @file
/// @class DLG4::VolumeBuilders::FromG4VSolid
/// @verbatim
// Created by @author Douglas S. Leonard on @date 6/8/25.  All rights Reserved.
// See related files for license, if any is provided.
//
// @ brief A stubbed VolumeBuilder Factory to create a builder from Any geant Solid.
// Only the out-of-class static Factory CreateFromG4VSolid() is exposed.
// Use it to configure logical volume and placement with VolumeBuilder from a standard G4VSolid.
/// @endverbatim
#include "VolumeBuilderTypes.hh"
#include "VolumeBuilder.hh"

namespace DLG4::VolumeBuilders {
    class FromG4VSolid;

    /**
     * @brief Constructor to make a builder from an Existing Geant solid
     * @param solid G4VSolid pointer
     * @return
     * @ingroup Factories
     * */
    FromG4VSolidPtr CreateFromG4VSolid(G4VSolid *solid);

#include <stdexcept>
    /**
     * @brief A solid buider class that just wrap an existing G4VSolid
     * @headerfile FromG4VSolid.hh
     * @see VolumeBuilder for inherited methods.
     * @ingroup Builders
     */
    class FromG4VSolid final: public VolumeBuilder<FromG4VSolid> {
        friend class VolumeBuilder<FromG4VSolid>;
        friend class VolumeBuilderReference;
        friend FromG4VSolidPtr CreateFromG4VSolidBuider(G4VSolid *solid);

    public:
        // No functional public methods.  Only the Factory!
        // Just boilerplate below here:
//        FromG4VSolid(const FromG4VSolid &other) = delete;

        FromG4VSolid(const FromG4VSolid & other);
        FromG4VSolid &operator=(const FromG4VSolid &other) = delete;



    private:
        //Implement pure vitual SolidConstructor from ISolidBuilder
        G4VSolid* SolidConstructor(const G4String &name) override {
            throw std::runtime_error(
                "FromG4VSolid::SolidConstructor(const G4String &name) not implemented.\n"
                "FromG4VSolid is constructed using prebuilt a G4VSolid pointer.");
        }

        // Does this need to exist?  Maybe.
        FromG4VSolid() {}
    };
}
#endif  //FromG4VSolid_HPP