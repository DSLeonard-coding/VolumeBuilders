//
// Created by Douglas S. Leonard on 6/19/25.  All rights Reserved
// See related files for license, if any is provided.
//
#include "VolumeBuildersTypes.hh"
#include "VolumeBuilder.hh"
#include "VolumeBuilder.hpp"
#include "RZBuilderCore.hh"
#include "BoxBuilderCore.hh"
#include "FromG4VSolidCore.hh"
#include "BuilderViewCore.hh"
#include "StructureBuilder.hh"
#include "StructureBuilder.hpp"
#include "AssemblyCore.hh"
#include "StructureViewCore.hh"


// instantiate 99% of templates
using namespace DLG4::VolumeBuilders;
using namespace DLG4::VolumeBuilders::Builders;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<_internals_::RZBuilderCore>;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<_internals_::FromG4VSolidCore>;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<BuilderViewCore>;
template class DLG4::VolumeBuilders::Builders::StructureBuilder<StructureViewCore>;
template class DLG4::VolumeBuilders::Builders::StructureBuilder<_internals_::AssemblyCore>;
template class DLG4::VolumeBuilders::Builders::VolumeBuilder<_internals_::BoxBuilderCore>;
