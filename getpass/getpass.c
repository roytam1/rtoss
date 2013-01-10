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
  
int Password( char * buffer, int buffSize, int show) 
{ 
    int count = 0; 
    INPUT_RECORD inRec; 
    HANDLE hInput, hOutput; 
    DWORD dwCharsRead, dwCharsWritten; 
      
    buffSize--; 
    hInput = GetStdHandle(STD_INPUT_HANDLE); 
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE); 
      
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
 
int main( int argc, char * argv[] )
{
    char buffer[256];

    Password(buffer, 256, 0);

	printf("%s",buffer);

	if(buffer[0])
	    return 0;
	else
		return 1;
}