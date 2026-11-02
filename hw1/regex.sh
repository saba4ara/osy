#!/usr/bin/env bash

set -euo pipefail

print_help() {
    echo >&2 "Pouziti skriptu:"
    echo >&2 "$(basename "$0") -h | -a | -b | -c | -d <prefix>"
    exit 1
}

while getopts ":habcd:" opt; do
    case $opt in
        h|:|\?)
            print_help
            ;;
    a) ls -1a | grep -i -E '(\.png|\.jpg)$'
           ;;
   b) sed -E -n 's/^(([[:alpha:]])[+-]?[0-9]+\2) *(.*)/\3/p'
           ;;
   c) tr -d '\n' | grep -Eo '\+420( ?[0-9]{3}){3}'
           ;;
        d) prefix=$OPTARG
		sed -E "s|\(file:([^\)]+)\)|\(file:${prefix}\1\)|g"
           ;;
    esac
    exit 0
done
print_help
