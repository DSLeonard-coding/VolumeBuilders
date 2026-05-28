//
// Created by osboxes on 8/10/25.
//

#ifndef GEOMODULETYPES_HH
#define GEOMODULETYPES_HH
//  no full definition includes here, only forward decls, because
//  everything else includes this and it will makes loops.
#include <G4String.hh>
#include <map>
#include <memory>

namespace DLG4::GeoModules {
// Note by DSL: Why do we use pointers?  Because non pointers must be initializable
// in headers.  Then headers need to include the member's class header.
// Then you get header include loops that can't be fixed with forward decls.
// The loop is broken with a pointer, initialized in the ctor in the cc file,
// with the member class included there!

    class IGeoModule;
    class GeoModulesParser;
    class GeoModulesContext;
    class GeoModulesMessenger;
    using GeoModuleRegistryEntry = std::pair<G4String, std::shared_ptr<IGeoModule>>;
    using GeoModulesMessengerPtr = std::shared_ptr<GeoModulesMessenger>;
    // this is actually THE cannonical store of module instances, so a smart ponter is good here:
    using GeoModulesMap = std::map<G4String, std::shared_ptr<IGeoModule>>;
    using OptsMap = std::map<G4String, G4String>;
    using GeoModulesParserPtr = std::shared_ptr<GeoModulesParser>;
    using GeoModulesContextPtr = std::shared_ptr<GeoModulesContext>;
    //The recipe probably doesn't need ownership, but whatever:
    //Just have to be careful to use shared_from_this() (correctly!).
    using GeoRecipe = void (*)(std::shared_ptr<GeoModulesContext> context);
    using GeoRecipesMap = std::map<G4String, GeoRecipe>;
    using GeoModulePtr = std::shared_ptr<IGeoModule>;
    using GeoRecipeMapPtr = std::shared_ptr<GeoRecipesMap>;
    using GeoModulesMapPtr = std::shared_ptr<GeoModulesMap>;
    using OptsMapPtr = std::shared_ptr<OptsMap>;

    /**
 * There might have even been a reason to make this.
 */
    struct DLG4Geos3vec {
        G4double x{0};
        G4double y{0};
        G4double z{0};
    };


}

#endif  //GEOMODULETYPES_HH
