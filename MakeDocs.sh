#two-pass to get dot file path translation
rm ./docs/html -rf
rm -rf public
doxygen ./docs/Doxyfile
doxygen ./docs/Doxyfile
cp -arf ./docs/html public
