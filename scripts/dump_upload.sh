#!/usr/bin/env bash

SERVICES=${1}

TODAY=$(date +%y-%m-%d-%H:%M)
CAR=$(cat /data/params/d/CarName)
ID=$(cat /data/params/d/DongleId)
FTP_CREDENTIALS="openpilot:ruF3~Dt8"
FTP_HOST="jmtechn.com:8022"

python3 /data/openpilot/selfdrive/debug/dump.py ${SERVICES} -c 5 -o /data/${SERVICES}.log

if ping -c 3 8.8.8.8 > /dev/null 2>&1; then
  if [ -f /data/${SERVICES}.log ]; then
    curl -T /data/${SERVICES}.log -u "${FTP_CREDENTIALS}" ftp://${FTP_HOST}/tmux_log/${TODAY}_${CAR}_${ID}_${SERVICES}.log
  fi
fi
