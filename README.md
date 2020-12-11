# About

This is a simple gomoku game that allows both local and network play.
It is made using SFML framework.

For playing over network, note that if the players are on different networks (or over the Internet) and if the host is behind a NAT port forward for port _51781_ on needs to be configured for the router to forward incoming traffic.

# Compile and run

Compilation is tested on Mac OS and Arch Linux. It has not been tested with Windows.

# Credits

The techniques for drawing board, stones and the related event handling is largely borrowed from [sevity's GoGame](https://github.com/sevity/GoGame).

Many of the the fundamental techniques including resource management, state stack, event handling, network is based on the [examples provided by the "SFML Game Development" book](https://github.com/SFML/SFML-Game-Development-Book).

All the voice clips are obtained from freesound.org and used as is without modification. These are the sound clips used with their creator's name (freesound user name) and their license.
- [Greetings (NPC, British Male) by theuncertainman](https://freesound.org/people/theuncertainman/sounds/429038/) licensed under [Creative Commons 0 license](https://creativecommons.org/publicdomain/zero/1.0/)
- [having a laugh.wav by aarongbuk](https://freesound.org/people/aarongbuk/sounds/106972/) licensed under [Creative Commons Attribution Noncommercial License](https://creativecommons.org/licenses/by-nc/3.0/) 
- [ Russian Dialogue: Good Work by trullilulli](https://freesound.org/people/trullilulli/sounds/422643/) licensed under [Creative Commons 0 License](https://creativecommons.org/publicdomain/zero/1.0/)
- [Some people take offense IKYN.wav by trullilulli](https://freesound.org/people/trullilulli/sounds/422643/) licensed under [Creative Commons Attribution License](https://creativecommons.org/licenses/by/3.0/)
