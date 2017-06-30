#!/bin/ksh

export PATH=/bin:/usr/bin:/usr/local/bin
export LD_LIBRARY_PATH=/lib:/lib64:/usr/lib:/usr/lib64:/usr/local/lib

print "Building HDB..."
gcc -Wall -lmxml -lpthread -L/usr/loca/lib -o hdb hdb.c

if (( $? == 0 )); then
	print "Running HDB..."
	strace ./hdb 2>&1 | wc
	./testmxml ./hdb.xml
fi

