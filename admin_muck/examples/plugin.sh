#! /bin/sh

bindir=./compile

if [ ! -d ${bindir} ] ; then
    mkdir $bindir ;
fi

amx=`echo $1 | sed 's/\.sma/.amx/'`

../compiler/sc -i../include -o${bindir}/$amx $1

