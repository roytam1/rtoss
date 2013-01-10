#include <windows.h>
#include <stdio.h>
/*
 * getpass: modified from
 * http://cboard.cprogramming.com/windows-programming/67166-portable-getch.html#post477362
 */
/* Password function for Win32 console 
* reads keystrokes but displays * to mask printable entries 
* first parameter: a pointer to a char array, buffer to hold password 
* second parameter: password buffer size, including null terminator 
* return value: funtion returns 1 if max buffer is reached, else returns 0 
*/
  
int Password(char * buffer, int buffSize, HANDLE hInput, HANDLE hOutput, int show) {
	int count = 0; 
	INPUT_RECORD inRec; 
	DWORD dwCharsRead, dwCharsWritten; 

	buffSize--; 

	while(ReadConsoleInput(hInput, &inRec, 1, &dwCharsRead) && count < buffSize) 
		if(inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown == TRUE) 
			if(isprint(inRec.Event.KeyEvent.uChar.AsciiChar)) {
				buffer[count++] = inRec.Event.KeyEvent.uChar.AsciiChar;
				if(show)
					WriteConsole(hOutput, "*", 1, &dwCharsWritten, NULL);
			} else
			if(inRec.Event.KeyEvent.uChar.AsciiChar == '\b' && count > 0) {
				if(show)
					WriteConsole(hOutput, "\b \b", 3, &dwCharsWritten, NULL);
				count--; 
			} else
			if(inRec.Event.KeyEvent.uChar.AsciiChar == '\r') 
				break; 

	buffer[count] = '\0'; 

	if(count == buffSize) 
	return(1); 
	else
	return(0); 
} 
 
int main(int argc, char* argv[]) {
	HANDLE hInput, hOutput; 
	DWORD dwCharsWritten; 

	char buffer[256];
	int show = 0;
	int start = 1;
	int i;

	if(argc > 1) {
		if(argv[1][0] == '-'){
			if(argv[1][1] == '?'){
				printf("Usage: %s [-s|?] [\"prompt\"]\n",argv[0]);
				return 1;
			} else if(argv[1][1] == 's'){
				show = 1;
				++start;
			}
		}
	}

	hInput = GetStdHandle(STD_INPUT_HANDLE); 
	hOutput = GetStdHandle(STD_ERROR_HANDLE); 

	if(argc > 1) {
		for(i = start; i < argc; ++i) {
			WriteConsole(hOutput, argv[i], strlen(argv[i]), &dwCharsWritten, NULL);
			WriteConsole(hOutput, " ", 1, &dwCharsWritten, NULL);
		}
	}

	Password(buffer, 256, hInput, hOutput, show);

	printf("%s",buffer);

	if(buffer[0])
		return 0;
	else
		return 1;
}