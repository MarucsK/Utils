#!/bin/bash

clang-format -i `find include/ -type f -name *.hpp`
clang-format -i `find test/ -type f -name *.cpp`
