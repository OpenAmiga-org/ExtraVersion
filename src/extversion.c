/** 
 **  TODO: To be continued (hopefully) soon..! /Marko
 **
 **/

#define DEBUG_MODE 	 0
#define APP_VERSTAG	"\0$VER: Extversion 0.4 (12.05.2014)"
#define APP_EXTVER	"\0$EXTVER: Extversion 0.4-Alpha_4 (12.05.2014) Extversion test comment."
#define APP_NAME	"Extversion"
#define APP_AUTHORS	""
#define BUFFER_SIZE	2049	// +1 for \0 character

#include <stdio.h>
#include <strings.h>
#include <dos/dos.h>

#include "extversion.h"


char extver_string[2048];

int scan_extver(char filename[BUFFER_SIZE]){
	FILE *fp;
	int c = 0;
	int i = -1;
	int len = 0;
	int is_found_marker = 0;
	int is_exit = 0;
	strcpy(extver_string, "\0");	// set to \0 to be sure

	if((fp = fopen(filename, "rb")) == NULL){
		printf("Error: cant open file %s\n", filename);
		return RETURN_ERROR;		// something wrong
	}

	#if DEBUG_MODE
		printf("$:%d\n", (char)"$");    
	#endif
	char MARKER[] = "$EXTVER: ";
	#if DEBUG_MODE
		printf("= %c\n", MARKER[0]);    
	#endif

	while((!feof(fp)) && (c = getc(fp))!=EOF){
	    if(!is_found_marker && i>=0 && c==((int)MARKER[i])){
			//printf("i = %d\n", i);    
			#if DEBUG_MODE
				printf("  %c == %c\n", c, MARKER[i]);    
			#endif
			i++;
			if(i==9){
				#if DEBUG_MODE
					printf("*HIt*\n");    
				#endif
				is_found_marker = 1;
			}
	    }else{
			i = -1;
			#if DEBUG_MODE			
				printf("/");    
			#endif
		}
		if(c=='\0'){
			i = 0;
		}	
		if(is_found_marker){
			#if DEBUG_MODE			
				printf("c = %c\n", c);
			#endif
			//strncat(extver_string, (*char)c, 1);
			extver_string[strlen(extver_string)] = (char)c;
//+++++++++
			if(c=='\0'){
				//printf("=== %s\n", extver_string);
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

	/*if(is_found_marker){
		while((!feof(fp)) && (c = getc(fp))!=EOF){
			printf(" c = %d\n", c);    
			//if(c=='\0') break;
			strncat(extver_string, (char)c, 1);
		}
	}*/

	fclose(fp);

	strncat(extver_string, "\0", 1);
	char *extver_string2 = trim_str(extver_string);

	#if DEBUG_MODE
		printf("extver_string:  >>>%s<<<\n", extver_string);
		printf("extver_string2: >>>%s<<<\n", extver_string2);
	#else
		printf("%s\n", extver_string2);
	#endif

	return RETURN_OK;
}
  
int main(int argc, char **argv){
	//char* used_VERSTAG = (char*)APP_VERSTAG;	// so the Amiga-version-string isn't optimized away later
	char* used_EXTVER = (char*)APP_EXTVER;		// so the Extraversion-string isn't optimized away later

	// Check for buffer overflows.
	int i;
	for(i=1; i<argc; i++){
		if((int)strlen(argv[i])+1 > BUFFER_SIZE){	// +1 for \0 character
			printf("%s: failed: buffer overflow\n", argv[0]);

			return RETURN_FAIL;		// complete or severe failure
		}
	}

	if(argc==2){	
		return scan_extver(argv[1]);
//	}else if(argc==3){	
//		return scan_extver(argv[1], argv[2]);
	}else{
		//printf("\n  Usage: %s <filename> [variable_name]\n\n", argv[0]);
		//printf("  %s version %s\n", APP_NAME, APP_VERSION); 
		//printf("  %s\n\n", APP_CREDZ); 
		print_help();
		return RETURN_WARN;		// a warning only
	}

	return RETURN_OK;
}

void print_amiga_version_tag(char verstag[]){
	printf("%s", &verstag[1]);
}

void print_version(){
	//printf("Version: %s %s\n", APP_NAME, APP_VERSION);
	
	printf("NAME\n EXTVERSION - Find the extra version number.\n\n");
	printf("FORMAT\n EXTVERSION [<library|script|file>] [VSTRING|HELP]\n\n");
	printf("TEMPLATE\n NAME,VSTRING/S,HELP/S\n\n");
	printf("FUNCTION\n EXTVERSION finds the extra version number of a library, command, script or simply any file if the extra version string has been correctly set in the file.\n\n");
	print_amiga_version_tag(APP_VERSTAG);
	printf("\n");	
}

void print_verstag(){
	print_amiga_version_tag(APP_VERSTAG);
	printf("\n");

}

void print_help(){
	printf("\n\n");
	printf("Extversion [assignment:][path] [MORE|CLARIFY|DEFAULT] | [REVERSE\n             |TIME|SIZE|TRAIL|QUICK|FILES|DIRS|LINKS|HELP|VERSION|MANUAL]\n");
	printf("\n");
	print_version();
	printf("\n");
//	printf("\n  Usage: %s <filename> [variable_name]\n\n", argv[0]);
//	printf("  %s version %s\n", APP_NAME, APP_VERSION); 
//	printf("  %s\n\n", APP_CREDZ); 
}

// Left and right trim a str.
char *trim_str(char *str){
	int len, start, end;
	char *new;

	if(!str) return NULL;
	len = strlen(str);
	new = malloc(len+1);
	if(!new) return NULL;

	start = 0;
	while(str[start]==' '){
		start++;
		if(start==len) break;
	}

	end = len;
	while(str[end]==' '){
		end--;
		if(end==0) break;
	}

	int i;
	for(i=start; i<end; i++){
		new[i-start] = str[i];
	}
	new[end] = '\0';

	#if DEBUG_MODE
		printf("start = %i\n", start);
		printf("end = %i\n", end);
	#endif

	return new;
}
