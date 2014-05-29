/** 
 **  Extversion
 **
 **  The drawer tests/ consists of testfiles for testing.
 **  Please see /docs/DevDoc.txt for more details.
 **
 **/

#include <stdio.h>
#include <strings.h>
#include <dos/dos.h>

#define DEBUG_MODE 	 0
#define APP_VERSTAG	"\0$VER: Extversion 0.4 (29.05.2014) Version test comment."
#define APP_EXTVER	"\0$EXTVER: Extversion Beta_1 (29.05.2014) Extversion test comment."
#define APP_NAME	"Extversion"
#define APP_AUTHORS	""
#define BUFFER_SIZE	2049	// +1 for \0 character

void check_args(int, char **);
void print_help();
void print_version();
void print_verstag();
//char *trim_str(char*);
int trim_str();

int debug_unfreed_mallocs = 0;

char extver_string[2048];
//char *extver_string2;
char str_buffer[2048];
char str_file[2048];

const char MARKER[] = "$EXTVER: ";
int is_exit = 0;
int space_indexes[3] = {
		0,	// namepart
		0,	// versionpart
		0	// datepart
	};
int is_opt_default = 0;
int is_opt_full = 0;
int is_opt_vstring = 0;
const char OPT_HELP[] = "HELP";
const char OPT_FULL[] = "FULL";
const char OPT_VSTRING[] = "VSTRING";

int scan_extver(char filename[BUFFER_SIZE]){
	FILE *fp;
	int c = 0;
	int i = -1;
	int len = 0;	// length of matched scan TODO: rename to size
	int is_found_marker = 0;

	strcpy(extver_string, "\0");	// set to \0 to be sure

	if((fp = fopen(filename, "rb")) == NULL){
		printf("Error: can't open file %s\n", filename);
		return RETURN_ERROR;		// something wrong
	}

	#if DEBUG_MODE
		printf("$:%d\n", (char)"$");    
	#endif
//	char MARKER[] = "$EXTVER: ";
	#if DEBUG_MODE
		printf("MARKER[0] = %c\n", MARKER[0]);    
	#endif

	//int k = 0;
	int j = 0;
	int start = 0;
	int end = 0;

	while((!feof(fp)) && (c = getc(fp))!=EOF){
	    if(!is_found_marker && i>=0 && c==((int)MARKER[i])){
			//printf("i = %d\n", i);    
			#if DEBUG_MODE
				printf("  Found matching chars: %c == %c\n", c, MARKER[i]);    
			#endif
			i++;
			if(i==9){
				#if DEBUG_MODE
					printf("*HIt* Found full marker!\n");    
				#endif
				is_found_marker = 1;
			}
	    }else{
			i = -1;
			#if DEBUG_MODE			
				printf("/");	// char do not match marker
			#endif
		}
		if(c=='\0'){
			i = 0;
		}	
		if(is_found_marker){
			#if DEBUG_MODE			
				printf("Found full matching marker, last char without ': c = '%c'\n", c);
			#endif
			//strncat(extver_string, (*char)c, 1);
			//extver_string[strlen(extver_string)] = (char)c;	//TRASHES MEM
			if(c!=0){
				//extver_string[k++] = (char)c;	
				extver_string[strlen(extver_string)] = (char)c;	
			}
//+++++++++
			if(c=='\0'){
				//printf("extver_string = %s\n", extver_string);
				#if DEBUG_MODE
					printf("len = %d\n", len);
				#endif
				if(len<=1){
					#if DEBUG_MODE
						printf("*false hit!* restarting...");
					#endif
					is_found_marker = 0;
				}else{
					is_exit = 1;
					break;
				}
			}else{
				if(len<1048-1){
					//strcat(extver_string, (char)c);
					len++;
				}else{
					is_exit = 1;
				}
			}
		}
		if(is_exit) break;
	}

	fclose(fp);

	//strncat(extver_string, "\0", 1);
	extver_string[strlen(extver_string)] = '\0';  // End the string here since the last char was the char that got stored and the same that scan ended on, thus it does not belong to the Extversion cookie. on and
//char *extver_string2 = trim_str(extver_string);
	//*extver_string2 = malloc(strlen(extver_string)+1);
	//extver_string2 = trim_str(extver_string);

	trim_str();

//char extver_string2[strlen(extver_string)+1];
//strcpy(extver_string2, extver_string);

	#if DEBUG_MODE
		//printf("extver_string:  >>>%s<<< (before trimming)\n", extver_string);
		//printf("extver_string2: >>>%s<<<\n", extver_string2);
	#endif

	/* Splitting routine.
	 */
	/*
	int first_space_i = 0;	// namepart
	int second_space_i = 0;	// versionpart
	int third_space_i = 0;	// datepart
	*/
	int space_occur	= 0;	// amount of space found

	for(i=0; i<strlen(extver_string); i++){
		if(extver_string[i]==32){
			#if DEBUG_MODE
				printf("space found at pos: %i\n", i);
			#endif
			space_indexes[space_occur++] = i;
//			space_occur++;
		}
		if(space_occur==3) break; // all parts found already, rest is text with spaces
	}

/*
	strncpy(extver_string, extver_string2, space_indexes[1]);
	extver_string[space_indexes[1]] = '\0';	// null char manually added
	printf("%s\n", extver_string);
*/

	return RETURN_OK;
}
  
