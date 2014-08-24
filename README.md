Hex
===

Ludum Dare #30 Competition Entry
--------------------------------

Adrian Dale 24/08/2014

Ludum Dare is a Game Jam competition. It is run quarterly.
A theme is chosen and participants then have 48hrs to code a game to fit the theme.

For LD#30 the theme was "Connected Worlds"

I therefore chose to go with a "connection" theme and implement the Hex board game,
which involves connecting opposite sides of the playing field with stones of your
colour.

How To Install
--------------

Download the file Hex.zip, either from this github repository, or from this dropbox URL:
https://dl.dropboxusercontent.com/u/21701260/Hex.zip

Unzip the contents of Hex.zip to a convenient place on your hard drive.
An easy way to do this is, when downloading the zip file, choose to open it with Windows Explorer.
Then drag the Hex folder contained inside it onto your Windows Desktop.

You can then double click the folder on your desktop to open it, then double click on Hex.exe to
start the game. You may receive a warning saying "The publisher could not be verified. Are you
sure you want to run this software?". Click Run to continue.

To uninstall, simply delete the Hex folder from your desktop.

How To Play
-----------

The aim of Hex is for you to form a line of stones of your colour connecting opposite sides of the board.
The red player must connect the top and bottom edges before blue manages to connect the left and right edges.

Players take turns to place a stone of their colour in any unoccupied cell.

It is not possible for the game to end in a draw.

You can learn more about the game of Hex here:
http://en.wikipedia.org/wiki/Hex_%28board_game%29

Notes:
------

My goal for this project was to come up with a complete and playable game within the competition time frame.
I wanted to create something from scratch using a minimum amount of supporting framework code
and in a language that I am familiar with, which in this case was C++ and using the SDL2 library.

Due to other commitments I actually had slightly less than the full 48 hours available to me, although
in total I believe I spent around 16-18 hours of development time. I have 14 hours of low fps screen capture
of my desktop, plus another hour or two I spent using Ableton which I couldn't capture as the audio was
conflicting with the capture software, then an hour or so preparing the submission and documentation.
Not sure what I'll do with this recording - I can't imagine it's exactly riveting viewing.

As this game was created in a very short space of time, something had to give quality-wise.

The weakest part being the Single Player AI, which, quite frankly, would be more
appropriately named AU. That's "Artificial Unintelligence". The computer's moves are chosen randomly,
making it a very weak opponent.

However, the two player experience is still playable as a pass and play game, with all moves correctly
verified and game end condition correctly detected.

I'm a little disappointed with how the text graphics look. I created all of the graphics at double size
with a view to porting the game to Android and/or iOS at a later date. Scaling the text images down
seems to have made them a little unclear.

I'd like to have spent some more time in Inkscape giving the graphics more of a shiny look.
That said, I'm pleased with how well all the board pieces fit together and I like the crisp vector
graphics look of the board. I'm more of a programmer than an artist.

I think that the game really needs some movement - perhaps an animated background?

One thing I wish I'd done early on in the development is spent some time creating some kind of component or
entity architecture. I'd thought that the small scale of this project would make that unnecessary, but 
found that a lot of random stuff ended up getting dumped into the HexGame object to the point where
it rapidly became unwieldy.

There are just over 1000 lines of source code in this project, exactly in line with my 500 lines per day rule
of thumb. Not bad, considering I also spent time on graphics, music and sound, too!

Single Player AI was a "stretch goal" right from the off, so I'm not too
worried about missing that. I believe that it would be possible to incorporate a proper AI as an off the shelf
plugin at a later date. I had the code at the URL below in mind, although I didn't use it in this project as I didn't
want to include something that wasn't my own work.
https://github.com/PetterS/monte-carlo-tree-search

In conclusion, I'm happy with my final result. In a relatively short amount of time I have managed to produce
a complete and playable game.

Credits:
--------

All code, graphics, sound and music created by me during the event.

Tools used:
-----------

Microsoft Visual C++ 2013 Express
http://www.microsoft.com/en-gb/download/details.aspx?id=40787

Notepad++
http://notepad-plus-plus.org/

Background generated using Alex C Peterson's Spacescape
http://alexcpeterson.com/spacescape

Graphics created in Inkscape and GIMP
http://www.inkscape.org
http://www.gimp.org

Music and sound created using Ableton with Ableton Push
http://www.ableton.com

Open Broadcaster (for screen capture of development process)
https://obsproject.com/

Libraries used:
---------------

For creating game window and basic rendering:
SDL 2 - https://www.libsdl.org/

For PNG image loading:
SDL_image - https://www.libsdl.org/projects/SDL_image/

For sound playing:
SDL_mixer - http://www.libsdl.org/projects/SDL_mixer/




