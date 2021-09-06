#!/bin/bash
for _ in {1..10..1}
do
 ./sequential 200 10000
done
for _ in {1..10..1}
do
 ./sequential 200 100000
done