int main(int argc, char **argv){
	char* used_VERSTAG = (char*)APP_VERSTAG;	// so the Amiga-version-string isn't optimized away later
	char* used_EXTVER = (char*)APP_EXTVER;		// so the Extraversion-string isn't optimized away later

	#if DEBUG_MODE
		printf("Extversion started with DEBUG_MODE set.\n");
	#endif

	// Check for buffer overflows.
	int i;
	for(i=1; i<argc; i++){
		if((int)strlen(argv[i])+1 > BUFFER_SIZE){	// +1 for \0 character
			printf("%s: failed: buffer overflow\n", argv[0]);

			return RETURN_FAIL;		// complete or severe failure
		}
	}


	check_args(argc, argv);

	if(strlen(str_file)==0){
		print_verstag();
		print_exttag();
	}else{
		scan_extver(str_file);
	}

	#if DEBUG_MODE
		printf("is_opt_defailt: %i\n", is_opt_default);
		printf("is_opt_full: %i\n", is_opt_full);
		printf("is_opt_vstring %i\n", is_opt_vstring);
	#endif

	if(is_opt_full){
//		printf("FULLLLLLLLLLLLLLLL\n");
//		strncpy(extver_string, extver_string2, strlen(extver_string2));
//		extver_string[strlen(extver_string2)] = '\0';	// null char manually added
//		strncpy(extver_string, extver_string, strlen(extver_string));
//		extver_string[strlen(extver_string2)] = '\0';	// null char manually added
//		extver_string[space_indexes[2]] = '\0';
	}else if(is_opt_vstring){
//		printf("VSTRINGGGGGGGGGGGGG\n");
//		strncpy(extver_string, extver_string2, strlen(extver_string2));
//		extver_string[strlen(extver_string2)] = '\0';	// null char manually added
//		extver_string[space_indexes[2]] = '\0';
		printf("%s", MARKER);
	}else if(is_opt_default){
//		printf("DEFAULTTTTTTTTTTT\n");
//		strncpy(extver_string, extver_string2, space_indexes[1]);
//		extver_string[space_indexes[1]] = '\0';	// null char manually added
		extver_string[space_indexes[1]] = '\0';
	}

/*
	// Free allocation.
	if(extver_string2){
		free(extver_string2);
		debug_unfreed_mallocs--;
	}
	printf("%s\n", extver_string);
*/
	if(strlen(extver_string)>0){
		printf("%s\n", extver_string);
	}

	#if DEBUG_MODE
		printf("Unfreed mallocs: %i\n", debug_unfreed_mallocs);
	#endif
	
	return RETURN_OK;
}

