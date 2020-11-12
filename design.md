# Gomoku design notes

# Goals

- Support local play: turn by turn (first white or black and then one after anohter)
- Support network play: P2P?


## Game play

- randomly picks which player gets which stone (black or white)
- the one with black stone starts 

## Turn
- always starts with black
- while one color is playing other color needs to wait
- check for winner at the end of each turn (naive?)
  - could add condition to check only when there's a possibility of winning but it only saves running the check in the first few turns so it might not be worth the extra work for this

# TODO

- implement turn
- implement win check
- implement network playing


# Note about Gomoku narabe (五目並べ)

Note that Gomoku narabe doesn't have any rule that forbids certain hands. This makes it possible for the first player to always win. 
https://ja.wikipedia.org/wiki/%E4%BA%94%E7%9B%AE%E4%B8%A6%E3%81%B9

There is a version of this with some rules to balance this (連珠) https://ja.wikipedia.org/wiki/%E9%80%A3%E7%8F%A0


# TBD
- players
- stats
- 




