	VECTOROBJECT Qix;
	u8 iLoopQix;
		// Setup the qix object
		// So, the Qix itself is a lot simpler than it first appears.
		// Essentially, it's a list of lines, each starting from the end position of the last
		// where the "first" line falls off the stack and is replaced at the end with a new one
		// with a random(?) length and direction.
		// Changing the length would essentially control the speed and overall size of the Qix
		Qix.Origin.x=0;
		Qix.Origin.y=0;
		Qix.Position.x=0;
		Qix.Position.y=0;
		Qix.MovementVector.x=376;
		Qix.MovementVector.y=32;
		Qix.Points=12;
		Qix.Scale=1;
		Qix.RotationAngle=0;
		Qix.RotationSpeed=0;
		for (iLoopQix=0;iLoopQix<Qix.Points;iLoopQix++)
		{
			Qix.VectorList[iLoopQix].x = 6000;
			Qix.VectorList[iLoopQix].y = 0;
			Qix.VectorList[iLoopQix].colour = 3;
		}
			// Draw and animate the qix.
			// This will be an "evil otto" style enemy that cannot be destroyed...
			// I have to draw the Qix with an "absolute" position within the bitmap, as the offset doesn't work?
			// Presumably, because the position is (0,0) and does not move.
			// Anyway, I'm keeping this in here because I like it - not really sure what it's going to do in terms of gameplay yet.

			DrawVectorObjectAbsolute((u16*)bmpPlayField,Qix);
			// Overwrite the "oldest" point in the list with a new random point.
			Qix.VectorList[iLoopQix].x = (((s16)QRandom())>>3)+(Qix.MovementVector.x>>5);
			Qix.VectorList[iLoopQix].y = (((s16)QRandom())>>3)+(Qix.MovementVector.y>>5);
			Qix.VectorList[iLoopQix].colour = 3;
			if (++iLoopQix >= Qix.Points)
			{
				iLoopQix = 0;
			}
			// Move the Qix towards the player
			Qix.MovementVector.x++;
			Qix.MovementVector.y++;

			
