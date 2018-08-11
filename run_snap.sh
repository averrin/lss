#!/usr/bin/env bash

export XLOCALEDIR=$RUNTIME/usr/share/X11/locale

# Mesa Libs for OpenGL support
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RUNTIME/usr/lib/$ARCH/mesa
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RUNTIME/usr/lib/$ARCH/mesa-egl
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RUNTIME/usr/lib/$ARCH/dri

SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`
$SCRIPTPATH/LSS
