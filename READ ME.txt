Name: Yuan Zhi Lin
Userid: yzlin

Objective: stop monsters going from top left to bottom right for 25 waves.

How to play: Click on one of the three tower icons on the top left part of the screen then click on any tile on the grid to place the tower. Click on a placed tower will select it and allows you to upgrade or sell the tower using hotkeys w or s. Right click or clicking on empty space will cancel selection.

Game Speed Setting: click on 1x, 2x or 3x on top right part of the screen to changes the game speed.
                    Note that higher speed will use more CPU (game ticks faster) but framerate is the same.
                    
Hotkeys: 1-3 - buy tower 1-3
         w   - upgrade selected tower
         s   - sell selected tower
         p   - pause or resume game
         
Program Mechanics: The program will tick every 2 milliseconds. In between each tick, the program will sleep for 2 milliseconds then wake up and do stuff. When the program wakes up, it handles all events received in the last 2 milliseconds call the tick function of the Floor class. The tick function does all calculations in the program. Every 20 ticks (40 milliseconds), the program will repaint once by calling draw function of the Floor class. This produces 25 FPS for the game. The game speed changes how often the program ticks. A game speed of 2x causes the program to tick every millisecond. However, this will not change the FPS because the program will just paint every 40 ticks (40 milliseconds) for a game speed of 2x. This will make the program recalculate more often and thus result in higher CPU usage. Pause will set the game speed to 0 so the program won't call the tick function but it will still repaint.

Game Mechanics: The game has infinite waves with each wave producing more monsters. Monsters of high number wave will have more HP and higher speed. A new wave will spawn if all of the monsters of the previous wave have been killed or reached the end of the path. Finishing wave 25 will win the game but you can continue playing until you lose all lives. Killing a monster will give you gold and score equal to the wave number. Finishing a wave will also give you 100+wave gold. Tower range is the number of tiles from the tower. Tower rate of fire (ROF) is number of seconds between each shot. Towers will attack the target within range furthest down the path.

Features:
- Supports gameplay with window size 600x360 and up (will scale and center the game if window resizes)
- Game will maintain a specific ratio and will things in game will scale bigger or smaller except for texts.
- Game speed is adjustable
- Monsters have HP bar
- Towers have range indicator when selected
- Buying a tower will have the tower and its range under your mouse pointer
- Upgrade a tower will cause its look to change and update its stats on the left
- Sell a tower will refund 80% of the gold you spent on the tower (include upgrades)
- Blue tower will slow enemies by a percentage for a few seconds. (multiple slows will take the highest slow and refresh the time to the longest slow duration)
- Red tower will attack multiple enemies within range instantly and over time.(damage them as soon as they are within range and keeps damaging them every x ticks while they are in range)
- Cannon balls will chase the monster until it's dead or reached the end of the path
- Cannon balls will disappear if the monster is dead or reached the end of the path
- Tower status and tower upgrade status bar on the left
- Warning and error notification on the bottom