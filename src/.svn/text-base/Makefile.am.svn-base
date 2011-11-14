bin_PROGRAMS = retrace
retrace_SOURCES = bitmap.cpp camera.cpp sdl.cpp geometry.cpp \
	main.cpp matrix.cpp shading.cpp

# set the include path found by configure
AM_CPPFLAGS =  $(LIBSDL_CFLAGS) $(all_includes)

# the library search path.
retrace_LDFLAGS = $(all_libraries)
retrace_LDADD = $(LIBSDL_LIBS)
noinst_HEADERS = bitmap.h camera.h color.h constants.h sdl.h \
	geometry.h matrix.h shading.h util.h \
	vector.h
