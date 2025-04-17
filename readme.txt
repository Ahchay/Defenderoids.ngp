DEFENDEROIDS
------------

Not so much a Shoot'em Up as a Mash'em Up...

A little bit of Defender, a little bit of Asteroids and a touch of Qix, Galaga and Lemmings for good measure.

Really just a playground to see how far I can push the NGPC bitmap modes

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

