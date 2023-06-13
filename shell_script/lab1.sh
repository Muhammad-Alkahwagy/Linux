#!/bin/bash

#lab1 shell script 

read -p "Enter your name: " USER_NAME
echo "Hello $USER_NAME"
echo Creating ${USER_NAME}_file ...
touch ${USER_NAME}_file

read -p "Enter your age: " AGE
read -p "Enter your address: " ADDRESS

