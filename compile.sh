#!/bin/bash
g++ -c main.cpp
g++ main.o -o bois -lsfml-graphics -lsfml-window -lsfml-system
./bois
