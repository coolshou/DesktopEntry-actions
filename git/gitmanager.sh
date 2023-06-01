#!/bin/bash

# git manager for right click
act=$1
files=$2

case $act in
	"add" )
	    # only file name
		args=$#
		files=${@:3-1}
		#echo "num: $args"
		#echo "files: $files"
		for file in $files;
		do
		    p=$(dirname $file)
		    f=$(basename $file)
		    #echo $file;
		    echo "git add $f"
		    cd "$p" && git add "$f"
		done
	;;
	"rm" )
		echo git rm $files
	;;
	*)
	echo "Usage:"
	echo "  $0 <add/rm> <filenames>"
	;;
esac
