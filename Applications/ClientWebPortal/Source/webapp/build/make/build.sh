#!/bin/bash
config=$1
directory=$(dirname $(readlink -f $0))
pushd $directory/
pushd $directory/../..
echo $config
if [ "$config" = "clean" ]
then
  rm -rf dist
else
  sudo -u $(logname) rm -rf $directory/../../node_modules/spire-client
  sudo -u $(logname) ln -s $directory/../../../api/dist $directory/../../node_modules/spire-client
  npm run-script build
  rm -rf ../../Application/webapp
  cp -a dist/. ../../Application/webapp/
fi
popd
popd
