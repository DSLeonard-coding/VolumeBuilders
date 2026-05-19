#!/bin/bash

Files=(
  README.md
  GeoModules/GeoModulesREADME.md
)

prepare() {
# Prepare files for Doxygen (Replace doctoc with [TOC])
for file in "${Files[@]}"; do
  sed -i '/<!-- START doctoc generated TOC please keep comment here to allow auto update -->/,/<!-- END doctoc generated TOC please keep comment here to allow auto update -->/{
    /<!-- START doctoc generated TOC please keep comment here to allow auto update -->/{
      s/.*/[TOC]/
      b
    }
    d
  }' "$file"
done
}

do_doxygen (){
#  Run Doxygen (Build the documentation once)
rm -rf ./docs/html
doxygen ./docs/Doxyfile
doxygen ./docs/Doxyfile # Second pass for dot graph translations

#  Cleanup and Restore
rm -rf public
cp -arf ./docs/html public
}

restore() {
for file in "${Files[@]}"; do
    # Restore the doctoc markers
    sed -i 's/^\[TOC\]$/<!-- START doctoc generated TOC please keep comment here to allow auto update -->\n<!-- END doctoc generated TOC please keep comment here to allow auto update -->/' "$file"
    # Update the actual TOC content
    doctoc "$file"
done
}

prepare
do_doxygen
restore
