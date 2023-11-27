#/bin/bash

if [ $# -le 2 ]; then
    echo "USAGE: ./test.sh <command> <source> <dest>";
    exit 0;
fi

SAMPLES=5;
COMMAND=$1;
SRC=$2;
DST=$3;

for i in $(seq 1 1 $SAMPLES)
do
    rm -r $DST;
    sync;
    sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches';
    sleep 1;
    time -v sh -c "$COMMAND $SRC $DST";
    diff -rq $SRC $DST;
    if [ $? -ne 0 ]
    then
        echo "Copy failed";
        exit 1;
    fi
done
