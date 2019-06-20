
# Use fltk-config script to get compiler/linker options used to build FLTK.
# Additionally, make sure header files and static library can be found.
CXXFLAGS += -g -I /usr/local/include -I ~/src/fltk/test $(shell fltk-config --cxxflags)
# --ldstaticflags specifies static linking
# --use-images required for PNG
LDFLAGS += -lfltk $(shell fltk-config --ldstaticflags --use-images)
