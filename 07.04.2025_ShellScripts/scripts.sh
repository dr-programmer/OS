#! /bin/bash

echo "Hello, World!"

sum=0
for i in {1..100}
do
    sum=$((sum + i))
done

echo "Sum of numbers from 1 to 100 is $sum"