#!/bin/bash
set -e

MODE="$1"

COMMON_SRC="./Main.c ./GateFuncs.c ./BlockCreation.c ./SavestringParse.c ./Util.c"
CFLAGS="-fsanitize=address -g"
LIBS=""
TICKS_SRC=""

case "$MODE" in
    threaded|"")
        TICKS_SRC="./ThreadedTicks.c"
        LIBS="-lpthread"
        ;;
    single)
        TICKS_SRC="./SingleTicks.c"
        ;;
    *)
        echo "Usage: $0 [threaded|single]"
        exit 1
        ;;
esac

gcc -o ./Main \
    $COMMON_SRC \
    $TICKS_SRC \
    $CFLAGS \
    $LIBS

./Main "$(cat ./input.txt)" 1
