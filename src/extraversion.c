/** 
 **  ExtraVersion
 **
 **
 */

//when not fund
//EXTRAVERSION: Could not find extra version information for "Disk.info".


/// See:
// AutoDoc: console.doc CMD_WRITE

#include <stdio.h>
#include <strings.h>
#include <dos/dos.h>
#include <proto/dos.h>

#define DEBUG_MODE 	 0
#define APP_YEAR	"2016"
#define APP_VERSION	"1.0b"
#define APP_VERSTAG	"\0$VER: ExtraVersion 1.0 (26.02.2016) This release is a beta version."
#define APP_EXTVER	"\0$EXTVER: ExtraVersion 1.0b (26.02.2016) This release is a beta version."
#define APP_NAME	"ExtraVersion"
#define APP_AUTHOR	"Marko K. Seppänen / OpenAmiga.org"
#if !IS_TEST_MODE
	#define IS_TEST_MODE 0	// Run "make test" to set this and run tests.
#endif

void print_help();
void print_version();
void print_verstag();
int trim_str();
void print_color_table();
void print_color_row(int, int);
void run_tests();

int debug_unfreed_mallocs = 0;

//char str_buffer[2048];
//char str_file[2048];

int is_ok = 0;
int is_exit = 0;
int is_opt_all = 0;
int is_opt_codes = 0;

//const char OPT_HELP[] = "HELP";
//const char OPT_FULL[] = "FULL";
//const char OPT_VSTRING[] = "VSTRING";


CONST_STRPTR argTemplate
	= "NAME,FULL/S,HELP/S,VSTRING/S";
int32 argArray[] = {0, 0, 0, 0};

char extver[2048] = "\0";
char str_name[257] = "\0";
char str_version[65] = "\0";
char str_date[12] = "\0";
char str_comment[257] = "\0";
 
int main(int argc, char **argv){
	char* used_VERSTAG = (char*)APP_VERSTAG;	// so the Amiga-version-string isn't optimized away later
	char* used_EXTVER = (char*)APP_EXTVER;		// so the Extraversion-string isn't optimized away later

	#if DEBUG_MODE
		printf("ExtraVersion started with DEBUG_MODE set.\n");
	#endif

	if(IS_TEST_MODE){
		run_tests();
		return RETURN_OK;
	}

	// Check for buffer overflows.
/*	int i;
	for(i=1; i<argc; i++){
		if((int)strlen(argv[i])+1 > BUFFER_SIZE){	// +1 for \0 character
			printf("%s: failed: buffer overflow\n", argv[0]);

			return RETURN_FAIL;		// complete or severe failure
		}
	}
*/

	struct RDArgs *result = IDOS->ReadArgs(argTemplate, argArray, NULL);
	if(result){
		is_ok = 0;
		//printf("XXXXX yippeeee\n");
/*		if((int32)argArray[0] != NULL){
			printf("name: %s\n", (STRPTR)argArray[0]);
			//str_file[strlen(argv[i])] = '\0';
			//str_file = argArray[0];
			is_opt_default = 1;
		}
*/		if((int32)argArray[0] != NULL){
			//printf("all\n");
			//is_opt_all = 1;
			//is_ok = 1;
		}
		if((int32)argArray[1] != NULL){
			// FULL option
			//is_opt_full = 1;
			if(get_extver(argv[1])){
				printf("%s %s (%s)\n", str_name, str_version, str_date);
				printf("%s\n", str_comment);
			}
			is_ok = 1;
		}
		if((int32)argArray[2] != NULL){
			// HELP option
			print_help();
			//is_exit = 1;
			is_ok = 1;
		}
		if((int32)argArray[3] != NULL){
			// VSTRING option
			if(get_extver(argv[1])){
				printf("%s\n", extver);
			}
			//is_exit = 1;
			is_ok = 1;
		}
		IDOS->FreeArgs(result);
	}

	if(!is_ok){
	if(argc==2){
		// Just a file as option.
		if(get_extver(argv[1])){
			printf("%s %s\n", str_name, str_version);
		}
		is_ok = 1;
	}
}

	if(!is_ok && argc>1){
		printf("%s: Syntax error\n", APP_NAME);
		is_exit = 1;
	}

	/*if(!is_exit){
		printf("do something\n");
	}
	*/

	#if DEBUG_MODE
		printf("Unfreed mallocs: %i\n", debug_unfreed_mallocs);
	#endif
	
	//printf("\n");
	return RETURN_OK;
}

