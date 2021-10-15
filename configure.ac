AC_INIT([flamel], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC
AX_CXX_COMPILE_STDCXX(14)
AX_BOOST_BASE

AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([stdlib.h])

AC_CHECK_HEADERS([algorithm])
AC_CHECK_HEADERS([chrono])
AC_CHECK_HEADERS([cmath])
AC_CHECK_HEADERS([fstream])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([iterator])
AC_CHECK_HEADERS([map])
AC_CHECK_HEADERS([memory])
AC_CHECK_HEADERS([numeric])
AC_CHECK_HEADERS([ostream])
AC_CHECK_HEADERS([stdexcept])
AC_CHECK_HEADERS([string])

AC_CHECK_HEADERS([csignal])
AC_CHECK_HEADERS([pstreams/pstream.h])
AC_CHECK_HEADERS([sndfile.hh])
AC_CHECK_HEADERS([sstream])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_ARG_WITH([CAENDigitizer],
  [AS_HELP_STRING([--with-CAENDigitizer=prefix],
    [prefix of CAENDigitizer library])],
  [CAENDigitizerPATHSET=1],
  [CAENDigitizerPATHSET=0])
if test $CAENDigitizerPATHSET = 1 ; then
  AM_CXXFLAGS="$AM_CXXFLAGS -I$with_CAENDigitizer/include"
  AM_LDFLAGS="$AM_LDFLAGS -L$with_CAENDigitizer/lib/x86_64"
fi


AC_ARG_WITH([CAENComm],
  [AS_HELP_STRING([--with-CAENComm=prefix],
    [prefix of CAENComm library])],
  [CAENCommPATHSET=1],
  [CAENCommPATHSET=0])
if test $CAENCommPATHSET = 1 ; then
  AM_CXXFLAGS="$AM_CXXFLAGS -I$with_CAENComm/include"
  AM_LDFLAGS="$AM_LDFLAGS -L$with_CAENComm/lib/x64"
fi

AC_ARG_WITH([CAENVME],
  [AS_HELP_STRING([--with-CAENVME=prefix],
    [prefix of CAENVME library])],
  [CAENVMEPATHSET=1],
  [CAENVMEPATHSET=0])
if test $CAENVMEPATHSET = 1 ; then
  AM_CXXFLAGS="$AM_CXXFLAGS -I$with_CAENVME/include"
  AM_LDFLAGS="$AM_LDFLAGS -L$with_CAENVME/lib/x64"
fi

AC_ARG_WITH([CAEN_FELib],
  [AS_HELP_STRING([--with-CAEN_FELib=prefix],
    [prefix of CAEN_FELib library])],
  [CAEN_FELibPATHSET=1],
  [CAEN_FELibPATHSET=0])
if test $CAEN_FELibPATHSET = 1 ; then
  AM_CXXFLAGS="$AM_CXXFLAGS -I$with_CAEN_FELib/include"
  AM_LDFLAGS="$AM_LDFLAGS -L$with_CAEN_FELib/lib"
fi

AC_CHECK_PROG([rootconfig_var], [root-config], [yes], [no])
if test x$rootconfig_var != "xyes" ; then
  AC_MSG_ERROR([Please install root])
fi
AM_CXXFLAGS="$AM_CXXFLAGS `root-config --cflags`"
AM_LDFLAGS="$AM_LDFLAGS `root-config --glibs`"

AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

LIBS="-lCAENDigitizer -lCAENComm -lCAENVME -lCAEN_FELib -lboost_program_options -lsndfile -g $LIBS"

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
