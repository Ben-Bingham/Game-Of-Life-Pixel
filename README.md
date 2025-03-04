## Game of Life Pixel
A simple GPU accelerated simulation of Conways Game of Life that allows you to draw each cell in the game board as an individual pixel.

![alt text](https://github.com/Ben-Bingham/Game-Of-Life-Pixel/raw/main/gallery/Big%20Glider.png "Glider on a 20 by 20 cell game board.")
Simple glider on a 20 by 20 cell game board.

![alt text](https://github.com/Ben-Bingham/Game-Of-Life-Pixel/raw/main/gallery/Small%20glider.png "Glider on a 1018 by 968 cell game board.")
The same glider (the small white spec in the center of the board) now on a 1018 by 968 cell game board, both this board and the previous one take up the same size in pixels: 1018 by 968 pixels.

#### Features
* Starting and stopping the simulation, and setting a minimum step time while doing this.
* Stepping the simulation one frame at a time.
* You can select the board from one of the default boards, or draw your own by left clicking to place cells, and right clicking to delete
  * ![alt text](https://github.com/Ben-Bingham/Game-Of-Life-Pixel/raw/main/gallery/Custom%20Drawn%20start.png "A 20 by 20 game board that has been customly drawn")
* Fully customize the number number of cells that make up the game board, either by entering a number, or by selecting the "Pixel Perfect Board" option to make each cell exactly one pixel

### Pixel Perfect Board Example
![alt text](https://github.com/Ben-Bingham/Game-Of-Life-Pixel/raw/main/gallery/Random%20start.png "A 2298 by 1723 cell board filled with random cells at a 30% fill rate.")
A 2298 by 1723 cell board filled with random cells at a 30% fill rate, this was created on my laptop with a 2880 by 1800 pixel display.

![alt text](https://github.com/Ben-Bingham/Game-Of-Life-Pixel/raw/main/gallery/Random%20midway.png "A 2298 by 1723 cell board filled with random cells at a 30% fill rate. This board has been simulated for a few seconds.")
This is the same random board as above but after it has been simulated for a second or two at maximum speed. My laptop is able to simulate a frame of this size in about 11 miliseconds with integrated AMD graphics.

Overall the actual simulation is not super demanding, especilly when in comparison to modern GPU power.

