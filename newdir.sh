#!/bin/bash

TMP="Makefile.template"
NEW=$1

if [ "$#" = "1" ]
then
    mkdir "parte_$NEW"
    mkdir "parte_$NEW/test"
    cp "$TMP" "parte_$NEW/Makefile"
else
    echo "Usage: $0 <dir>"
    echo "Prepares a parte_dir directory"
fi

