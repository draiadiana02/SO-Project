#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Mod de utilizare: $0 <caracter>"
    exit 1
fi


char="$1"
counter=0
regex="^[A-Z][a-zA-Z0-9 ,]*[$char][a-zA-Z0-9 ,]*[.!?]{1}$"

while read -r line; do
    length=${#line}
    if [ $length -eq 0 ]; then
        break
    fi
    if echo "$line" | grep -E -q "$regex" && 
        echo "$line" | grep -E -q -v ",[ ]*si"; then
        ((counter++))
    fi
done
#done < "fisier_testscript.txt"

echo "Numarul de propozitii corecte cu caracterul '$char': $counter"
