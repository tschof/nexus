#!/bin/bash
beam_commit="668e125c9d4872acb262f278563916fc749772d5"
if [ ! -d "Beam" ]; then
  git clone https://www.github.com/spiretrading/beam.git Beam
  if [ "$?" == "0" ]; then
    pushd Beam
    git checkout "$beam_commit"
    popd
  else
    rm -rf Beam
    exit_status=1
  fi
fi
if [ -d "Beam" ]; then
  pushd Beam
  if ! git merge-base --is-ancestor "$beam_commit" HEAD; then
    git checkout master
    git pull
    git checkout "$beam_commit"
  fi
  popd
fi
