#!/bin/bash
script_dir=$(pwd)

echo $script_dir

cd $script_dir/docx/
sudo ./build_docx.sh

cd $script_dir
chmod -R 0755 ./*