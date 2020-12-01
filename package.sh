#!/usr/bin/env bash
set -euo pipefail

rm ./game_package/ip.txt
rm ./game_package/game
rm -r ./game_package/assets/*



cp -R ./assets/* ./game_package/assets/
cp ./game ./game_package

cp ./ip.txt ./game_package/

zip -r game.zip game_package
