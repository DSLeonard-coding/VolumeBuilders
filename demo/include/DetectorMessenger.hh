// This file is part of the GenericLAND software library.
// $Id: DetectorMessenger.hh,v 1.1.1.1 2016/11/03 10:52:21 ejjeon Exp $
//
// DetectorMessenger.hh by Glenn Horton-Smith, Dec. 1999
#ifndef __DetectorMessenger_hh__
#define __DetectorMessenger_hh__ 1

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class DetectorConstruction;

class DetectorMessenger final: public G4UImessenger {
public:
    DetectorMessenger(DetectorConstruction *Detector);
    ~DetectorMessenger() override;

    void SetNewValue(G4UIcommand *command, G4String newValues) override;
    G4String GetCurrentValue(G4UIcommand *command) override;

private:
    DetectorConstruction *Detector;

    G4UIcommand *DetGeometrySelectCmd;
    G4UIcommand *DetGeometryOptCmd;

    DetectorMessenger *myMessenger{};
};

#endif
