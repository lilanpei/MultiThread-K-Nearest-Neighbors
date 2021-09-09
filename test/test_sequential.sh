#!/bin/bash
for k in 10 100 1000
  do
      echo "------------------------ k: $k, N_points: 10k ------------------------ "
      for _ in {1..5..1}
        do
            ./sequential $k 10000
            echo " "
        done
  done

echo "##############################################################################"

for k in 10 100 1000
  do
      echo "------------------------ k: $k, N_points: 100k ------------------------ "
      for _ in {1..5..1}
        do
            ./sequential $k 100000
            echo " "
        done
  done
