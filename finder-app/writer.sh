#!/bin/sh
# writer script for assignment 1
# Author: Anthony Stewart


if [ $# -ne 2 ] #check if there were 2 arguments passed to the program
  then
    echo "Failed to provide the 2 arguments needed to execute $0."
    echo "Example: writer.sh /tmp/aesd/assignment1/sample.txt ios"
    exit 1
fi

writefile=$1
writestr=$2

#"mkdir -p" creates directorys and subdirecotories
#"$(dirname $writefile) 2>/dev/null" extracts the directory without a filename and redirects errors to null file

if ! $(mkdir -p $(dirname $writefile) 2>/dev/null)
  then
    echo "Error unable to write file. Exitting."
    exit 1
fi

echo $writestr > $writefile
