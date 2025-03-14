#!/usr/bin/env bash

set -euxo pipefail

# Prepare conda
set +x && eval "$($(which conda) shell.bash hook)" && set -x

# Setup the OS_TYPE environment variable that should be used for conditions involving the OS below.
case $(uname) in
  Linux)
    OS_TYPE=linux
    ;;
  Darwin)
    OS_TYPE=macos
    ;;
  MSYS*)
    OS_TYPE=windows
    ;;
  *)
    echo "Unknown OS type:" $(uname)
    exit 1
    ;;
esac

echo '::group::Create build environment'
conda create \
  --name ci \
  --quiet --yes \
  python="${PYTHON_VERSION}" pip \
  ninja cmake
conda activate ci
pip install --progress-bar=off --upgrade setuptools==72.1.0

if [[ "${PYTHON_VERSION}" != "3.11" ]]; then
  pip install --progress-bar=off av!=10.0.0
fi

echo '::endgroup::'
