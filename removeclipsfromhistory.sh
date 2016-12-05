#!/bin/bash

# Removes the complete history of binary files which once were a part of this repository but increased the size of the history massively.

files=( "Alexa,-Prinzessin-von-Wales.jpg" "Frosch.jpg" "burn.jpg" "hexe.jpg" "marry.jpg" "riese.jpg" "ritter_kuniberg_reitet_2.jpg" "siege.jpg" "torture.png" "train" "crazy" "gate" "boar" "castle" "cave" "caveman" "chess" "climbing" "fairy" "jedi" "kill" "mahler" "maninthebox" "music" "opajott" "popcorn" "princess" "prison" "robery" "steampunk" "throne" "tower" "trash" "treasure" "vampir" "well" "werwolf" )

for i in "${files[@]}"
do
	echo "$i"
	git filter-branch -f --index-filter \
	"git rm -r --cached --ignore-unmatch ./clips/$i"
done

git diff master@{1}
