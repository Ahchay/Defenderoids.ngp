DEFENDEROIDS
------------

Not so much a Shoot'em Up as a Mash'em Up...

A little bit of Defender, a little bit of Asteroids and a touch of Qix, Galaga and Lemmings for good measure.

Plot:
The Lemmanoids homeworld has been destroyed by the evil Attackeroids

Only you, the last remaining Defenderoid can save them!

The last Lemmanoids have crash-landed on an inhospitible alien world, bombarded by fragments of the Lemmanoid homeworld.

It is up to you to mine the fragmented Lemmanoid homeworld for the mysterious Pictcells so they can rebuild their cities and escape the clutches of the evil Attackeroids

Gameplay:

Basic movement is Asteroids-y, scrolling horizontally over a mountainous terrain

Player is constrained vertically within a single screen Height

Our Lemmanoids will bob along the mountain tops until they reach the city.

Note: this may make life too easy so need some mechanism for the player to influence their behaviour to "go home"

Invaders will warp in and try to catch the Lemmanoids. Shoot them and the Lemmonoid will whip out their umbrella and float gracefully back to earth

Shoot asteroids to split them up into smaller and smaller parts until they are destroyed completely.

Idea: City builder... Mine the asteroids for City Parts! 
Three different coloured asteroids (my bitmap is Red/White/Blue, so Iron/Rock/Glass or something?) 
Collect $foo of each for one city.
Lemmanoids will collect the parts as they are dropped and schlep them to the city site.

Enemies:
Invaders: Your classic abducting behaviour - they warp in and try and kidnap the Lemmanoids
Bomber: They will attack a city - when the city runs out of resources it will be destroyed.
Qix: The Witch-Space enemy. If all Lemmanoids are destroyed, the landscape will warp up and become the Qix. Destroy one segment at a time

Lemmanoids:
Walker: They start the game aimlessly wandering along the landscape
Carrier: When you completely destroy an asteroid, the resource will float down to the ground, any passing Lemmanoid will pick it up and carry it to the building Sprite
Builder: When it arrives it will go into build mode and add the resource to the city
Abductee: When abducted by an invader, Lemmanoid will panic and drop any resources they are holding- Invader speed will halve due to the excess weight
Floater: If you shoot the abducting alien, Lemmanoid will float back to the ground and resume normal behaviour

City:
Will start as an empty building site.
Adding resources to the city will improve it.
For every $foo resources you add, the city can house one Lemmanoid (the last builder I guess)
For every bomb that lands on the city, it will lose one resource. Any Lemmanoids that are no longer safe will be evicted

Level Success:
All invaders destroyed.
Any Lemmanoids in the city will be permanently saved.
Any Lemmanoids not in the city will carry over to the next wave
Each wave will have additional Lemmanoids and start with a new building site

Level failure:
City destroyed, all Lemmanoids captured

Player:
Energy gauge rather than lives
Collisions with asteroids/aliens etc reduces Energy
Collecting resources before a Lemmanoid picks it up increases Energy
Game over when your energy reduces to zero

Game organisation:

We have five different types of object:
-	Playership
-	Shots
-	Mountain
-	Sprites
-	Asteroids

They each have their own co-ordinate systems, so I need something that translates each to the overall game co-ord system (which i haven't really decided on yet)

Everything has to be knocked down to a unified co-ordinate system

Game Co-ordintate:
    Unit: u16
    Width: 65535
    Height: 65535

Game size (pixels):
    W: 512 (current) = 1024 (max)
    H: 112 (current) = 1024 (max)

SPR_PLANE Size:
    W: 256
    H: 256

Bitmap_PLANE:
    W: 144
    H: 112

VectorSrites:
    Player:
        Unit: (u8)Pixels?
        X: Fixed, but relative to Horizontal Offset
        Y: 0-WorldHeight

    Shot:
        Unit: (u8)Pixels?
        X: 0-WorldWidth
        Y: 0-WorldHeight

Mountain:
    Unit: (u8)Pixels
    X - Defined by Horizontal Offset
    Y - Index into HeightMap array

Sprites:
    Unit: (u16)
    X - 0-65535
    Y - 0-65535

VectorObjects:
    Asteroids
        Unit: (u16)
        X - 0-65535
        Y - 0-65535

Ratios/Conversions:

    Game:World ratio:
    Sprite:World ratio:
        W = 65535:1024 = >>6
        H = 65535:128 = >>6

    Sprite:Sprite Plane ratio:
        X: 65535:256 = >>7
        Y: 65535:256 = >>7

    Vector:Screen ratio:
        W = 65535:1024 = >>6
        H = 65535:128 = >>6

    Player/Shot:World (VectorSprite) ratio:
        W/H = 1:1

