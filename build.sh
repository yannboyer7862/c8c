#!/bin/bash

ninja -C build_rel -t clean
ninja -C build_rel -j1
cp build_rel/c8c .
