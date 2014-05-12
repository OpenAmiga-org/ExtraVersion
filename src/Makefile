CC    		= gcc

compile : extversion.c
	; -N switch, avoid alignment and build non-aligned (smaller) binaries
	$(CC) -N extversion.c -o Extversion -mcrt=newlib 
	strip Extversion

release: compile
	delete /release/#?
	copy Extversion /release
	copy /docs/Extversion.doc /release

clean : 
	If Exists "Extversion"@@\
		delete "Extversion"@@\
	endif

