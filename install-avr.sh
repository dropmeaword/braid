#!/bin/bash

if [ $EUID != 0 ]; then
        echo "this script must be run using sudo"
        echo ""
        echo "usage:"
        echo "sudo ./install_dependencies.sh"
        exit $exit_code
   exit 1
fi


apt-get install gcc-avr avr-libc

if command -v foo >/dev/null 2>&1; then
	pip install pyserial
else
	echo >&2 "I require python 'pip' but it's not installed. Fail."; exit 1;
fi

