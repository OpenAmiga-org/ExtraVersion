# $VER: Makefile_for_Extversion 1.1 (19.05.2014)
# $EXTVER: Makefile_for_Extversion Beta_1 (19.05.2014)
# Author(s): Marko K. Seppänen at OpenAmiga.org

CC = gcc
RELEASE_PATH = "/release/"

compile : extversion.c
	; -N switch, avoid alignment and build non-aligned (smaller) binaries
	$(CC) -N extversion.c -o Extversion -mcrt=newlib 
	strip Extversion

lha : 
	copy /docs/Extversion.readme /release/
	cd $(RELEASE_PATH)@@\
	LHA -aer a "Extversion.lha" "Extversion"

release : clean compile
	makedir /release/Extversion/
	copy Extversion /release/Extversion/
	copy /docs/Extversion.doc /release/Extversion/
	copy /docs/Extversion.readme /release/Extversion/

clean : 
	If Exists "Extversion"@@\
		delete "Extversion"@@\
	endif@@\
	If Exists "/release/Extversion.lha"@@\
		delete "/release/Extversion.lha"@@\
	endif@@\
	If Exists "/release/Extversion.readme"@@\
		delete "/release/Extversion.readme"@@\
	endif@@\
	If Exists "/release/Extversion.doc"@@\
		delete "/release/Extversion.doc"@@\
	endif@@\
	If Exists "/release/Extversion"@@\
		delete "/release/Extversion/#?"@@\
		delete "/release/Extversion"@@\
	endif


