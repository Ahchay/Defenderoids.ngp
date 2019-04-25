
const COLOURPOINT AsteroidTemplate[14][2] = {{{6,0,1},{6,2,1}},{{8,1,1},{7,2,1}},{{9,2,1},{8,3,1}},{{10,4,1},{8,4,1}},{{9,6,1},{8,5,1}},{{8,7,1},{7,6,1}},{{6,8,1},{6,6,1}},{{4,8,1},{5,6,1}},{{2,7,1},{3,5,1}},{{1,6,1},{2,5,1}},{{0,4,1},{2,4,1}},{{1,2,1},{2,3,1}},{{2,1,1},{3,2,1}},{{4,0,1},{5,2,1}}};

const u8 HeightMap[] = {
							100,99,98,97,96,95,96,97,
							96,97,96,95,94,95,94,95,
							96,95,96,97,98,97,96,97,
							98,97,98,97,98,99,100,99,
							98,97,96,97,98,97,96,97,
							96,95,94,95,96,97,98,99,
							100,99,98,97,96,97,96,95,
							96,95,94,93,92,91,90,91,
							90,89,88,87,88,87,88,89,
							90,91,92,93,94,95,96,97,
							96,95,94,95,96,97,98,97,
							96,95,96,95,94,95,94,95,
							94,93,92,91,92,93,92,91,
							90,91,92,91,90,91,90,89,
							90,89,90,89,88,89,90,89,
							90,91,90,91,92,93,94,95,
							94,93,92,93,92,91,90,89,
							90,91,90,89,90,91,92,91,
							90,91,92,91,92,91,92,93,
							94,95,96,97,96,95,96,97,
							98,97,98,99,100,99,100,101,
							102,101,100,99,98,99,100,101,
							100,99,100,101,102,103,102,101,
							102,103,104,103,104,103,102,103,
							104,105,104,105,106,105,104,103,
							104,105,106,107,108,107,106,105,
							104,103,102,103,102,101,100,99,
							98,99,100,99,100,99,100,101,
							100,101,100,99,98,99,98,97,
							98,97,96,97,96,97,96,97,
							98,99,100,99,98,99,100,101,
							102,101,100,99,100,101,100,101
						};

const COLOURPOINT PlayerSprite[] = {
							                           {3, 0,1},
							                           {3, 1,2},
											           {3, 2,1},
							                           {3, 3,1},
							                           {3, 4,1},
							{0, 5,2},                  {3, 5,1},                  {6, 5,2},
							{0, 6,2},         {2, 6,1},{3, 6,1},{4, 6,1},         {6, 6,2},
							{0, 7,1},         {2, 7,1},{3, 7,2},{4, 7,1}         ,{6, 7,1},
							{0, 8,1},         {2, 8,1},{3, 8,2},{4, 8,1},         {6, 8,1},
							         {1, 9,1},{2, 9,3},{3, 9,1},{4, 9,3},{5, 9,1},
							{0,10,1},{1,10,1},{2,10,3},         {4,10,3},{5,10,1},{6,10,1},
							{0,11,3},                                             {6,11,3},
							// Use the last five elements of the array for engine noise...
							                  {2,12,1},{3,12,1},{4,12,1},
							                  {2,13,1},         {4,13,1}
						};

const COLOURPOINT Shot[] = {
	        {3,0,2},
	        {3,1,1},
	        {3,2,1},
	        {3,3,1},
	        {3,4,1},
	// Again, five elements for engine noise
	{2,5,0},{3,5,0},{2,5,0},
	{2,6,0},        {4,6,0}
};

