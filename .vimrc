set path=hcsrc/**

command! Tags !ctags -R libgui/* libsrc/*
command! Ninja :wa|!ninja -C ../../build/current
command! Make1 !cd libgui/qtgui; make;

command! Doxy !doxygen Doxyfile



