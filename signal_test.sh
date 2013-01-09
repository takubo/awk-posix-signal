#!/bin/sh

gawk -f ./sigact_test.awk &
pid=$!
sleep 1
./kill_test.awk $pid