// VECTOROBJECT Constructor
// {{ORIGINX,ORIGINY},{POSITIONX,POSITIONY},{MOVEMENTVECTORX,MOVEMENTVECTORY},POINTS,{colourpoints},SCALE,ROTATIONANGLE,ROTATIONSPEED}
const VECTOROBJECT Alphabet[] = {
	// "D" 0
	{
			{3,3},
			{0,0},
			{0,0},
			9,
			{
				{0,1,0},
				{0,1,1},
				{4,0,1},
				{5,1,1},
				{5,6,1},
				{4,7,1},
				{0,7,1},
				{1,7,1},
				{1,0,1}
			},
			1,
			0,
			0
	},
	// "e" 1
	{
			{3,3},
			{0,0},
			{0,0},
			12,
			{
				{4,6,0},
				{4,6,1},
				{3,7,1},
				{1,7,1},
				{0,6,1},
				{0,3,1},
				{1,2,1},
				{3,2,1},
				{4,3,1},
				{4,4,1},
				{0,4,1},
				{0,2,1}
			},
			1,
			0,
			0
	},
	// "f" 2
	{
			{3,3},
			{0,0},
			{0,0},
			11,
			{
				{5,3,0},
				{5,3,1},
				{4,2,1},
				{3,2,1},
				{2,3,1},
				{2,8,1},
				{1,9,1},
				{0,8,1},
				{1,5,0},
				{1,5,1},
				{4,5,1}
			},
			1,
			0,
			0
	},
	// "n" 3
	{
			{3,3},
			{0,0},
			{0,0},
			8	,
			{
				{0,2,0},
				{0,2,1},
				{0,7,1},
				{0,4,1},
				{1,2,1},
				{3,2,1},
				{4,3,1},
				{4,7,1}
			},
			1,
			0,
			0
	},
	// "d" 4
	{
			{3,3},
			{0,0},
			{0,0},
			11,
			{
				{4,0,0},
				{4,0,1},
				{4,7,1},
				{4,5,1},
				{3,7,1},
				{1,7,1},
				{0,6,1},
				{0,4,1},
				{1,3,1},
				{3,3,1},
				{4,4,1}
			},
			1,
			0,
			0
	},
	// "r" 5
	{
			{3,3},
			{0,0},
			{0,0},
			8,
			{
				{0,2,0},
				{0,2,1},
				{0,7,1},
				{0,4,1},
				{1,2,1},
				{3,2,1},
				{4,3,1},
				{5,4,1}
			},
			1,
			0,
			0
	},
	// "o" 6
	{
			{3,3},
			{0,0},
			{0,0},
			9,
			{
				{0,3,0},
				{0,3,1},
				{3,3,1},
				{4,4,1},
				{4,6,1},
				{3,7,1},
				{1,7,1},
				{0,6,1},
				{0,3,1}
			},
			1,
			0,
			0
	},
	// "i" 7
	{
			{3,3},
			{0,0},
			{0,0},
			9,
			{
				{1,7,0},
				{1,7,1},
				{1,3,1},
				{1,2,0},
				{1,2,1},
				{2,1,1},
				{1,1,1},
				{0,2,1},
				{1,2,1}
			},
			1,
			0,
			0
	},
	// "s" 8
	{
			{3,3},
			{0,0},
			{0,0},
			9,
			{
				{4,3,0},
				{4,3,1},
				{2,3,1},
				{1,4,1},
				{2,5,1},
				{3,5,1},
				{4,6,1},
				{2,7,1},
				{1,6,1}
			},
			1,
			0,
			0
	},
	// "P" 9
	{
			{3,3},
			{0,0},
			{0,0},
			11,
			{
				{0,7,0},
				{0,7,1},
				{0,0,1},
				{0,2,1},
				{1,0,1},
				{3,0,1},
				{4,1,1},
				{4,3,1},
				{3,4,1},
				{0,4,1},
				{0,0,1}
			},
			1,
			0,
			0
	},
	// "t" 10
	{
			{5,10},
			{0,0},
			{0,0},
			8,
			{
				{2,0,0},
				{2,0,1},
				{2,6,1},
				{3,7,1},
				{4,6,1},
				{1,3,0},
				{1,3,1},
				{4,3,1}
			},
			1,
			0,
			0
	},
	// "a" 11
	{
			{3,6},
			{0,0},
			{0,0},
			10,
			{
				{0,2,0},
				{0,2,1},
				{3,3,1},
				{4,4,1},
				{4,7,1},
				{4,2,1},
				{3,7,1},
				{1,7,1},
				{0,6,1},
				{0,2,1}
			},
			1,
			0,
			0
	},
	// "'A'" 12
	{
			{3,3},
			{0,0},
			{0,0},
			28,
			{
				{3,7,0},
				{3,8,1},
				{3,4,1},
				{4,3,1},
				{5,3,1},
				{6,4,1},
				{6,8,1},
				{3,5,0},
				{3,5,1},
				{6,5,1},
				{3,0,0},
				{3,0,1},
				{5,0,1},
				{7,1,1},
				{8,2,1},
				{9,5,1},
				{9,7,1},
				{8,9,1},
				{7,10,1},
				{5,11,1},
				{3,11,1},
				{2,10,1},
				{1,9,1},
				{0,7,1},
				{0,4,1},
				{1,2,1},
				{2,1,1},
				{3,0,1}
			},
			1,
			0,
			0
	},
	// "b" 13
	{
			{3,8},
			{0,0},
			{0,0},
			11,
			{
				{0,14,1},
				{0,12,1},
				{1,10,1},
				{3,9,1},
				{4,10,1},
				{5,12,1},
				{4,14,1},
				{3,15,1},
				{0,14,1},
				{0,15,1},
				{0,3,1}
			},
			1,
			0,
			0
	},
	// "u" 14
	{
			{3,7},
			{0,0},
			{0,0},
			10,
			{
				{5,9,1},
				{5,15,1},
				{5,14,1},
				{5,15,1},
				{4,15,1},
				{3,15,1},
				{1,14,1},
				{0,13,1},
				{0,9,1},
			},
			1,
			0,
			0
	}
};