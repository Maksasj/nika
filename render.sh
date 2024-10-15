#!/bin/bash

gcc example0.c -O3 -lm && ./a.out 

for i in *.ppm;
  do name=`echo "$i" | cut -d'.' -f1`
  echo "$name"
  ffmpeg -y -i "$i" "${name}.png"
done