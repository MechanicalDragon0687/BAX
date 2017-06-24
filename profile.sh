#!/bin/bash

# Hacked together from the Poor Man's Profiler and FlameGraph's GDB Stack Trace
# Requires GDB and Perl

SAMPLES=250
PERIOD=0
GDB=arm-none-eabi-gdb

if [ $# -ne 1 ]; 
    then exit
fi

QEMU_EXEC=./qemu-ctr
QEMU_FLAGS="-M ctr9 -kernel $1 -s"

FLAMEGRAPH_DIR=/home/wolfvak/FlameGraph

$QEMU_EXEC $QEMU_FLAGS &
QEMU_PID=$!

for x in $(seq 1 $SAMPLES)
  do
    $GDB -ex "file build/BAX.elf" -ex "target remote 127.0.0.1:1234" \
                      -ex "set pagination 0" -ex "thread apply all bt" -batch
    sleep $PERIOD
    (>&2 printf "%d/%d\r" $x $SAMPLES)
  done | \
  $FLAMEGRAPH_DIR/stackcollapse-gdb.pl | $FLAMEGRAPH_DIR/flamegraph.pl > profile.svg

kill $QEMU_PID
