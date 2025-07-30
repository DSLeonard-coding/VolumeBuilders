//HpgeSim geometry include file.  Template by D. Leonard, 2021
//This file is included into parts of the HpgeSim class files
//it is not in a standard header format

//DSL says ideally these should be unique_ptrs not raw,
// could have a raw pointer back to them for syntax compatibility at point of use.
// but for now, just leaving it.

#ifdef INCLUDE_HPGE_GLOBALS
        void ConstructMaterials();

        G4Element *_elementTa; //gw
        G4Element *_elementZn; //gw
        G4Element *_elementCl; //gw
        G4Element *_elementMo; //sy
        G4Element *_elementSn; //sy

        G4Material *_tantalum; // gw
        G4Material *_ptfce; //sy
        G4Material *_insulator; // sy
        G4Material *_brass; // sy
        G4Material *_tinlead; // sy
        G4Material *_Al2O3; // sy

        G4Material *_MoO3; // sy
        G4Material *_plastic_bag; // sy
        G4Material *tetratex; // sy
        G4Material *vikuiti; // sy
        G4Material *_SiEl; // sy
        G4Material *_container; // point source container, polyethylene terephthalate
        G4Material *_source_paper; // point source

        G4Element *_elementH;
        G4Element *_elementC;
        G4Element *_elementN;
        G4Element *_elementO;
        G4Element *_elementAl;
        G4Element *_elementK;
        G4Element *_elementCa;
        G4Element *_elementSi;
        G4Element *_elementCr;
        G4Element *_elementFe;
        G4Element *_elementNi;
        G4Element *_elementNa;
        G4Element *_elementI;
        G4Element *_elementCs;
        G4Element *_elementF;
        G4Element *_elementCu;
        G4Element *_elementPb;
        G4Element *_elementNb;
        G4Element *_elementAu;
        G4Element *_elementGe;
        G4Element *_elementGd;
        G4Element *_elementS;

        G4Material *_GdSO4;
        G4Material *_Gd2O3;
        G4Material *_air;
        G4Material *_rock;
        G4Material *_glass;
        G4Material *_steel;
        G4Material *_water;
        G4Material *_stainless;
        G4Material *_lead;
        G4Material *_aluminum;
        G4Material *_copper;
        G4Material *_copper2;
        G4Material *_niobium;
        G4Material *_gold;
        G4Material *_vm2000;
        G4Material *_N2_Gas;
        G4Material *_calcium;
        G4Material *_mineralOil;
        G4Material *_nylon;
        G4Material *_acrylic;
        G4Material *_blackAcryl;
        G4Material *_polyethylene;
        G4Material *_tyvek;
        G4Material *_teflon;
        G4Material *_quartz;
        G4Material *_grease;
        G4Material *_kevlar;
        G4Material *_vacuum;
        G4Material *_ethylalcohol;
        G4Material *_gewafer;
        G4Material *PMT_Vac;
        G4Material *Dodecane;
        G4Material *Pseudocumene;
        G4Material *PXE;
        G4Material *PPO;
        G4Material *BPO;
        G4Material *BisMSB;
        G4Material *Scintillator;
        G4Material *Photocathode_mat;
        G4Material *GdLoadedScint;
        G4Material *LAB[6];
        G4Material *Lithium;
        G4Material *CaMoO4;
        G4Material *NaI;
        G4Material *CsI;
#endif

