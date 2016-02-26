# $VER: Makefile_for_ExtraVersion 1.1 (19.05.2014)
# $EXTVER: Makefile_for_ExtraVersion Beta_1 (19.05.2014)
# Author(s): Marko K. Seppänen at OpenAmiga.org

CC = gcc
RELEASE_PATH = "/release/"

compile : extraversion.c
	; -N switch, avoid alignment and build non-aligned (smaller) binaries
	$(CC) -N extraversion.c -o ExtraVersion -mcrt=newlib 
	strip ExtraVersion

lha : 
	copy /docs/ExtraVersion.readme /release/
	cd $(RELEASE_PATH)@@\
	LHA -aer a "ExtraVersion.lha" "ExtraVersion"
	copy /docs/OS4Depot.txt /release/
	copy /docs/ExtraVersion.readme /release/

release : clean compile
	makedir /release/ExtraVersion/
	copy ExtraVersion /release/ExtraVersion/
	copy /docs/ExtraVersion.doc /release/ExtraVersion/
	copy /docs/ExtraVersion.readme /release/ExtraVersion/

test :
	$(CC) -N -DIS_TEST_MODE=1 extraversion.c -o ExtraVersion.test -mcrt=newlib 
	strip ExtraVersion.test

clean : 
	If Exists "ExtraVersion.test"@@\
		delete "ExtraVersion.test"@@\
	endif@@\
	If Exists "ExtraVersion"@@\
		delete "ExtraVersion"@@\
	endif@@\
	If Exists "/release/ExtraVersion.lha"@@\
		delete "/release/ExtraVersion.lha"@@\
	endif@@\
	If Exists "/release/ExtraVersion.readme"@@\
		delete "/release/ExtraVersion.readme"@@\
	endif@@\
	If Exists "/release/ExtraVersion.doc"@@\
		delete "/release/ExtraVersion.doc"@@\
	endif@@\
	If Exists "/release/ExtraVersion"@@\
		delete "/release/ExtraVersion/#?"@@\
		delete "/release/ExtraVersion"@@\
	endif


