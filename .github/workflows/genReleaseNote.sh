#!/bin/bash
# From https://github.com/MetaCubeX/mihomo/blob/82517e6ba8059339287911af899ffdffca6a4044/.github/genReleaseNote.sh

while getopts "v:" opt; do
  case $opt in
    v)
      version_range=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

if [ -z "$version_range" ]; then
  echo "Please provide the version range using -v option. Example: ./genReleaseNote.sh -v 1.14.1...1.14.2"
  exit 1
fi

new_commits=$(git log --pretty=format:"* %h %s by @%an" --grep="^feat" -i $version_range | sort -f | uniq)
if [ -n "$new_commits" ]; then
  echo "## New" >> release.md
  echo "$new_commits" >> release.md
  echo "" >> release.md
fi

fix_commits=$(git log --pretty=format:"* %h %s by @%an" --grep="^fix" -i $version_range | sort -f | uniq)
if [ -n "$fix_commits" ]; then
  echo "## Fix" >> release.md
  echo "$fix_commits" >> release.md
  echo "" >> release.md
fi

maint_commits=$(git log --pretty=format:"* %h %s by @%an" --grep="^chore\|^docs\|^refactor" -i $version_range | sort -f | uniq)
if [ -n "$maint_commits" ]; then
  echo "## Maintenance" >> release.md
  echo "$maint_commits" >> release.md
  echo "" >> release.md
fi

echo "**Full Changelog**: https://github.com/KdeInit/chrome_plus/compare/$version_range" >> release.md
