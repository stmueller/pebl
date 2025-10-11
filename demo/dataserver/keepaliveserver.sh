#!/bin/bash
##This should be run with nohup.
#PIDFILE=/tmp/pebldataserver.pid
#SCRIPT=~master/dataserver/fileserver.pbl
#echo 'Checking if our monitor script is active'

cont=true
while $cont
do
    ./server2.py
done
