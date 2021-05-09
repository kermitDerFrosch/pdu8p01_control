#!/bin/bash

HOST=192.168.0.100
USER=admin
PASS=admin

declare -a OUTLETS
declare OP
declare STATUS

STATUS=0

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
    STATUS)
      STATUS=1
      ;;
    *)
      HELP=1
      ;;
  esac

  shift
done
if [ ! -z ${HELP+x} ];then
  echo "USAGE: $0 <on|off|status> <port0..7> ..."
  exit 1
fi
if [ $STATUS -eq 1 ];then
    wget -o /dev/null -O /tmp/pdustatus.xml "http://$HOST/status.xml"
    for i in cur0 stat0 curBan tempBan humBan statBan outletStat0 outletStat1 outletStat2 outletStat3 outletStat4 outletStat5 outletStat6 outletStat7
    do
      eval $i='($(grep $i /tmp/pdustatus.xml | sed -r "s/<(|\/)$i>//g"))'
      eval $i=${!i//[$'\t\r\n']}

    done
    echo "Power: $cur0/$curBan $stat0 ${tempBan}°C ${humBan}% $statBan"
    echo "Outlets: 0:$outletStat0 1:$outletStat1 2:$outletStat2 3:$outletStat3 4:$outletStat4 5:$outletStat5 6:$outletStat6 7:$outletStat7";

fi
if [ ! -z ${OUTLETS+x} ];then
    wget -o /dev/null -O /dev/null "http://$USER:$PASS@$HOST/control_outlet.htm?$KRAMS&op=$OP&submit=Apply"
fi
