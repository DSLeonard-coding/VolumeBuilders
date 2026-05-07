#include "DetectorConstruction_includes.hh"  // common includes
#define INCLUDE_GEOMODULE_SHARED_OBJECTS
#include"MaterialsGeoModule.inc"
#undef INCLUDE_GEOMODULE_SHARED_OBJECTS
#include <GeoModules/GeoModules.hh>
using namespace CLHEP;

// Materials are now just a GeoModule.
void MaterialsGeoModule::Construct(GeoModulesContextPtr context) {
    G4NistManager* nist = G4NistManager::Instance();

    //  DSL copies the material defs up to our level and removes this:
    // Problem1... it called the materials_ data file (optical properties
    //    for KamLand etc) which we don't need, creating errors if missing.
    // Probelm2... it hides control of these definitions in a library we
    //    don't control, and created warnings if we redfine them.

    // 2026: No longer exporting variable names for use (no header variables).
    // Instead Use G4Materials* GeoModulesContext::GetMaterial(const G4String &materialName)
    // ie VBHelpers::GetMaterial("name")
    // This will also get NIST materials now!!!

    //////////////////////////////////
    G4double a; // atomic mass
    G4double z; // atomic number
    G4String name;
    G4String symbol;


    // NIST replacements for elements
    auto elementH  = nist->FindOrBuildElement("H");
    auto elementB  = nist->FindOrBuildElement("B");
    auto elementC  = nist->FindOrBuildElement("C");
    auto elementN  = nist->FindOrBuildElement("N");
    auto elementO  = nist->FindOrBuildElement("O");
    auto elementAl = nist->FindOrBuildElement("Al");
    auto elementSi = nist->FindOrBuildElement("Si");
    auto elementK  = nist->FindOrBuildElement("K");
    auto elementCr = nist->FindOrBuildElement("Cr");
    auto elementFe = nist->FindOrBuildElement("Fe");
    auto elementNi = nist->FindOrBuildElement("Ni");
    auto elementNa = nist->FindOrBuildElement("Na");
    auto elementI  = nist->FindOrBuildElement("I");
    auto elementCs = nist->FindOrBuildElement("Cs");
    auto elementCa = nist->FindOrBuildElement("Ca");
    auto elementF  = nist->FindOrBuildElement("F");
    auto elementCu = nist->FindOrBuildElement("Cu");
    auto elementPb = nist->FindOrBuildElement("Pb");
    auto elementNb = nist->FindOrBuildElement("Nb");
    auto elementAu = nist->FindOrBuildElement("Au");
    auto elementGe = nist->FindOrBuildElement("Ge");
    auto elementTa = nist->FindOrBuildElement("Ta");
    auto elementZn = nist->FindOrBuildElement("Zn");
    auto elementCl = nist->FindOrBuildElement("Cl");
    auto elementMo = nist->FindOrBuildElement("Mo");
    auto elementSn = nist->FindOrBuildElement("Sn");
    auto elementGd = nist->FindOrBuildElement("Gd");
    auto elementS  = nist->FindOrBuildElement("S");

    ///////////////////////////////////
    G4double density;
    G4double mol;
    G4int nelements;
    G4int natoms;
    G4MaterialPropertiesTable *MPT;

    //------------Bulk BOH3--------- added by DSL, May 20223

    density = 0.8 * g / cm3; // a rough figure for powder/granular, may be 0.6 to 1.
    name = "boric_powder";
    nelements = 3;

    auto boric_powder = new G4Material(name, density, nelements);
    boric_powder->AddElement(elementB, natoms = 1);
    boric_powder->AddElement(elementH, natoms = 3);
    boric_powder->AddElement(elementO, natoms = 3);
    //------------MoO3--------- added by DSL, May 20223

    density = 1.3 * g / cm3; // a rough figure for powder.
    name = "MoO3";
    nelements = 2;

    auto MoO3 = new G4Material(name, density, nelements);
    MoO3->AddElement(elementMo, natoms = 1);
    MoO3->AddElement(elementO, natoms = 3);

    //------------Gd2O3--------- D.S.L. Aug 2022

    // Density is for ideal solid, ie irrelevant actually
    //   will use material copier to adjust for measured value in individual powders
    density = 7.41 * g / cm3;
    nelements = 2;
    name = "Gd2O3";

    auto Gd2O3 = new G4Material(name, density, nelements);
    Gd2O3->AddElement(elementGd, natoms = 2);
    Gd2O3->AddElement(elementO, natoms = 3);

    //------------GdSo4--------- D.S.L.'s implementation (no access to old ones)

    // not very sure about this density,
    //   will use material copier to adjust for measured value in individual powders
    density = 4.139 * g / cm3;
    nelements = 3;
    name = "GdSO4";

    auto GdSO4 = new G4Material(name, density, nelements);
    GdSO4->AddElement(elementGd, natoms = 1);
    GdSO4->AddElement(elementS, natoms = 1);
    GdSO4->AddElement(elementO, natoms = 4);

    //---------- point source container

    density = 1.38 * g / cm3;
    nelements = 3;
    name = "poly_container";

    auto poly_container = new G4Material(name, density, nelements);
    poly_container->AddElement(elementC, natoms = 10);
    poly_container->AddElement(elementH, natoms = 8);
    poly_container->AddElement(elementO, natoms = 4);

    //---------- point source paper

    name = "source_paper";
    density = 1.5 * g / cm3;
    nelements = 3;

    auto source_paper = new G4Material(name, density, nelements);
    source_paper->AddElement(elementC, natoms = 6);
    source_paper->AddElement(elementH, natoms = 10);
    source_paper->AddElement(elementO, natoms = 5);

    //---------PTFCE
    //for CC2 structure

    name = "PTFCE";
    density = 2.1 * g / cm3;
    nelements = 3;

    auto ptfce = new G4Material(name, density, nelements);
    ptfce->AddElement(elementF, natoms = 3);
    ptfce->AddElement(elementC, natoms = 2);
    ptfce->AddElement(elementCl, natoms = 1);

    //--------insulator
    //for CC2 structure

    name = "insulator";
    density = 1. * g / cm3;
    nelements = 2;

    auto insulator = new G4Material(name, density, nelements);
    insulator->AddElement(elementH, natoms = 4);
    insulator->AddElement(elementC, natoms = 2);

    //---------brass
    //for CC2 structure

    name = "brass";
    density = 6.53 * g / cm3;
    nelements = 2;

    auto brass = new G4Material(name, density, nelements);
    brass->AddElement(elementZn, 40. * perCent);
    brass->AddElement(elementCu, 60. * perCent);

    //===================SAMPLE MATERIAL===========================

    //-- tinlead

    name = "tinlead";
    density = 1.138 * g / cm3;
    nelements = 2;

    auto tinlead = new G4Material(name, density, nelements);
    tinlead->AddElement(elementSn, natoms = 1);
    tinlead->AddElement(elementPb, natoms = 1);

    //-- cmd20 tetratex

    name = "tetratex";
    density = 3.9985 * g / cm3;
    nelements = 2;

    auto tetratex = new G4Material(name, density, nelements);
    tetratex->AddElement(elementC, natoms = 2);
    tetratex->AddElement(elementF, natoms = 4);

    //-- cmd38 vikuiti

    name = "vikuiti";
    density = 2.42816 * g / cm3;
    nelements = 3;

    auto vikuiti // Mylar -> Polyethiylene terephthalate
        = new G4Material(name, density, nelements);
    vikuiti->AddElement(elementC, natoms = 10);
    vikuiti->AddElement(elementH, natoms = 8);
    vikuiti->AddElement(elementO, natoms = 4);

    //-- cmd41 SiEl

    name = "SiEl";
    density = 0.93974 * g / cm3;
    nelements = 4;

    auto SiEl // Mylar -> Polyethiylene terephthalate
        = new G4Material(name, density, nelements);
    SiEl->AddElement(elementC, natoms = 2);
    SiEl->AddElement(elementH, natoms = 6);
    SiEl->AddElement(elementO, natoms = 1);
    SiEl->AddElement(elementSi, natoms = 1);

    //----------Ta
    //for Ta sample

    name = "tantalum";
    density = 16.69 * g / cm3;
    nelements = 1;

    auto tantalum = new G4Material(name, density, nelements);
    tantalum->AddElement(elementTa, natoms = 1);

    // --- fused alumina // Su-yeon
    //sample material

    name = "fused_alumina";
    if (!(context->GetExtraOpt("Al_dens") >> density)) {
        //default
        density = 3.05084 * g / cm3;
        // Echo command instructions:
        G4cout << "Use:" << G4endl;
        G4cout << "    /detGeometry/opt \"Al_dens <val>\" " << G4endl;
        G4cout << "    to select Al density in g/cm^3." << G4endl;
        G4cout << "Assuming default value 3.05084" << G4endl;
    } else {
        density *= g / cm3;
    }

    //DL makes uncommenting obsolete:
    //  density = 3.05084*g/cm3; //cmd68
    //density = 2.79948*g/cm3; //cmd89
    //density = 1.91728*g/cm3; //cmd87
    nelements = 2;

    auto Al2O3 = new G4Material(name, density, nelements);
    Al2O3->AddElement(elementAl, natoms = 2);
    Al2O3->AddElement(elementO, natoms = 3);

    // --- Air  N=70% O=30% ---------
    name = "air";
    density = 1.29e-3 * g / cm3;
    nelements = 2;

    auto air = new G4Material(name, density, nelements);
    air->AddElement(elementN, 70 * perCent);
    air->AddElement(elementO, 30 * perCent);

    // --- PMT vacuum is very dilute air -------
    density = 1e-3 * kGasThreshold;         // from PhysicalConstants.h
    G4double temperature = STP_Temperature; // from PhysicalConstants.h
    G4double pressure = STP_Pressure * density / (1.29e-3 * g / cm3);
    auto PMT_Vac = new G4Material(name = "PMT_Vac", density, 1, kStateGas, temperature, pressure);
    PMT_Vac->AddMaterial(air, 1.);

    // --- Rock  SiO2 ---------------
    name = "rock";
    density = 2.7 * g / cm3;
    nelements = 2;

    auto rock = new G4Material(name, density, nelements);
    rock->AddElement(elementSi, natoms = 1);
    rock->AddElement(elementO, natoms = 2);

    // --- Glass  SiO2 ---------------
    name = "glass";
    density = 2.2 * g / cm3; // changed 1999/12/03 (was 2.7*g/cm3) -- GAS
    nelements = 2;

    auto glass = new G4Material(name, density, nelements);
    glass->AddElement(elementSi, natoms = 1);
    glass->AddElement(elementO, natoms = 2);

    // --- Steel  Fe ----------------
    name = "steel";
    density = 7.87 * g / cm3;
    nelements = 1;

    auto steel = new G4Material(name, density, nelements);
    steel->AddElement(elementFe, natoms = 1);

    // --- Water  H2O ---------------
    name = "water";
    density = 1.0 * g / cm3;
    nelements = 2;

    auto water = new G4Material(name, density, nelements);
    water->AddElement(elementH, natoms = 2);
    water->AddElement(elementO, natoms = 1);

    // --- Stainless Steel  71% Fe, 19% Cr, 10% Ni ------
    name = "stainless";
    density = 7.87 * g / cm3;
    nelements = 3;

    auto stainless = new G4Material(name, density, nelements);
    stainless->AddElement(elementFe, 0.71);
    stainless->AddElement(elementCr, 0.19);
    stainless->AddElement(elementNi, 0.10);

    // --- Lead  Pb ------
    name = "lead";
    density = 11.35 * g / cm3;
    nelements = 1;

    auto lead = new G4Material(name, density, nelements);
    lead->AddElement(elementPb, natoms = 1);

    // --- Aluminum  Al ------
    name = "aluminum";
    density = 2.7 * g / cm3;
    nelements = 1;

    auto aluminum = new G4Material(name, density, nelements);
    aluminum->AddElement(elementAl, natoms = 1);

    // --- Copper Cu ------
    name = "copper";
    density = 8.96 * g / cm3;
    nelements = 1;

    auto copper = new G4Material(name, density, nelements);
    copper->AddElement(elementCu, natoms = 1);

    name = "copper2";
    density = 8.96 * g / cm3;
    nelements = 1;

    auto copper2 = new G4Material(name, density, nelements);
    copper2->AddElement(elementCu, natoms = 1);

    // --- Niobium Nb ------
    name = "niobium";
    density = 8.57 * g / cm3;
    nelements = 1;

    auto niobium = new G4Material(name, density, nelements);
    niobium->AddElement(elementNb, natoms = 1);

    // --- Gold ------
    name = "gold";
    density = 19.32 * g / cm3;
    nelements = 1;

    auto gold = new G4Material(name, density, nelements);
    gold->AddElement(elementAu, natoms = 1);

    // --- Vm2000 ------
    name = "vm2000";
    density = 0.9 * g / cm3;
    nelements = 2;

    auto vm2000 = new G4Material(name, density, nelements);
    vm2000->AddElement(elementC, natoms = 2);
    vm2000->AddElement(elementH, natoms = 4);

    // --- N2 gas ------
    name = "N2_Gas";
    density = 1.165 * g / cm3;
    nelements = 1;

    auto N2_Gas = new G4Material(name, density, nelements);
    N2_Gas->AddElement(elementN, natoms = 1);

    // --- Calcium ------
    name = "calcium";
    density = 1.165 * g / cm3;
    nelements = 1;

    auto calcium = new G4Material(name, density, nelements);
    calcium->AddElement(elementCa, natoms = 1);

    // --- Mineral Oil  (CH2)n ------
    name = "mineralOil";
    density = 0.77 * g / cm3;
    nelements = 2;

    auto mineralOil = new G4Material(name, density, nelements);
    mineralOil->AddElement(elementC, natoms = 1);
    mineralOil->AddElement(elementH, natoms = 2);

    // Use the chemical formula as a useful label
    mineralOil->SetChemicalFormula("OIL");

    // --- Vacuum ------
    a = 4. * g / mole;
    //    density = 0.1786 * mg / cm3;
    density = 0. * mg / cm3;
    auto vacuum = new G4Material("vacuum", z = 2., a, density, kStateGas, 4.3 * kelvin, 1.e-8 * bar);

    // --- GeWafer ------
    name = "gewafer";
    density = 5.323 * g / cm3;
    nelements = 1;

    auto gewafer = new G4Material(name, density, nelements);
    gewafer->AddElement(elementGe, natoms = 1);

    // For the moment, no molecular weight defined... Use Dodecane!

    //
    // .......................... Dodecane .............................
    //
    density = 0.749 * g / cm3;
    nelements = 2;
    auto dodecane = new G4Material("dodecane", density, nelements);

    // Use the chemical formula as a label for the function in the scintillator
    dodecane->SetChemicalFormula("OIL");

    dodecane->AddElement(elementC, 12);
    dodecane->AddElement(elementH, 26);

    // Calculate the molecular weight
    mol = elementC->GetA() * 12 + elementH->GetA() * 26;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PC
    dodecane->SetMaterialPropertiesTable(MPT);

    //
    // ............................. PC ................................
    //
    // Pseudo-cumene (C9 H12) also called 1,2,4-Trimethybenzene

    density = 0.8758 * g / cm3; // at T=20 deg C
    nelements = 2;
    auto pseudocumene = new G4Material(name = "pseudocumene", density, nelements);
    pseudocumene->AddElement(elementC, 9);
    pseudocumene->AddElement(elementH, 12);

    // Use the chemical formula as a label
    pseudocumene->SetChemicalFormula("AROMATIC");

    // Calculate the molecular weight
    mol = elementC->GetA() * 9 + elementH->GetA() * 12;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the pseudocumene
    pseudocumene->SetMaterialPropertiesTable(MPT);

    //
    // ............................. LAB ................................
    //
    // LAB (CnH2n+1-C6H5, n=9~14) //added by EJJeon (2008-02-26)
    G4int num_C;
    G4int num_H;
    char Name[15];
    G4Material* LAB[15];
    density = 0.86 * g / cm3;
    nelements = 2;
    for (int i = 0; i < 6; i++) {
        num_C = i + 15;
        num_H = 2 * (i + 9) + 6;
        sprintf(Name, "LAB_n=%i", i + 9);
        LAB[i] = new G4Material(Name, density, nelements);
        LAB[i]->AddElement(elementC, num_C);
        LAB[i]->AddElement(elementH, num_H);

        // Use the chemical formula as a label
        LAB[i]->SetChemicalFormula("AROMATIC");

        // Calculate the molecular weight
        mol = elementC->GetA() * num_C + elementH->GetA() * num_H;
        // Allocate memory for a new Material Property Table
        MPT = new G4MaterialPropertiesTable();
        // Fill with the molecular weight
        MPT->AddConstProperty("MOL", mol / g);
        // Attach this MPT to the pseudocumene
        LAB[i]->SetMaterialPropertiesTable(MPT);
    }

    //
    // ............................. PXE ..............................
    //

    density = 0.99 * g / cm3;
    nelements = 2;
    auto PXE = new G4Material("PXE", density, nelements);

    // Use the chemical formula as a label
    PXE->SetChemicalFormula("AROMATIC");

    PXE->AddElement(elementC, 16);
    PXE->AddElement(elementH, 18);

    // Calculate the molecular weight
    mol = elementC->GetA() * 16 + elementH->GetA() * 18;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PXE
    PXE->SetMaterialPropertiesTable(MPT);

    //
    // ............................. PPO ...............................
    //
    // PPO (C15 H11 N 0) -- also called DPO, 2,5-diphenyloxazole

    density = 1.06 * g / cm3; // ??? at T=?
    auto PPO = new G4Material(name = "PPO", density, nelements = 4);

    // Use the chemical formula as a label
    PPO->SetChemicalFormula("FLUOR");

    PPO->AddElement(elementC, 15);
    PPO->AddElement(elementH, 11);
    PPO->AddElement(elementN, 1);
    PPO->AddElement(elementO, 1);

    // Calculate the molecular weight
    mol = elementC->GetA() * 15 + elementH->GetA() * 11 + elementN->GetA() * 1 +
          elementO->GetA() * 1;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PC
    PPO->SetMaterialPropertiesTable(MPT);

    //
    // ............................. BPO ...............................
    //
    // BPO (C21 H15 N O) -- like PPO, with one more phenyl ring

    density = 1.06 * g / cm3; // unknown (set to the PPO value)

    auto BPO = new G4Material(name = "BPO", density, nelements = 4);

    // Use the chemical formula as a label
    BPO->SetChemicalFormula("FLUOR");

    BPO->AddElement(elementC, 21);
    BPO->AddElement(elementH, 15);
    BPO->AddElement(elementN, 1);
    BPO->AddElement(elementO, 1);

    // Calculate the molecular weight
    mol = elementC->GetA() * 21 + elementH->GetA() * 15 + elementN->GetA() * 1 +
          elementO->GetA() * 1;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the PC
    BPO->SetMaterialPropertiesTable(MPT);

    //
    // .............................. Bis-MSB .....................................
    //

    density = 1.3 * g / cm3; // Unknown
    nelements = 2;
    auto BisMSB = new G4Material("BisMSB", density, nelements);

    // Use the chemical formula as a label
    BisMSB->SetChemicalFormula("WLS");

    BisMSB->AddElement(elementC, 24);
    BisMSB->AddElement(elementH, 22);
    //

    // Calculate the molecular weight
    mol = elementC->GetA() * 24 + elementH->GetA() * 22;
    // Allocate memory for a new Material Property Table
    MPT = new G4MaterialPropertiesTable();
    // Fill with the molecular weight
    MPT->AddConstProperty("MOL", mol / g);
    // Attach this MPT to the Bis-MSB
    BisMSB->SetMaterialPropertiesTable(MPT);

    // GenericLAND scintillator
    density = 0.78 * g / cm3;
    auto scintillator = new G4Material(name = "scintillator", density, nelements = 3);
    G4double PPO_fraction = 1.5 * g / (1e3 * cm3 * density); // 1.5 g/l
    scintillator->AddMaterial(mineralOil, 0.80 / (1.0 + PPO_fraction));
    scintillator->AddMaterial(pseudocumene, 0.20 / (1.0 + PPO_fraction));
    scintillator->AddMaterial(PPO, PPO_fraction / (1.0 + PPO_fraction));
    scintillator->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);

    // EJ: Gd-loaded Lab target material
    G4NistManager *man = G4NistManager::Instance();
    G4Material *erialGd = man->FindOrBuildMaterial("G4_Gd");
    density = 0.78 * g / cm3;
    nelements = 4;
    auto GdLoadedScint = new G4Material(name = "GdLoadedScint", density, nelements);

    GdLoadedScint->AddMaterial(mineralOil, 0.80 / (1.0 + PPO_fraction));
    GdLoadedScint->AddMaterial(pseudocumene, 0.199 / (1.0 + PPO_fraction));
    GdLoadedScint->AddMaterial(erialGd, 0.001 / (1.0 + PPO_fraction)); // EJ: 0.1% loaded
    GdLoadedScint->AddMaterial(PPO, PPO_fraction / (1.0 + PPO_fraction));

    //
    // ............................. CaMoO4 ...............................
    //
    G4int iz; // atomic number(protons)
    G4int n;  // number of nucleons
    G4int nisotope;
    G4double abundance;

    auto Ca40 = new G4Isotope(name = "Calcium", iz = 20, n = 40, a = 40.078 * g / mole);
    auto elementDeplCa = new G4Element(name = "enriched Calsium", symbol = "Ca", nisotope = 1);
    elementDeplCa->AddIsotope(Ca40, abundance = 100. * perCent);

    // G4Isotope* Mo98 = new G4Isotope(name="Molybdenum98", iz=42, n=98, a=97.9054073*g/mole);
    auto Mo100 = new G4Isotope(name = "Molybdenum100", iz = 42, n = 100, a = 99.907477 * g / mole);
    auto elementEnrMo = new G4Element(name = "enriched Molybdenum", symbol = "Mo", nisotope = 1);
    elementEnrMo->AddIsotope(Mo100, abundance = 100. * perCent);

    density = 4.34 * g / cm3;
    nelements = 3;
    auto CaMoO4 = new G4Material(name = "CaMoO4", density, nelements);
    CaMoO4->AddElement(elementDeplCa, 1);
    CaMoO4->AddElement(elementEnrMo, 1);
    CaMoO4->AddElement(elementO, 4);
    CaMoO4->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);

    // ............................. CsI crystal ...........................
    //
    name = "CsI";
    density = 4.51 * g / cm3;
    nelements = 2;
    auto CsI = new G4Material(name, density, nelements);
    CsI->AddElement(elementCs, natoms = 1);
    CsI->AddElement(elementI, natoms = 1);
    CsI->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);
    // EJ: Birks Constant for the LXe scintillator(0.126*CLHEP::mm/MeV)
    // EJ: Birks Constant for the Water scintillator(0.126*CLHEP::mm/MeV)
    // EJ: Birks Constant for the LAB-based liquid scintillator(0.117*CLHEP::mm/MeV)
    // EJ: Birks Constant for the Gd-loaded LS_LAB scintilltor(0.124*CLHEP::mm/MeV)

    //
    // ............................. NaI crystal ...........................
    //
    name = "NaI";
    density = 3.67 * g / cm3;
    nelements = 2;
    auto NaI = new G4Material(name, density, nelements);
    NaI->AddElement(elementNa, natoms = 1);
    NaI->AddElement(elementI, natoms = 1);
    NaI->GetIonisation()->SetBirksConstant(0.117 * mm / MeV);


    //
    // ............................. Teflon PTFE ...........................
    //
    name = "teflon";
    density = 2.2 * g / cm3;
    nelements = 2;
    auto teflon = new G4Material(name, density, nelements);
    teflon->AddElement(elementC, natoms = 2);
    teflon->AddElement(elementF, natoms = 4);

    //
    // ............................. Ethyl Alcohol ....................
    //
    auto ethylalcohol = man->FindOrBuildMaterial("G4_ETHYL_ALCOHOL");

    // --- Nylon    H        O  -----
    //             -N-(CH2)5-C-
    name = "nylon";
    density = 0.805 * g / cm3;
    nelements = 4;

    auto nylon = new G4Material(name, density, nelements);
    nylon->AddElement(elementH, natoms = 11);
    nylon->AddElement(elementC, natoms = 6);
    nylon->AddElement(elementO, natoms = 1);
    nylon->AddElement(elementN, natoms = 1);

    //               H H
    // --- Acrylic  -C-C- --------------------
    //               H COOCH3
    name = "acrylic";
    density = 1.14 * g / cm3;
    nelements = 3;

   auto  acrylic = new G4Material(name, density, nelements);
    acrylic->AddElement(elementH, natoms = 6);
    acrylic->AddElement(elementC, natoms = 4);
    acrylic->AddElement(elementO, natoms = 2);

    // --- Polyethylene
    name = "polyethylene";
    density = 0.91 * g / cm3;
    nelements = 2;

    auto polyethylene = new G4Material(name, density, nelements);
    polyethylene->AddElement(elementH, natoms = 2);
    polyethylene->AddElement(elementC, natoms = 1);

    // --- Tyvek  ==  High Density Polyethylene:  (...-CH2-CH2-...)*n
    name = "tyvek";
    density = 0.96 * g / cm3;
    nelements = 2;

    auto tyvek = new G4Material(name, density, nelements);
    tyvek->AddElement(elementH, natoms = 2);
    tyvek->AddElement(elementC, natoms = 1);


    // --- kevlar == (-NH-C6H4-NH-CO-C6H4-CO-)*n
    name = "kevlar";
    density = 1.44 * g / cm3; // ??
    nelements = 4;

    auto kevlar = new G4Material(name, density, nelements);
    kevlar->AddElement(elementH, natoms = 10);
    kevlar->AddElement(elementC, natoms = 14);
    kevlar->AddElement(elementO, natoms = 2);
    kevlar->AddElement(elementN, natoms = 2);
}
