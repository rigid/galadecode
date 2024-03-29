# configure.ac 
# v0.3 - Daniel Hiepler (2010-2012)

# --------------------------------
#    initialization
# --------------------------------

AC_INIT([galadecode], m4_esyscmd([./version.sh --short]),[http://github.com/rigid/galadecode/issues], [galadecode], [http://github.com/rigid//AC_PACKAGE_NAME])
AC_COPYRIGHT([Copyright (C) 2012 Daniel Hiepler])

AM_INIT_AUTOMAKE([color-tests dist-bzip2 parallel-tests silent-rules -Wall])
AC_CONFIG_MACRO_DIR([m4])


# --------------------------------
#    information on the package
# --------------------------------
# infos
PACKAGE_DESCRIPTION="CLI utility to decode raw GALA signal"
AC_SUBST([PACKAGE_DESCRIPTION], [${PACKAGE_DESCRIPTION}], [package description])
AC_DEFINE_UNQUOTED(PACKAGE_DESCRIPTION, "${PACKAGE_DESCRIPTION}", [package description])

# versioning (s. version.sh)
AC_SUBST([PACKAGE_MAJOR_VERSION], m4_esyscmd([./version.sh --major]), [major version])
AC_SUBST([PACKAGE_MINOR_VERSION], m4_esyscmd([./version.sh --minor]), [minor version])
AC_SUBST([PACKAGE_MICRO_VERSION], m4_esyscmd([./version.sh --micro]), [micro version])
AC_SUBST([PACKAGE_GIT_VERSION], m4_esyscmd([./version.sh --git]), [git version])


# --------------------------------
#    checks for programs
# --------------------------------
AC_PROG_CC
AM_PROG_CC_C_O

PKG_PROG_PKG_CONFIG([0.14])
if test -z "$PKG_CONFIG"; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/ or check your distribution.])
fi


# --------------------------------
#    checks for libraries
# --------------------------------

AC_SEARCH_LIBS([cposix])


# --------------------------------
#    checks for header files
# --------------------------------
AC_HEADER_STDC


# --------------------------------
#    checks for types
# --------------------------------


# --------------------------------
#    checks for structures
# --------------------------------


# --------------------------------
#    checks for compiler characteristics
# --------------------------------


# --------------------------------
#    checks for library functions
# --------------------------------


# --------------------------------
#    checks for system services
# --------------------------------


# --------------------------------
#   compile process
# --------------------------------
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


# --------------------------------
# Arguments for configure-script
# --------------------------------
AC_ARG_ENABLE(
        debug,
                [  --disable-debug         Don't compile for debugging ],
                [ debug=false ],
                [ debug=true])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)



# --------------------------------
# Output
# --------------------------------
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
[
	Makefile
	src/version.c
	src/Makefile 
])
AC_OUTPUT


echo "
Configure report:

    Package.....................:  ${PACKAGE_STRING}
    Description.................:  ${PACKAGE_DESCRIPTION}
    Revision....................:  ${PACKAGE_GIT_VERSION}   
    URL.........................:  ${PACKAGE_URL}
    Bugreports..................:  ${PACKAGE_BUGREPORT}

    Install prefix..............:  ${prefix}
    C compiler..................:  ${CC}
    C++ compiler................:  ${CPP}
    System CFLAGS...............:  ${CFLAGS}
    System CXXFLAGS.............:  ${CXXFLAGS}
    System LDFLAGS..............:  ${LDFLAGS}
"
