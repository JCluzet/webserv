#!/bin/bash
read -p "Do you want to replace default.conf? (y/n) " -n 1 -r
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo ""
    echo "Replacing default.conf..."
realpath=$(realpath $0);newrealpath=${realpath%/*};cat config/.conf | sed "s|/home/parallels/Desktop/webserv|$newrealpath|g" > config/default.conf;echo "path_replace: $newrealpath"
    echo "Done ✅"
else
    echo ""
    echo "Not replacing default.conf"
fi
