#!/bin/bash

DIR="guilherme_vieira_gabriel_isomura_victor_araujo"
TMP="test"
BIN=$1

if [ "$#" = "1" ]
then
    rm -rf "$DIR"
    rm -f "$DIR.tar.gz"

    for i in $(ls)
    do
        if [ -e "$i/$BIN"* ]
        then
            mkdir "$DIR"
            for j in $(ls "$i")
            do
                if [ "$j" \!= "$TMP" ]
                then
                    cp -R "$i/$j" "$DIR"
                fi
            done
            tar -czf "$DIR.tar.gz" "$DIR"

            rm -rf "$DIR"
            echo "Source for $1 found"
            exit 0
        fi
    done

    echo "Source for $1 not found"
    exit 1
else
    echo "Usage: $0 <binary>"
    echo "Creates a .tar.gz containing the source code for the given binary"
fi

