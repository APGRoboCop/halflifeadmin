#! /bin/sh

bindir=./compile

if [ ! -d ${bindir} ] ; then
    mkdir $bindir ;
fi


for file in `ls -1 *.sma` ; do
	amx=`echo $file | sed 's/\.sma/.amx/'`
    ../compiler/sc -i../include -o${bindir}/$amx $file ;
done

