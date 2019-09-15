#!/bin/sh

for i in $*; do
  echo $i
  ffprobe -show_format $i 2>/dev/null  | grep  ^TAG
  echo ""
done
