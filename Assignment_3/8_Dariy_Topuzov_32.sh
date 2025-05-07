#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 dir1 dir2"
  exit 1
fi

dir1="$1"
dir2="$2"

if [ ! -d "$dir1" ] || [ ! -d "$dir2" ]; then
  echo "Both arguments must be existing directories."
  exit 2
fi

files=($(find "$dir1" "$dir2" -maxdepth 1 -type f -name "*.c"))

count=${#files[@]}

if [ "$count" -gt 3 ]; then
  > sources.txt
  for f in "${files[@]}"; do
    realpath "$f" >> sources.txt
  done
  chmod 444 sources.txt
  echo "sources.txt created with $count .c files."
else
  echo "Total number of .c files: $count (not enough to create sources.txt)"
fi
