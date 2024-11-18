# Overview

This is a small sample of the calculator series repo. The focus was on memory safety and concurrency. NetCalc uses a threadpool to manage file servicing requests from clients

## Build

in the home directory, run ./build.sh

## Run

All created Executables will be put in their respective subfolder within build.
If you want to easily generate unsolved calc files, I recommend running the NetCalcTests.py : python3 calculator-project-netcalc-Tests/Tests/NetCalcTests.py 
This will add files to the netcalc_tests/unsolved folder.

For netcalc, the client is written in python and is located in 5_NetCalc/client/client.py
