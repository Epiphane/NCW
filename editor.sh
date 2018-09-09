#!/bin/bash
cd "$(dirname "$0")"

export EDITORPATH="$(pwd)/build/source/Editor/Editor"
echo Running $EDITORPATH...

$EDITORPATH