#!/bin/bash

rm -f output.o game.rom game.nes
wla-6502 output.s
wlalink linkfile game.rom
cat header.bin game.rom > game.nes
