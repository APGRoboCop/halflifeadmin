#!/bin/bash

admininc="../../scripting/include/admin.inc"

if [ x$1 = x ]; then
	echo "No resource file specified. Exiting";
	exit;
fi

filename=$1

if [ x$2 = x ]; then
	echo "No version number specified. Exiting."
	exit;
fi

version=$2

if [ x$3 = x ]; then
	echo "No MS version number specified. Exiting.";
	exit;
fi

msversion=$3

echo "Setting version $version ($msversion) in resource file $filename.";

sed "s/\(#define AM_VERSION\).*/\1 $version/" $filename | sed "s/\(#define RC_VERSION\).*/\1 \"$version\"/" | sed "s/\(#define RC_VERS_DWORD\).*/\1 $msversion/" > /tmp/res.$$
cp /tmp/res.$$ $filename

smaversion=`echo $version | sed -e 's/[^0-9]//g' `

echo "Setting version $smaversion in admin.inc file"

sed "s/\(#define ADMINMOD_VERSION\).*/\1 $smaversion/" $admininc > /tmp/res.$$
cp /tmp/res.$$ $admininc


echo "Done."
