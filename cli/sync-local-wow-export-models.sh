#!/bin/sh

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
destination="$script_dir/../resources/objects/wow.export"
printf 'Copying WoW export files into %s\n' "$destination"
mkdir -p "$destination"

for folder in creature world; do
	source="$HOME/wow.export/$folder"
	if [ -d "$source" ]; then
		printf 'Copying %s to %s\n' "$source" "$destination"
		cp -a "$source/." "$destination/"
	else
		printf 'Skipping missing folder: %s\n' "$source"
	fi
done

printf 'WoW export copy finished.\n'
