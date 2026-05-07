// D.S Leonard  GeoVariables.inc file to define shared variables for hpgesim geomodules.
// File created in 2026, content is older.
// This file MUST be named this to be included in the GeoModulesContext

// Shared USER variables for geometry modules.  Add what you need here.
        // these serve to provide coordinates between geometry modules mostly.

        // CC1/CC2 variables first:
        /// Z position within Airshield of top of the can for either CC1 or CC2
#ifdef GEOMDULES_CONTEXT_INCLUDES
// includes go here
//#include <blah.hh>
#else
        G4double sample_base_z = 0;
        /// A position within Airshield of bottom of chamber, initially implemented for array only.
        G4double chamber_base_z = 0;
        ///global offset of main chamber volume
        DLG4Geos3vec airshield_global_pos{};
        /// main chamber volume
        G4VPhysicalVolume *AirshieldPhys = nullptr;

        //Array variables:
        //***  Array geometry interface variables ****/
        //  By DL.
        /// Array_z_separation, default of 50mm;  Too many ways to set this honestly.
        G4double z_gap = 50 * CLHEP::mm;
        ///Mo sequential powder batch number.  Should this be global?
        G4int powder_num = 0;
        ///z position of sample supporter shelf
        /// many samples key off this or the next;
        G4double array_sup_top_z = 0;
        /// top of the bottom endcap:
        G4double bot_endcap_top_z = 0;
        /// bottom of the top endcap;
        G4double top_endcap_bot_z = 0;
        /// top of the gian base acrylic. Use this instead of hex plate usually.
        G4double array_surround_top_z = 0;

        ///**************WARNING... ***************
        /// the acrylic surround sits 0.6 mm above the hex plate
        ///Samples that sit across both must reference the surround, NOT the hex plate
        ///or you will get overlaps!!
        G4double bot_hex_plate_top_z_ = 0;
        // bottom of top hex plate
        G4double top_hex_plate_bot_z = 0;

        ///Height of top of sample acrylic plate;
        G4double array_plate_top_z = 0;

        /// x positions of cans, used to reference sources etc.
        G4double xCell[17] = {};
        /// y positions of cans, used to reference sources etc.
        G4double yCell[17] = {};
        /// z positions of cans, used to reference sources etc.
        G4double zCell[17] = {};

        bool AcrylicPlate2019_Array_is_constructed{false};
#endif