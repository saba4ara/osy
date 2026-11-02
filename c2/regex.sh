#!/usr/bin/env bash

d="Datum: 2026-09-28"

if [[ $d =~ ([0-9]{4})-([0-9]{2})-([0-9]{2})  ]]; then
	echo "${BASH_REMATCH[3]}.${BASH_REMATCH[2]}.${BASH_REMATCH[1]}"
fi	
