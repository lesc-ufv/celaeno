#!/bin/bash

set -e

DIR_SCRIPT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

DIR_PROJECT="$(dirname "$(dirname "$DIR_SCRIPT")")"

cd "$DIR_PROJECT"

# Compile the project
docker build . --platform=linux/amd64/v3 -t celaeno -f docker/Dockerfile.applets --build-arg="CELAENO_DIR=$(pwd)"

# Copy the built binary
docker run -it --rm -v "$(pwd)":/host celaeno cp -r ./build /host

# Setup compile_commands and conan libraries
docker run -it --rm \
  -e DIR_PROJECT="$DIR_PROJECT" \
  -v "$(pwd)":/host \
  celaeno /host/applet/compile/impl_compile.sh