void print_amiga_version_tag(char verstag[]){
	printf("%s", &verstag[1]);
}

/*
void print_version(){
	//printf("Version: %s %s\n", APP_NAME, APP_VERSION);
	
	printf("NAME\n EXTVERSION - Find the extra string number.\n\n");
	printf("FORMAT\n EXTVERSION [<library|script|file>] [VSTRING|FULL|HELP]\n\n");
	printf("TEMPLATE\n NAME,VSTRING/S,FULL/S,HELP/S\n\n");
	printf("FUNCTION\n EXTVERSION finds the extra version number of a library, command, script or simply any file if the extra version string has been correctly set in the file.\n");
	//print_amiga_version_tag(APP_VERSTAG);
	printf("\n");	
}
*/

/*void print_verstag(){
	print_amiga_version_tag(APP_VERSTAG);
	printf("\n");

}
*/

/*
void print_exttag(){
	print_amiga_version_tag(APP_EXTVER);
	printf("\n");

}
*/

void print_help(){
	//printf("\n");
	printf("(C) %s %s\n", APP_YEAR, APP_AUTHOR);
	print_amiga_version_tag(APP_VERSTAG);
	printf("\n");
	print_amiga_version_tag(APP_EXTVER);
	printf("\n");
	printf("\n");
	printf("   USAGE\n");
	printf("    %s [<file>] | [HELP] | [FULL][VSTRING]\n\n", APP_NAME);
	printf("   NAME\n");
	printf("    %s - Finds the extra version number/string (non-Amiga version).\n\n", APP_NAME);
	printf("   FORMAT\n");
	printf("    Extversion [<file>] [FULL] [HELP] [VSTRING]\n\n");
	printf("   TEMPLATE\n");
	printf("    %s\n\n", argTemplate);
	printf("   FUNCTION\n");
	printf("    Extversion finds the extra version number/string of a file, given it exists.\n\n");
	printf("    By default, only the name and the version number are printed. Further\n");
	printf("    information might be available, such as the date, use the FULL option to\n");
	printf("    print it as well.\n\n");
	printf("    The VSTRING keyword requires a NAME parameter. If the extra version\n");
	printf("    information can be found for the named object, it will be printed in the\n");
	printf("    standard format; if no such information is available, an empty line will be\n");
	printf("    printed instead.\n");
	//printf("    \n");
}

/*  Scans file for extra version information, fills the
	following buffers if info is found:
	str_name 	for the "Program_name" eg.
	str_version for the "0.1rc1") eg.
	str_date 	for the "dd.mmm.yyyy" eg.
	str_comment for the "This is some comment." eg.
	Returns -1 if scanning file failed.
 */
