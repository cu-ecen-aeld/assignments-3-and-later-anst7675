#!/bin/sh
# finder script for assignment 1
# Author: Anthony Stewart

if [ $# -ne 2 ] #check if there were 2 arguments passed to the program
  then
    echo "Failed to provide the 2 arguments needed to execute $0."
    echo "Example: finder.sh /tmp/aesd/assignment1 linux"
    exit 1
fi

filesdir=$1
searchstr=$2

if [ ! -d $filesdir ] #check if the provided directory exists
  then
    echo "$filesdir does not represent a directory on the filesystem."
    exit 1
fi

#"find $filesdir -type f": searches and prints all files in directory only
#"wc -l": returns only a count of lines from wc command
#"find $filesdir -type f -exec cat {} \;" searches for and cats all files in directory
#"grep $searchstr": identifies only matching substrings from input
echo "The number of files are $(find $filesdir -type f | wc -l) and the number of matching lines are $(find $filesdir -type f -exec cat {} \; | grep $searchstr | wc -l)"
