#! /bin/sh
#display iptables filter

i=1
CAT_IPTABLES="/var/cat_iptables.log"

if [ 0 -ne $# ];
then
    echo "ERROR::input para is not right!"
    exit 1
else
    Bbspcmd iptables get table filter
    while [  $i  -le  5  ]
    do
        if [ -f $CAT_IPTABLES ]
        then        
            cat /var/cat_iptables.log
            rm -f /var/cat_iptables_bak.log
            rm -f /var/cat_iptables.log
            exit 0
        else
            sleep 1
            i=`expr $i + 1`
        fi

    done   
    exit 0
fi