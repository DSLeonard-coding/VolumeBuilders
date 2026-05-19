// D.S Leonard  GeoVariables.inc file to define shared variables for hpgesim geomodules.
// File created in 2026, content is older.
// This file MUST be named this to be included in the GeoModulesContext

// Shared USER variables for geometry modules.  Add what you need here.
        // these serve to provide coordinates between geometry modules mostly.

        // CC1/CC2 variables first:
        /// Z position within Airshield of top of the can for either CC1 or CC2
#ifdef GEOMDULES_CONTEXT_INCLUDES
// includes go here
#include "VolumeBuilders.hh"

#else
   DLG4::VolumeBuilders::Length3Vec sample_center = DLG4::VolumeBuilders::Length3Vec(0,0,0,VB::Length::mm);
#endif