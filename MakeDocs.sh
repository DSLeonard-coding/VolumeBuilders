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
rm -rf public
doxygen ./docs/Doxyfile
doxygen ./docs/Doxyfile
cp -arf ./docs/html public
sed -i 's/^\[TOC\]$/<!-- START doctoc generated TOC please keep comment here to allow auto update -->\n<!-- END doctoc generated TOC please keep comment here to allow auto update -->/' README.md d
doctoc README.md
