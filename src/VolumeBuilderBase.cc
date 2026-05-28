//
// Created by Douglas S. Leonard on 6/19/25.  All rights Reserved
// See related files for license, if any is provided.
//
#include "VolumeBuildersTypes.hh"
#include "VolumeBuilderBase.hh"
#include "VolumeBuilderBase.hpp"
#include "RZBuilderCore.hh"
#include "BoxBuilderCore.hh"
#include "FromG4VSolidCore.hh"
#include "VolumeBuilderCore.hh"
#include "StructureBuilderBase.hh"
#include "StructureBuilderBase.hpp"
#include "AssemblyCore.hh"
#include "StructureBuilderCore.hh"


// instantiate 99% of templates
using namespace DLG4::VolumeBuilders;
using namespace DLG4::VolumeBuilders::BuilderCores;
template class DLG4::VolumeBuilders::BuilderCores::VolumeBuilderBase<_internals_::RZBuilderCore>;
template class DLG4::VolumeBuilders::BuilderCores::VolumeBuilderBase<_internals_::FromG4VSolidCore>;
template class DLG4::VolumeBuilders::BuilderCores::VolumeBuilderBase<VolumeBuilderCore>;
template class DLG4::VolumeBuilders::BuilderCores::StructureBuilderBase<StructureBuilderCore>;
template class DLG4::VolumeBuilders::BuilderCores::StructureBuilderBase<_internals_::AssemblyCore>;
template class DLG4::VolumeBuilders::BuilderCores::VolumeBuilderBase<_internals_::BoxBuilderCore>;
