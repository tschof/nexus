#!/bin/bash
# stop_server.sh
# 
# Use this script to stop a client_web_portal.
#
# Usage: ./stop_server.sh    Stops the client_web_portal.

reset=$(tput sgr0)
red=$(tput setaf 1)
green=$(tput setaf 2)
yellow=$(tput setaf 3)
echo
# client_web_portal is present.
processes=$(ps -ef | grep -i "client_web_portal" | grep -v "grep" | grep -v "bash" | awk '{ print $8 }')
check_run=$(awk -v a="$processes" -v b="client_web_portal" 'BEGIN { print index(a, b) }')
if [ "$check_run" = "0" ]; then
  # client_web_portal is not running.
  echo "${red}[ERROR]${reset} Could not terminate ${yellow}client_web_portal${reset}."
  echo "        ${yellow}client_web_portal${reset} is not active."
else
  # client_web_portal is already running.
  pid=$(ps -ef | grep -i "client_web_portal" | grep -v "grep" | grep -v "bash" | awk '{ print $2 }')
  kill -s INT $pid
  echo "${yellow}client_web_portal${reset} terminated."
  srv_logs=$(ls srv_*.log 2>/dev/null)
  if [ -n "srv_logs" ]; then
    if [ ! -d "logs" ]; then
      mkdir logs
    fi
    for var in $srv_logs; do
      mv $var logs
    done
  else
    echo "${red}[ERROR]${reset} No log file could be found."
  fi
fi
echo
