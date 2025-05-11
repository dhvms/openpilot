#!/usr/bin/env bash

LOG=${1}

TODAY=$(date +%y-%m-%d-%H:%M)
CAR=$(cat /data/params/d/CarName)
ID=$(cat /data/params/d/DongleId)
FTP_CREDENTIALS="openpilot:ruF3~Dt8"
FTP_HOST="jmtechn.com:8022"

if [ -f /data/${LOG} ]; then
  if ping -c 3 8.8.8.8 > /dev/null 2>&1; then
    curl -T /data/${LOG} -u "${FTP_CREDENTIALS}" ftp://${FTP_HOST}/tmux_log/${TODAY}_${CAR}_${ID}_${LOG}
  fi
fi
