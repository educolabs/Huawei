#! /bin/sh  
#arping

if [ $# -eq 0 ];
then
    echo "ERROR::input para is not right!"
exit 1
fi

for param in "$@"
do
    if [ "$param" == "-i" ] || [ "$param" == "-I" ];
    then
        arping $*
        exit $?
    fi
done

echo "ERROR::input para must -I"
exit 1
