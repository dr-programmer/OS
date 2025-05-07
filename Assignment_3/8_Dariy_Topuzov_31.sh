#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Provide at least one argument"
  exit 1
fi

echo -n "Enter search string: "
read search_string

for file in "$@"; do
  if [ -f "$file" ]; then
    count=$(grep -c "$search_string" "$file")
    echo "$file: $count"
  else
    echo "File '$file' does not exist."
  fi
done
