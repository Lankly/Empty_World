# Empty World

A roguelike written in pure c!

This was originally a project to teach myself c. Now it's a project to keep myself coding.

##Features
###Targeted Combat
The player can target specific body parts of creatures in the game. If no specific part is targeted, one is selected at random. This allows combat to be a lot more strategic. Since creatures won't die until a vital part is damaged thoroughly enough, there is no single HP bar. This makes armor very important in the game as well.

###Unique Maps
Maps are generated with a unique (as far as I can tell) algorithm. Instead of corridors then rooms, I spawn rooms then corridors. Additional rooms decrease in size on average.

###Macros
The player is allowed to create macros recording whatever they do in the game until the macro key is pressed again. The idea is to make repetative actions not a hassle.

##Caveats
###Not Guaranteed Stable
Game may crash randomly since I can't really thoroughly test the game by myself. Please, if the game crashes, please let me know what caused it. The game may also quit with a message that something happened that it couldn't handle. If this happens, please let me know what the message was.

###Memory Leaks Everywhere
I have not gotten around to really taking care of memory problems in the game. I will get around to it eventually, but I'm focusing on gameplay more right now.

###No Way to Win
There is currently no way to win, and no creatures more than a few floors down.