int scan_file(char *file){
	int c;
	FILE *f;
	strcat(file, "\0");
	memset(str_name, 0, 257);		// clear
	memset(str_version, 0, 65);		// clear
	memset(str_date, 0, 12);		// clear
	memset(str_comment, 0, 257);	// clear
	char queue[8];
	int stage = 0;
	int p = 0;

	if((f = fopen(file, "rb")) == NULL){
		printf("%s: object not found: %s", APP_NAME, file);
		//return RETURN_ERROR;		// something wrong
		return -1;
	}
	
	while(TRUE){
		c = getc(f);
		if(c==EOF) break;

		switch(stage){
			//** Search for the string $EXTVER: **
			case 0 :
				queue[0] = queue[1];
				queue[1] = queue[2];
				queue[2] = queue[3];
				queue[3] = queue[4];
				queue[4] = queue[5];
				queue[5] = queue[6];
				queue[6] = queue[7];
				queue[7] = c;

				if(memcmp(queue, "$EXTVER:", 8)==0){
					// $EXTVER: string found, move to next stage.
//					printf("**HIT**\n");
					stage++;
				}
				break;
			case 1 :
				// Scan the name part (ending with a space).
				if(p==0 && c==' '){
					// Jump over the first char in the name if it's a space.
				}else{
					// Scan until we a hit a space.
					if(c==' '){
						p = 0;
						stage++;
						break;
					}
					str_name[p] = c;
					p++;
				}
				break;
			case 2 :
				// Scan the version part (ending with a space or '(' ).
				if(p==0 && c==' '){
					// Jump over the first char if it's a space.
				}else{
					// Scan until we a hit a space or '(' .
					if(c==' ' || c=='('){
						p = 0;
						stage++;
						break;
					}
					str_version[p] = c;
					p++;
				}
				break;
			case 3 :
				// Scan for '(' .
				if(p==0 && c==' '){
					// Jump over the first char if it's a space.
				}else{
					// Scan until we a hit '(' .
					if(c=='('){
						p = 0;
						stage++;
						break;
					}
					if(p>1) return -1;	// Not a valid extver string
					p++;
				}
				break;
			case 4 :
				// Scan the date part (ending with ')' ).
				if(c==')'){
					p = 0;
					stage++;
					break;
				}
				str_date[p] = c;
				if(p>12) return -1;	// Not a valid extver string
				p++;
				break;
			case 5 :
				// Scanning for the comment ending with '\0' or '\n' or EOF.
				if(c=='\0' || c=='\n' || c==EOF){
					// Then there isn't any comment.
					return 1;
				}
				if(p==0 && c==' '){
					// Jump over the first char if it's a space.
				}else{
					// Scan until we a hit '\0' or '\n' or EOF .
					if(c=='\0' || c=='\n' || c==EOF){
						p = 0;
						stage++;
						break;
					}
					str_comment[p] = c;
					p++;
				}
				break;
			case 6 :
				// If we have come this far, then we have a corrrect extver tag.
				return 1;
		}
	}

	if(f){
		//printf("Closing file\n");
		fclose(f);
	}
/*
printf("\n\n");
strcat(str_name, "\0");
printf("str_name = [%s]\n", str_name);
strcat(str_version, "\0");
printf("str_version = [%s]\n", str_version);
strcat(str_date, "\0");
printf("str_date = [%s]\n", str_date);
strcat(str_comment, "\0");
printf("str_comment = [%s]\n", str_comment);
*/

	return -1;
}

/* Returns -1 if no extra version is found.
 */
int get_extver(char *file){
	
	if(scan_file(file)<0) return -1;

	if(str_name[0]==0) return -2;
	if(str_version[0]==0) return -2;

	strcpy(extver, "$EXTVER: ");
	strcat(extver, str_name);
	strcat(extver, " ");
	strcat(extver, str_version);
	strcat(extver, " (");
	strcat(extver, str_date);
	strcat(extver, ") ");
	strcat(extver, str_comment);

	//printf("\n\nextver: %s\n", extver);

	return 1;
}

void run_tests(){
	char *file_bad =  "tests/bad/testfile.bad.";
	char *file_good1 = "tests/good/testfile.good.1.";
	char *file_good2 = "tests/good/testfile.good.2.";
	char *file = malloc(256);
	//char *file;

	int i;
	char buf[3];
	int ret = 0;

	printf("\n  Begin testing...\n\n");

	printf("  Pass 1, testing bad files:\n");

	for(i=1; i<=6; i++){
		strcpy(file, file_bad);
		snprintf(buf, 3, "%d", i);
		strcat(file, buf);
		buf[0] = 0;
		strcat(file, buf);

		printf("  Testing %s ...", file);
		if(get_extver(file)<0){
			printf("OK\n");
		}else{
			printf("FAILED\n");
			free(file);
			return;
		}
	}	

	printf("\n  Pass 2, testing good files:\n");

	for(i=1; i<=23; i++){
		strcpy(file, file_good1);
		snprintf(buf, 3, "%d", i);
		strcat(file, buf);
		buf[0] = 0;
		strcat(file, buf);

		printf("  Testing %s ...", file);
		if(ret = get_extver(file)>0){
			if(memcmp(extver, "$EXTVER: TestScript 001 (27.05.2014) Test comment.", 2)==0){
				printf("OK\n");
			}else{
				free(file);
				printf("FAILED (found: %s)\n", extver);

				printf("\n\n");
				strcat(str_name, "\0");
				printf("str_name = [%s]\n", str_name);
				strcat(str_version, "\0");
				printf("str_version = [%s]\n", str_version);
				strcat(str_date, "\0");
				printf("str_date = [%s]\n", str_date);
				strcat(str_comment, "\0");
				printf("str_comment = [%s]\n", str_comment);

				return;
			}
		}else{
			printf("FAILED (ret %i)\n", ret);
			free(file);
			return;
		}
	}	

	printf("\n  Ending testing.\n\n");
	free(file);
	return;
}
