#!/usr/bin/env bash

RED='\033[0;31m'
GREEN='\033[0;32m'
UNDERLINE='\033[4m'
BOLD='\033[1m'
NC='\033[0m'

set_time_settings() {
  sudo mount -o remount,rw /
  sudo timedatectl set-timezone "$1"
  sudo timedatectl set-ntp true
  sudo mount -o remount,ro /
}

if [ -d /data/openpilot ]; then
  pushd /data/openpilot
fi

if [ -f /data/openpilot/prebuilt ]; then
  echo -n "0" > /data/params/d/PrebuiltEnable
  sudo rm -f prebuilt
fi

if ping -c 3 8.8.8.8 > /dev/null 2>&1; then
  LANG=$(cat /data/params/d/LanguageSetting)
  if [ "${LANG}" = "main_ko" ]; then
    set_time_settings Asia/Seoul
  elif [ "${LANG}" = "main_en" ]; then
    set_time_settings America/New_York
  fi
  SUBMODULE=$(git config --global submodule.recurse)
  SSL_VERIFY=$(git config --global http.sslVerify)
  BRANCH=$(git rev-parse --abbrev-ref HEAD)
  BRANCH_GONE=$(git branch -vv | grep ': gone]' | awk '{print $1}')
  CURRENT_FETCH=$(git config --get remote.origin.fetch)
  DESIRED_FETCH="+refs/heads/*:refs/remotes/origin/*"

  if [ "$CURRENT_FETCH" != "$DESIRED_FETCH" ]; then
    sed -i "s@$CURRENT_FETCH@${DESIRED_FETCH//\//\\\/}@" .git/config
  fi

  if [ "$SSL_VERIFY" != "false" ]; then
    git config --global http.sslVerify false
    echo "http.ssl verify false"
  fi

  if [ "$SUBMODULE" != "true" ]; then
    git config --global submodule.recurse true
    echo "submodule.recurse true"
  fi

  echo -e "\n${RED}Resetting local changes...${NC}\n"
  git reset --hard HEAD

  echo -e "\n${RED}Fetching latest changes from remote...${NC}\n"
  git fetch --all --prune

  echo -e "\n${RED}Resetting to latest remote commit...${NC}\n"
  git reset --hard origin/$BRANCH

  echo -e "\n${RED}Submodule sync...${NC}\n"
  git submodule sync --recursive

  echo -e "\n${RED}Submodule init and recursive...${NC}\n"
  git submodule update --init --recursive --force

  echo -e "\n${GREEN}Git Fetch and Reset completed!${NC}\n"

  if [ "${BRANCH_GONE}" != "" ]; then
    echo $BRANCH_GONE | xargs git branch -D
  fi

  REMOTE_COMMIT_HASH=$(git ls-remote origin $BRANCH | awk '{print $1}')
  LOCAL_COMMIT_HASH=$(git rev-parse HEAD)

  REMOTE_COMMIT_TIME=$(date -d @"$(git show -s --format=%ct origin/$BRANCH)" '+%Y-%m-%d %H:%M:%S')
  LOCAL_COMMIT_TIME=$(date -d @"$(git show -s --format=%ct HEAD)" '+%Y-%m-%d %H:%M:%S')

  echo -e "  Remote Commit: [ ${GREEN}${BOLD} $REMOTE_COMMIT_HASH ${NC} ] - $REMOTE_COMMIT_TIME"
  echo -e "   Local Commit: [ ${GREEN}${BOLD} $LOCAL_COMMIT_HASH ${NC} ] - $LOCAL_COMMIT_TIME"

  if [ "$REMOTE_COMMIT_HASH" = "$LOCAL_COMMIT_HASH" ]; then
    echo -e "\nCommit is ${GREEN}${BOLD}match${NC}. Proceeding restart...\n"
    exec /data/openpilot/scripts/restart.sh
  else
    echo -e "\nCommit is ${RED}${BOLD}not match${NC}. Skipping restart.\n"
  fi

else
  touch /data/check_network
fi
