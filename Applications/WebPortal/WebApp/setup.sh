#!/bin/bash
source="${BASH_SOURCE[0]}"
while [ -h "$source" ]; do
  dir="$(cd -P "$(dirname "$source")" >/dev/null 2>&1 && pwd)"
  source="$(readlink "$source")"
  [[ $source != /* ]] && source="$dir/$source"
done
directory="$(cd -P "$(dirname "$source")" >/dev/null 2>&1 && pwd)"
root=$(pwd)
"$directory/../../../WebApi/setup.sh"

dali_commit="0112f990256539cd096c33b178a8ca7ede6ebf43"
if [ ! -d "dali" ]; then
  git clone https://www.github.com/spiretrading/dali
  if [ "$?" == "0" ]; then
    pushd dali
    git checkout "$dali_commit"
    popd
  else
    rm -rf dali
    exit_status=1
  fi
fi
if [ -d "dali" ]; then
  pushd dali
  if ! git merge-base --is-ancestor "$dali_commit" HEAD; then
    git checkout master
    git pull
    git checkout "$dali_commit"
  fi
  popd
fi
if [ ! -d WebApi ]; then
  mkdir WebApi
  pushd WebApi
  "$directory/../../../WebApi/configure.sh" -DD="$root"
  popd
fi
