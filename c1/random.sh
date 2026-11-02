#!/usr/bin/env bash

count=$1

for (( i = 0; i < count; i++ )) 
do
	echo $RANDOM
done

