/**@file GeosTest.cc
  Simulation main program (sim.cc)
 This version is a complete rewrite by D.S. Leonard 2021.

 Inspired by E.Jeon version, inspired by Glenn Horton-Smith GLG4Sim code Jan 2005.
 */
#include "TheSimulation.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "DetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
//#ifdef G4UI_USE
#include "G4UIExecutive.hh"
//#endif
#include <cstdlib>
#include <filesystem>

#include "G4Run.hh"

#include <memory>
#include <string>


#ifndef GIT_DESCRIBE
#define GIT_DESCRIBE not_set
#endif
#define STR(x) #x
#define STRINGIZE(x) STR(x)

inline void read_init_vis_mac(G4UImanager *theUI) {
    if (std::filesystem::exists("init_vis.mac")) {
        theUI->ApplyCommand("/control/execute init_vis.mac");
    }
}


int main(int argc, char **argv) {
#ifdef __linux__
    // force X11 in wayland
    setenv("QT_QPA_PLATFORM", "xcb", 1);
    setenv("XDG_SESSION_TYPE", "x11", 1);
    setenv("LIBGL_ALWAYS_SOFTWARE","1",1);
    unsetenv("WAYLAND_DISPLAY");
#endif
    // DL adds this straight from git now so we can know the exact commit:
    G4cout << "This is VolumeBuilder Demo git version string: " << STRINGIZE(GIT_DESCRIBE) <<
        G4endl;
    if (argc == 1) {
        G4cout << G4endl;
        G4cout << "    Usage:" << G4endl;
        G4cout << "    " << argv[0] << "                                        : Show usage." <<
            G4endl;
        G4cout << "    " << argv[0] <<
            " term  [file1.mac] [file2.mac] ...      : Run with terminal UI. Optionally execute macro files."
            << G4endl;
        G4cout << "    " << argv[0] <<
            " gui                                    : Run with GUI with prompt for geometry selection."
            << G4endl;
        G4cout << "    " << argv[0] <<
            " gui  file1.mac [file2.mac] ...         : Run with GUI. Optionally execute macro files."
            << G4endl;
        G4cout << "    " << argv[0] <<
            " <file1.mac> [file2.mac] ...            : Execute macro file(s) in batch mode" <<
            G4endl;
        G4cout << "    " << argv[0] <<
            " geos                                   : List registered geometries. " << G4endl;
        G4cout << G4endl;
        G4cout <<
            "    The term and gui options both load init_vis.mac 1st, if available, if and only if files are specified"
            << G4endl;
        G4cout << G4endl;
        return 0;
    }

    TheSimulation the_sim;

    // interactive or batch according to command-line args
    // D.S.L's updated logic,
    //   ...allows term,gui, or batch with arbitary command line file name in all cases.
    //
#ifdef G4VIS_USE
    G4VisExecutive theVisManager;
    theVisManager.Initialize();
#endif
    // user interface
    G4UImanager *theUI = G4UImanager::GetUIpointer();

    // interactive or batch according to command-line args
    bool terminal = false;
    bool gui = false;
    bool readmacs = false;
    int firstmac = 2;
    G4UIsession *theSession = nullptr;
    std::unique_ptr<G4UIterminal> theTerminal;
    std::unique_ptr<G4UIExecutive> theGui;
    if ((strcmp(argv[1], "term") == 0)) {
        // Terminal requested, args may follow:
        theTerminal.reset(new G4UIterminal(new G4UItcsh));
        theSession = theTerminal.get();
        read_init_vis_mac(theUI);
        terminal = true;
        if (argc > 2) {
            readmacs = true;
        }
    } else if (strcmp(argv[1], "gui") == 0) {
        // Gui requested, args may follow:
        theGui.reset(new G4UIExecutive(argc, argv));
        read_init_vis_mac(theUI);
        theSession = theGui->GetSession();
        gui = true;
        //        if ((argc > 2) && (theGui->IsGUI())) {
        if (argc > 2) {
            readmacs = true;
        }
    } else if (strcmp(argv[1], "geos") == 0) {
        // list geometries
        the_sim.ListGeometries();
        return 0;
    } else {
        // non option args, batch mode.
        readmacs = true;
        firstmac = 1;
    }
    if (readmacs) {
        G4String command = "/control/execute ";
        for (int iarg = firstmac; iarg < argc; iarg++) {
            G4String fileName = argv[iarg];
            G4cout << "reading file " << fileName << G4endl;
            theUI->ApplyCommand(command + fileName);
        }
    }
    if (gui) {
        if (!readmacs) {
            the_sim.ListGeometries();
            std::cout << "Please type a geometry from the list above and press enter." << std::endl;
            std::string selection;
            std::cin >> selection;
            theUI->ApplyCommand("/vis/open OGLSQt");
            theUI->ApplyCommand("/detGeometry/select " + selection);
            theUI->ApplyCommand("/run/initialize");
            theUI->ApplyCommand("/vis/drawVolume world");
            theUI->ApplyCommand("/vis/ASCIITree/set/outFile geotree.txt");
            theUI->ApplyCommand("/vis/ASCIITree/verbose 16");
            theUI->ApplyCommand("/vis/drawTree world");
        }
    }
    if (gui || terminal) {
        if (theSession) {
            theSession->SessionStart();
        } else {
            throw std::logic_error("This is a bug in GeoTest.cc\n");
        }
    }
    return 0;
}
