.SUFFIXES: .c .asm .rel .abs
NAME = DEFENDEROIDS
OBJS = 	main.rel \
	library.rel \
	vgmplayer.rel \
	veclib.rel \
	Defenderoids.rel
	

$(NAME).ngp: makefile ngpc.lcf $(OBJS)
	tulink -la -o $(NAME).abs ngpc.lcf $(OBJS)
	tuconv -Fs24 $(NAME).abs
	s242ngp $(NAME).s24

.c.rel:
	cc900 -c -O3 $< -o $@

clean:
	del *.rel
	del *.abs
	del *.map 
	del *.s24 
	del *.ngp
