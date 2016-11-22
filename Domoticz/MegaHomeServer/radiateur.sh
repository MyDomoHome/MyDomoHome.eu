#!/bin/bash 
#
#Radiateur script
#
#utilisation de 2 pin pour envoyer 4ordres
#
# $1 => id Digital Output number 1 
# $2 => id Digital Output number 2
# $3 => satus stop , eco , gel , confort
       
if [ -n "$1" ]; then 
  if [ -n "$2" ]; then
	if [ "$3" = "stop" ]; then
		  wget http://192.168.10.177/a?command=$2off tmp
		  wget http://192.168.10.177/a?command=$1on tmp
		  exit 
	fi
	if [ "$3" = "eco" ]; then
		  wget http://192.168.10.177/a?command=$1on tmp
		  wget http://192.168.10.177/a?command=$2on tmp
		  exit 
	fi
	if [ "$3" = "gel" ]; then
		  wget http://192.168.10.177/a?command=$1off tmp
		  wget http://192.168.10.177/a?command=$2on tmp
		  exit 
	fi
	if [ "$3" = "confort" ]; then
		  wget http://192.168.10.177/a?command=$1off tmp
		  wget http://192.168.10.177/a?command=$2off tmp
		  exit 
	fi
  fi
fi

