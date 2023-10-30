#!/bin/bash
if [ -z "$1" ]
then
    echo Usage: clone_project_from_template [PROJECTNAME]
    exit
fi
echo Copying template directory to \'$1\'
cp -a ./template/. ./$1/
cd $1
echo Changing all template name references in project files to \'$1\'
sed -i "s/template/$1/g" template.linkcmd
sed -i "s/template/$1/g" template.mak
sed -i "s/template/$1/g" template.zdsproj
echo Changing project filenames
mv template.zdsproj $1.zdsproj
mv template.mak $1.mak
mv template.linkcmd $1.linkcmd
mv template.wsp $1.wsp
cd ..
echo Done
