#!/bin/sh

if [ "$#" -ne 3 ]
then
	VERSION_TAG=$(date +%d%m%Y)
else
	VERSION_TAG=$3
fi

LAUNCH_SCRIPT="#!/bin/sh\n
appname=\`basename \$0 | sed s,\.sh$,,\`\n
\n
dirname=\`dirname $0\`\n
tmp='\${dirname#?}'\n
\n
if [ \"\${dirname%\$tmp}\" != \"/\" ]; then\n
dirname=\$PWD/\$dirname\n
fi\n
LD_LIBRARY_PATH=\$dirname\n
export LD_LIBRARY_PATH\n
\$dirname/\$appname \"\$@\"\n
"

QT_CONF_FILE="[Paths]\n
"

rm -rf $2
mkdir $2

mkdir $2/plugins/
cp -r $QTDIR/plugins/* $2/plugins/
cp -r $QTDIR/qml $2/

LD_LIBRARY_PATH="$QTDIR/lib"
LIBS="$(ldd $1 | cut -d = -f 1 | tr -d '\t')"
LIBS+=" libQt5Widgets.so.5 libQt5Svg.so.5"

for i in `ls $2/plugins`; do
	for j in `ls $2/plugins/$i`; do
		LIBS+="$(ldd $2/plugins/$i/$j | cut -d = -f 1 | tr -d '\t')"
	done
done

for i in `ls $2/qml`; do
	for j in `ls $2/qml/$i`; do
		LIBS+="$(ldd $2/qml/$i/$j | cut -d = -f 1 | tr -d '\t')"
	done
done



for i in $LIBS; do
	cp -f "$QTDIR/lib/$i" $2/ 2>/dev/null
done


mkdir $2/bin
cp $QTDIR/bin/sdpscanner $2/bin/

cp $1 $2/
echo -e $LAUNCH_SCRIPT > $2/$(basename $1).sh
chmod +x $2/$(basename $1).sh

echo -e $QT_CONF_FILE > $2/qt.conf


tar cvjpf $(basename $1)-$VERSION_TAG.tar.bz2 $2

