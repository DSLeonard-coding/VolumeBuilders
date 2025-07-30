//
// Created by Douglas S. Leonard on 6/19/25.  All rights Reserved
// See related files for license, if any is provided.
//
#include "VolumeBuilderTypes.hh"
#include "VolumeBuilder.hh"
#include "VolumeBuilder.hpp"
#include "RZBuilder.hh"
#include "BoxBuilder.hh"
#include "FromG4VSolid.hh"
#include "VolumeBuilderReference.hh"
#include "StructureBuilder.hh"
#include "StructureBuilder.hpp"
#include "Assembly.hh"


// instantiate 99% of templates
using namespace DLG4::VolumeBuilders;
template class VolumeBuilder<RZBuilder>;
template class VolumeBuilder<FromG4VSolid>;
template class VolumeBuilder<VolumeBuilderReference>;
template class StructureBuilder<StructureBuilderReference>;
template class StructureBuilder<Assembly>;
template class VolumeBuilder<BoxBuilder>;
