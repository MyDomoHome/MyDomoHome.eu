#!/bin/bash 
#
#Volet script
#
# $1 => id Digital Output number for up
# $2 => id Digital Output number for down
# $3 => satus up or down  or stop
       
if [ -n "$1" ]; then 
  if [ -n "$2" ]; then
	if [ "$3" = "up" ]; then
		  wget http://192.168.10.177/a?command=$2off tmp
		  wget http://192.168.10.177/a?command=$1on tmp
		  exit 
	fi
	if [ "$3" = "down" ]; then
		  wget http://192.168.10.177/a?command=$1off tmp
		  wget http://192.168.10.177/a?command=$2on tmp
		  exit 
	fi
	if [ "$3" = "stop" ]; then
		  wget http://192.168.10.177/a?command=$1off tmp
		  wget http://192.168.10.177/a?command=$2off tmp
		  exit 
	fi
  fi
fi

