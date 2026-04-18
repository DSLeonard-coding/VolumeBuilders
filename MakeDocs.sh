#two-pass to get dot file path translation
# replace doctoc with doxygen md TOC marker
sed -i '/<!-- START doctoc generated TOC please keep comment here to allow auto update -->/,/<!-- END doctoc generated TOC please keep comment here to allow auto update -->/{
  /<!-- START doctoc generated TOC please keep comment here to allow auto update -->/{
    s/.*/[TOC]/
    b
  }
  d
}' README.md
rm ./docs/html -rf
doxygen ./docs/Doxyfile
doxygen ./docs/Doxyfile
rm -rf public
cp -arf ./docs/html public
sed -i 's/^\[TOC\]$/<!-- START doctoc generated TOC please keep comment here to allow auto update -->\n<!-- END doctoc generated TOC please keep comment here to allow auto update -->/' README.md d
doctoc README.md
