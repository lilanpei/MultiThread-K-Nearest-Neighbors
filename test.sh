#!/bin/bash
for bash in $(ls ./test_*.sh)
    do
        echo "==================== bash: $bash ===================="
        ./$bash > log.txt
    done