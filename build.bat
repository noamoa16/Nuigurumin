@echo off
g++ -std=c++17 -O3 -c main.cpp
g++ -std=c++17 -O3 -c Timer.cpp
g++ -std=c++17 -O3 -o nuigurumin.exe main.o Timer.o