//
// Created by Douglas S. Leonard on 6/19/25.  All rights Reserved
// See related files for license, if any is provided.
//
#include "VolumeBuildersTypes.hh"
#include "VolumeBuilder.hh"
#include "VolumeBuilder.hpp"
#include "RZBuilder.hh"
#include "BoxBuilder.hh"
#include "FromG4VSolid.hh"
#include "BuilderViewCore.hh"
#include "StructureBuilder.hh"
#include "StructureBuilder.hpp"
#include "Assembly.hh"


// instantiate 99% of templates
using namespace DLG4::VolumeBuilders;
using namespace DLG4::VolumeBuilders::Builders;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<RZBuilder>;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<FromG4VSolid>;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<BuilderViewCore>;
template class DLG4::VolumeBuilders::Builders::StructureBuilder<StructureViewCore>;
template class DLG4::VolumeBuilders::Builders::StructureBuilder<Assembly>;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<BoxBuilder>;