void check_args(int argc, char **argv){
	int i;
	for(i=1; i<argc; i++){
		#if DEBUG_MODE
			printf("!!! %d - %s\n", i, argv[i]);
		#endif

		#if DEBUG_MODE
			printf("OPTION found: %s\n", toupper(argv[i]));
		#endif

		/* Grab options. 
		 */

		int j = 0;
		for(j=0; j<strlen(argv[i]); j++){
			str_buffer[j] = toupper(argv[i][j]);
		}
		str_buffer[strlen(argv[i])] = '\0';
		#if DEBUG_MODE
			printf("Parsed option: %s\n", str_buffer);
		#endif

		if(0==strcmp(str_buffer, OPT_HELP)){
			print_help();
			is_exit = 1;	// exit
			break;
		}else if(0==strcmp(str_buffer, OPT_FULL)){
			is_opt_full = 1;
		}else if(0==strcmp(str_buffer, OPT_VSTRING)){
			is_opt_vstring = 1;
		}else{
			int j = 0;
			for(j=0; argv[i][j]; j++){
				str_file[j] = argv[i][j];
			}
			str_file[strlen(argv[i])] = '\0';
			#if DEBUG_MODE
				printf("Parsed file: %s\n", str_file);
			#endif
			is_opt_default = 1;
		}
	}	// end of outer if
	if(is_opt_full || is_opt_vstring) is_opt_default = 0;
	
	#if DEBUG_MODE
		printf("is_opt_full: %i\n", is_opt_full);
		printf("is_opt_vstring %i\n", is_opt_vstring);
		printf("is_opt_default: %i\n", is_opt_default);
	#endif
}

void print_amiga_version_tag(char verstag[]){
	printf("%s", &verstag[1]);
}

void print_version(){
	//printf("Version: %s %s\n", APP_NAME, APP_VERSION);
	
	printf("NAME\n EXTVERSION - Find the extra version number.\n\n");
	printf("FORMAT\n EXTVERSION [<library|script|file>] [VSTRING|FULL|HELP]\n\n");
	printf("TEMPLATE\n NAME,VSTRING/S,FULL/S,HELP/S\n\n");
	printf("FUNCTION\n EXTVERSION finds the extra version number of a library, command, script or simply any file if the extra version string has been correctly set in the file.\n");
	//print_amiga_version_tag(APP_VERSTAG);
	printf("\n");	
}

void print_verstag(){
	print_amiga_version_tag(APP_VERSTAG);
	printf("\n");

}

void print_exttag(){
	print_amiga_version_tag(APP_EXTVER);
	printf("\n");

}

void print_help(){
	printf("\n\n");
	printf("EXTVERSION [<library|script|file>] [FULL|VSTRING|HELP]\n");
	printf("\n");
	print_version();
	printf("\n");
//	printf("\n  Usage: %s <filename> [variable_name]\n\n", argv[0]);
//	printf("  %s version %s\n", APP_NAME, APP_VERSION); 
//	printf("  %s\n\n", APP_CREDZ); 
}

// Left and right trim a str.
//char *trim_str(char *str){
int trim_str(){

	#if DEBUG_MODE
		printf("trim_str got: >>>%s<<<\n", extver_string);
	#endif

	int len, start, end;
//	char *new;

	if(!extver_string) return NULL;
	len = strlen(extver_string);
//	new = malloc((len+1)*sizeof(char));
//	debug_unfreed_mallocs++;
//	memset(new, 0, (len+1)*sizeof(char));

/*	if(!new){
		#if DEBUG_MODE
			printf("malloc FAILED!!\n");
		#endif
		return NULL;
	}
*/
	start = 0;
	while((extver_string[start]==' ') || (extver_string[start]=='\0')){
		start++;
		if(start==len) break;
	}
	//printf("break at char: '%c'\n", str[start]);

	end = len;
	while(extver_string[end]==' '){
		end--;
		if(end==0) break;
	}
	end--;
	//printf("break at char: '%c'\n", str[end]);

	//strncpy(new, str+start, end-start+1);
	//strncpy(new, str+start, end-start+1);
	int k = 0;

	int i;
	for(i=start; i<=end; i++){
		extver_string[k++] = extver_string[i];
		//printf("extver_string[q]: %c, %i->%i\n", extver_string[k-1], i, k);
	}
	extver_string[k] = '\0';

	//printf("extver_string >>%s<<\n", extver_string);
	#if DEBUG_MODE
		printf("trim_str left: >>>%s<<<\n", extver_string);
	#endif

return 0;
/*
//	int i;
	for(i=end+2; i<=len+1; i++){
		new[i] = '\0';
	}

	#if DEBUG_MODE
		printf("trim_str(..), start = %i\n", start);
		printf("trim_str(..), end = %i\n", end);
		printf("trim_str ended with: >>>%s<<<\n", new);
	#endif

	return new;
*/
}
