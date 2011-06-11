#!/bin/sh

cd ../src

make 

mkdir ../concept/out_robo

for i in ../concept/img/* ; do
	./main $i ../concept/out_robo/`basename $i`
done
