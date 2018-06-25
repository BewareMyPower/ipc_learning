#!/bin/bash
# show_mq_info.sh
if test $# -lt 1
then
    echo "usage: ./show_mq_info.sh mq-name"
    exit 1
fi

mq_path='/dev/mqueue'$1
cat $mq_path
