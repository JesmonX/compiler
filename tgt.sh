#!/bin/bash
./build/compiler ./tests/lab3/$1.sy ./target/$1.acc
./tests/accipit-linux-x86_64-gnu ./target/$1.acc