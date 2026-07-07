#!/bin/sh

function showconf()
{
	echo "#cat "$1
	cat $1
	echo ""
}

function read_file(){
 for line in `cat $1`
    do
        conf=${line##resolv-file=}
		if [ -e $conf ]; then
			if [ "$conf" = "/var/wan/default_data_dns" ];then
			    if [ $def_flag -eq 0 ];then
					showconf $conf
					def_flag=1
				fi
			else
			    showconf $conf
			fi
		   
        fi
    done
}

def_flag=0

echo ""
echo "#cat etc/resolv.conf"
cat etc/resolv.conf
echo "#cat /var/wan/dns_all"
cat /var/wan/dns_all
echo ""

for file in /var/dnsmasq*
do
    echo "#cat "$file
    if [ -r $file ]; then
        cat $file
		echo ""
		read_file $file
    fi
done

for file in /var/dnsv6/dnsmasq*
do
    echo "#cat "$file
    if [ -r $file ]; then
        cat $file
		echo ""
		read_file $file
    fi
done

