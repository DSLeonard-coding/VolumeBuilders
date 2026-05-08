/// SimulationMessenger composed by Douglas S. Leonard 2025,
///  Initially at least as selected commands from CupDebugMessenger
///
/// The top level messenger.  All commands should go in a messenger specific
/// to the class in use, or in this top level messenger, as all couplings should be maintainable
/// outside the library code.
///
///
// This file is part of the GenericLAND software library.
// $Id: CupDebugMessenger.hh,v 1.1.1.1 2016/10/31 08:41:44 ejjeon Exp $
//
// Probably originally named GLG4DebugMessenger.hh by Glenn Horton-Smith, Dec. 1999
// Rebranded as
// $Id: CupDebugMessenger.hh,v 1.1.1.1 2016/10/31 08:41:44 ejjeon Exp $

#ifndef __SimulationMessenger_hh__
#define __SimulationMessenger_hh__

#include "G4UImessenger.hh"

class G4UIcommand;

class SimulationMessenger : public G4UImessenger {
public:

    SimulationMessenger();
    ~SimulationMessenger();

    void SetNewValue(G4UIcommand *command, G4String newValues);
    G4String GetCurrentValue(G4UIcommand *command);

private:
    G4UIdirectory *DebugDir;
    G4UIcommand *dovercmd;
    G4UIcommand *dreadcmd;
    G4UIcommand *ddumpcmd;
    G4UIcommand *seedcmd;
    G4UIcommand *runIDcmd;
#ifdef G4DEBUG
    G4UIcommand *illucmd;
#endif
};

#endif
