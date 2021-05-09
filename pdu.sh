#!/bin/bash

HOST=192.168.0.100
USER=admin
PASS=admin

declare -a OUTLETS
declare OP

if [ -z ${1+x} ];then
  HELP=1
fi

while [ ! -z ${1+x} ]; do
  case ${1^^} in
    [0-7])
      OUTLETS[${#OUTLETS[@]}]=$1
      if [ -z ${KRAMS+x} ];then
        KRAMS+="outlet$1=1"
      else
        KRAMS+="&outlet$1=1"
      fi
      ;;
    ON)
      OP=0
      ;;
    OFF)
      OP=1
      ;;
    SWITCH)
      OP=2
      ;;
    *)
      HELP=1
      ;;
  esac

  shift
done
if [ ! -z ${HELP+x} ];then
  echo "USAGE: $0 <on|off|switch> <port0..7> ..."
  exit 1
fi

echo wget -o /dev/null -O /dev/null \""http://$USER:$PASS@$HOST/control_outlet.htm?$KRAMS&op=$OP&submit=Apply"\"
wget -o /dev/null -O /dev/null "http://$USER:$PASS@$HOST/control_outlet.htm?$KRAMS&op=$OP&submit=Apply"
