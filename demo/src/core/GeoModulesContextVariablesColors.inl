// GeoModulesContextVariablesColors.inl  D.S. Leonard 2026
#ifdef GEOMDULES_CONTEXT_INCLUDES
#include <G4Colour.hh>
#else
    struct Colors {
        // make colours
        G4Colour coppertone=G4Colour(.72, .45, .2);
        G4Colour white=G4Colour(1.0, 1.0, 1.0);
        G4Colour grey = G4Colour(0.5, 0.5, 0.5);
        G4Colour lgrey = G4Colour(.85, .85, .85);
        G4Colour red = G4Colour(1.0, 0.0, 0.0);
        G4Colour blue = G4Colour(0.0, 0.0, 1.0);
        G4Colour cyan = G4Colour(0.0, 1.0, 1.0);
        G4Colour magenta = G4Colour(1.0, 0.0, 1.0);
        G4Colour yellow = G4Colour(1.0, 1.0, 0.0);
        G4Colour orange = G4Colour(.75, .55, 0.0);
        G4Colour lblue = G4Colour(0.0, 0.0, .75);
        G4Colour lgreen = G4Colour(0.0, .75, 0.0);
        G4Colour green = G4Colour(0.0, 1.0, 0.0);
        G4Colour brown = G4Colour(0.7, 0.4, 0.1);
    };
#endif