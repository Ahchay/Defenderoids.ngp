#ifndef _VECTOROBJECTS
#define _VECTOROBJECTS

// Landscape heightmap
// Generated in Excel using formula =A1+IF(RAND()<0.5,-1,1) so height increases or decreases by one each time
// See HeightMap.xlsx
const u8 HeightMap[] = {
						100,101,100,101,100,99,98,97,96,97,96,97,98,97,96,97,
						96,97,96,95,94,95,94,95,94,93,92,93,92,91,92,91,
						90,91,92,93,94,95,94,93,94,93,92,93,94,93,94,95,
						94,93,94,95,94,95,94,95,94,93,94,93,92,91,90,91,
						92,91,90,89,90,89,88,89,88,87,88,89,90,91,90,91,
						90,89,90,89,90,91,92,93,92,91,92,91,92,91,92,91,
						90,91,92,91,92,93,92,93,94,93,94,95,96,97,96,97,
						98,99,100,99,100,99,98,97,98,97,98,99,98,99,98,99,
						98,99,100,99,100,101,102,103,102,103,102,103,102,101,100,99,
						98,99,98,99,100,101,100,101,102,103,104,103,104,105,104,105,
						106,107,106,105,104,105,104,103,102,101,100,99,98,97,98,99,
						100,101,100,99,98,97,98,99,100,101,100,99,98,97,98,99,
						98,99,100,101,100,99,100,101,102,103,104,105,104,103,102,101,
						102,101,102,101,102,101,102,103,104,103,102,101,102,103,102,103,
						102,101,100,101,102,103,102,101,100,99,98,97,96,95,94,93,
						94,95,94,95,96,97,96,95,94,93,92,91,90,91,90,89,
						90,91,92,91,92,93,94,93,94,93,94,93,92,93,92,93,
						94,93,92,91,90,91,92,91,92,91,90,91,90,91,90,89,
						88,87,88,87,86,87,86,85,84,83,84,83,84,85,86,85,
						86,85,84,85,84,85,86,87,88,87,88,89,90,89,90,89,
						88,87,88,87,86,87,86,85,86,85,84,85,86,85,86,87,
						86,87,86,87,86,85,86,87,88,87,88,89,88,89,88,89,
						88,87,86,85,84,83,82,83,84,85,86,87,88,89,90,91,
						92,93,92,91,92,91,92,93,92,91,90,89,90,91,92,93,
						94,95,94,95,96,97,96,95,94,95,96,95,94,95,96,97,
						98,97,96,95,96,95,96,95,94,95,94,95,96,97,96,95,
						96,97,96,97,98,97,98,97,98,97,98,97,98,99,98,99,
						98,97,96,97,98,99,100,101,100,101,100,101,102,101,102,101,
						100,99,100,101,102,101,100,101,100,99,98,97,96,97,96,97,
						98,99,100,99,100,99,98,97,98,99,100,101,102,101,100,101,
						102,101,102,101,102,103,102,101,102,101,100,99,98,99,100,101,
						102,101,102,101,100,99,98,97,96,97,96,97,98,97,98,99
					};

// The Galaga ship - Gyaraga apparently (https://galaga.fandom.com/wiki/Gyaraga)
const COLOURPOINT PlayerSprite[] = {
							                           {4,0,1}, {5,0,1},
							                           {4,1,2}, {5,1,2},
											           {4,2,1}, {5,2,1},
							                           {4,3,1}, {5,3,1},
							                           {4,4,1}, {5,4,1},
							{0,5,2},                   {4,5,1}, {5,5,1},                   {8,5,2},
							{0,6,2},          {2,6,1}, {4,6,1}, {5,6,1}, {6,6,1},          {8,6,2},
							{0,7,1},          {2,7,1}, {4,7,2}, {5,7,2}, {6,7,1},          {8,7,1},
							{0,8,1},          {2,8,1}, {4,8,2}, {5,8,2}, {6,8,1},          {8,8,1},
							         {1, 9,1},{2,9,3}, {4,9,1}, {5,9,1}, {6,9,3}, {7,9,1},
							{0,10,1},{1,10,1},{2,10,1},                  {6,10,1},{7,10,1},{8,10,1},
							{0,11,3},                                                      {8,11,3},
							// Use the last five elements of the array for engine noise...
							                  {2,12,0},                  {6,12,0},
							                  {2,13,0},                  {6,13,0}
						};

// The shot template

#define SHOT_POINTS (3)

const COLOURPOINT Shot[] = {
    	        {0,0,0},
    	        {0,2,3},
    	        {0,5,1},
    	        {0,7,2}
};

const LEVEL DefenderoidsLevels[] = {
	{"Start me up",4,12,4},
	{"Getting Harder",12,12,3}
};


#endif