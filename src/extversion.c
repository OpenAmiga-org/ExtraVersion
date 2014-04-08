/** 
 **  TODO: Going to continue soon..! /Marko
 **
 **/

#define APP_VERSTAG	"\0$VER: Extversion x.xx (07.04.2014)"
#define APP_EXTVER	"\0$EXTVER: Extversion Alpha 1 (07.04.2014) Extversion test comment."
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

	if((fp = fopen(filename, "rb")) == NULL){
		printf("Error: cant open file %s\n", filename);
		return RETURN_ERROR;		// something wrong
	}

	printf("$:%d\n", (char)"$");    
	char MARKER[] = "$EXTVER: ";
	printf("= %c\n", MARKER[0]);    

	while((!feof(fp)) && (c = getc(fp))!=EOF){
	    if(!is_found_marker && i>=0 && c==((int)MARKER[i])){
			//printf("i = %d\n", i);    
			printf("  %c == %c\n", c, MARKER[i]);    
			i++;
			if(i==9){
				printf("*HIt*\n");    
				is_found_marker = 1;
			}
	    }else{
			i = -1;
			printf("/");    
		}
		if(c=='\0'){
			i = 0;
		}	
		if(is_found_marker){
			printf("c = %c\n", c);
			if(c=='\0'){
				//printf("=== %s\n", extver_string);
				printf("len = %d\n", len);
				if(len<=1){
					printf("*false hit!* restarting...");
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

//	printf("= %s\n", extver_string);

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
	
	printf("Internal version: ");
	print_amiga_version_tag(APP_VERSTAG);
	printf("\n");	
}

void print_verstag(){
	print_amiga_version_tag(APP_VERSTAG);
	printf("\n");

}

void print_help(){
	printf("\n\n");
	printf("ShowFiles|sf [assignment:][path] [MORE|CLARIFY|DEFAULT] | [REVERSE\n             |TIME|SIZE|TRAIL|QUICK|FILES|DIRS|LINKS|HELP|VERSION|MANUAL]\n");
	printf("\n");
	print_version();
	printf("\n");
//	printf("\n  Usage: %s <filename> [variable_name]\n\n", argv[0]);
//	printf("  %s version %s\n", APP_NAME, APP_VERSION); 
//	printf("  %s\n\n", APP_CREDZ); 
}
