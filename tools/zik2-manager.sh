#!/bin/sh
 appname=`basename $0 | sed s,\.sh$,,`
 
 dirname=`dirname ./deploy_qt.sh`
 tmp='${dirname#?}'
 
 if [ "${dirname%$tmp}" != "/" ]; then
 dirname=$PWD/$dirname
 fi
 LD_LIBRARY_PATH=$dirname
 export LD_LIBRARY_PATH
 $dirname/$appname "$@"

