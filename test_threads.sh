#!/bin/bash
#for nw in {1..256..32}
for (( nw=1; nw<=256; nw*2 ))
do
 ./threads_pool  200 $nw 128 100000
done
