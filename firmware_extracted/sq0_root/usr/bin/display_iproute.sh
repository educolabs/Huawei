#!/bin/sh

eval "ip rule"
a=`ip rule | awk -F"lookup " '{print $2}'| sort -u`
for table in $a
do
	cmd="ip route list table "$table
	echo "#"$cmd
	eval $cmd
	echo ""
done




