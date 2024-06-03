#!/bin/bash

set -e

DIR_SCRIPT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

DIR_PROJECT="$(dirname "$(dirname "$DIR_SCRIPT")")"

cd "$DIR_PROJECT"

# docker build . -t celaeno -f docker/Dockerfile.applets --build-arg="CELAENO_DIR=$(pwd)"

docker run -it --rm \
  -e DIR_PROJECT="$DIR_PROJECT" \
  -v "$(pwd)":/host \
  celaeno /host/applet/compile_commands/impl_setup_compile_commands.sh
