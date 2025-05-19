#!/usr/bin/env bash

LOG_FOLDER="$1"
LOG_FOLDER_NAME=$(basename "$LOG_FOLDER")

TODAY=$(date +%y-%m-%d-%H:%M)
CAR=$(cat /data/params/d/CarName)
ID=$(cat /data/params/d/DongleId)
FTP_USER="openpilot"
FTP_PASSWORD="ruF3~Dt8"
FTP_HOST="jmtechn.com"
FTP_PORT="8022"

echo "$(date) - Starting upload: ${LOG_FOLDER}"

if ! ping -c 3 8.8.8.8 > /dev/null 2>&1; then
  echo "$(date) - Network connection failed" >&2
  exit 1
fi

ftp -n << EOF
open $FTP_HOST $FTP_PORT
user $FTP_USER $FTP_PASSWORD
mkdir /tmux_log/${LOG_FOLDER_NAME}
mkdir /tmux_log/${LOG_FOLDER_NAME}/${TODAY}_${CAR}_${ID}
bye
EOF

upload_file() {
  local filename="$1"
  local remote_path="/tmux_log/${LOG_FOLDER_NAME}/$2"
  curl -v -T "$filename" -u "$FTP_USER:$FTP_PASSWORD" "ftp://${FTP_HOST}:${FTP_PORT}${remote_path}"
    if [ $? -ne 0 ]; then
        echo "$(date) - Failed to upload $2" >&2
        exit 2
    fi
}

upload_file "${LOG_FOLDER}/qcamera.ts" "qcamera.ts"
upload_file "${LOG_FOLDER}/rlog.zst" "rlog.zst"
upload_file "${LOG_FOLDER}/qlog.zst" "qlog.zst"

echo "$(date) - Upload complete"
exit 0
