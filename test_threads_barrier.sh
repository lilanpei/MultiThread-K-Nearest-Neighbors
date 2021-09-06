#!/bin/bash
for k in 10 100 1000
    do
        echo "==================== k: $k ===================="
        for (( nw=1; nw<=256; nw*2 ))
            do
                echo "==================== N_workers: $nw ===================="
                for _ in {1..5..1}
                    do
                        echo "------------------------ k: $k, N_workers: $nw, N_points: 10k ------------------------ "
                        ./threads_barrier k $nw 10000
                    done
            done
    done

echo "##############################################################################"

for k in 10 100 1000
    do
        echo "==================== k: $k ===================="
        for (( nw=1; nw<=256; nw*2 ))
            do
                echo "==================== N_workers: $nw ===================="
                for _ in {1..5..1}
                    do
                        echo "------------------------ k: $k, N_workers: $nw, N_points: 100k ------------------------ "
                        ./threads_barrier k $nw 100000
                    done
            done
    done