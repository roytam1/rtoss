/* *************************************************************
   Created with BCX -- The BASIC To C Translator (ver 5.09.1108)
   BCX (c) 1999 - 2006 by Kevin Diggins
   *************************************************************
     Translated for compiling with a C Compiler
   *************************************************************

   ------------------------------------------------------------------------------
    LinkRes2Exe 1.02 (c)2006 by Mike Henning
   ------------------------------------------------------------------------------
    This program is free to use and modify for personal or commercial use as long
    as LinkRes2xe itself is not being redistributed for profit.
    Any use of this program is at the users own risk. The author will not be held
    responsible for any damage that may result directly or indirectly.
   ------------------------------------------------------------------------------
*/

#include <windows.h>    // Win32 Header File 
#include <stdio.h>
#include <stdlib.h>


// *************************************************
// Instruct Linker to Search Object/Import Libraries
// *************************************************
#pragma comment(lib,"kernel32.lib")


// *************************************************
//          User Defined Types And Unions
// *************************************************

typedef struct _ResHead1
{
DWORD  DataSize;
DWORD  HeaderSize;
}ResHead1, *LPRESHEAD1;


typedef struct _ResHead2
{
DWORD  DataVersion;
WORD   MemFlags;
WORD   LangId;
DWORD  Version;
DWORD  Charistics;
}ResHead2, *LPRESHEAD2;


#ifndef BSTR
#define BSTR WCHAR*
#endif
// *************************************************
//            User Global Variables
// *************************************************

static int     Verbose;
static FILE   *FP1;



// *************************************************
//               Standard Macros
// *************************************************

#define GET(A,B,C)fread(B,1,C,A)


// *************************************************
//               Standard Prototypes
// *************************************************

int     str_cmp(char*, char*);
char*   BCX_TmpStr(size_t);
char*   string (int,int);
char*   str (double);
char*   join (int, ... );
BOOL    Exist   (char*);
BOOL    Exist_A (char*);
BOOL    Exist_B (char*);
DWORD   lof (char*);
char*   WideToAnsi (BSTR, UINT, DWORD);

// *************************************************
//               User Prototypes
// *************************************************

int     AddRes2File (char *, char *);
LPCSTR  RetResType (int);
LPSTR   RetMemFlags (int);
DWORD   IsUnicode (char *);
DWORD   dwAlign (DWORD);

// *************************************************
//                  Main Program
// *************************************************

int main(int argc, char *argv[])
{
if(argc>2)
  {
    if(argc==4)
      {
        if(str_cmp(argv[3],"-verbose")==0)
          {
            Verbose=TRUE;
          }
      }
    return AddRes2File(argv[1],argv[2]);
  }
else
  {
    printf("%s\n"," LinkRes2Exe 1.02  (c)2006 by Mike Henning");
    printf("%s\n"," Usage: LinkRes2Exe  file.res  file.exe [-verbose]");
  }
  return 0;   //  End of main program
}

// *************************************************
//                 Runtime Functions
// *************************************************

char *BCX_TmpStr (size_t Bites)
{
  static int   StrCnt;
  static char *StrFunc[64];
  StrCnt=(++StrCnt & 63);
  StrFunc[StrCnt]=(char*)realloc(StrFunc[StrCnt],Bites + 128);
  return (char*)memset(StrFunc[StrCnt],0,Bites+128);
}


int str_cmp (char *a, char *b)
{
  int counter=0;
  while(1)
   {
    if((a[counter]^b[counter]))
     {
      if((UINT) a[counter]>= (UINT) b[counter])
      return  1;
      return -1;
     }
    if(!a[counter]) return 0;
    counter++;
   }
#if !defined( __cplusplus )
 return 0;
#endif
}


char *str (double d)
{
  char *strtmp = BCX_TmpStr(16);
  sprintf(strtmp,"%.15G",d);
  return strtmp;
}


char *string (int count, int a)
{
  if(count<1) return BCX_TmpStr(1);
  char *strtmp = BCX_TmpStr(count);
  return (char*)memset(strtmp,a,count);
}


char * join(int n, ...)
{
  int i = n, tmplen = 0;
  char *s_;
  char *strtmp;
  va_list marker;
  va_start(marker, n); // Initialize variable arguments
  while(i-- > 0)
  {
    s_ = va_arg(marker, char *);
    if(s_) tmplen += strlen(s_);
  }
  strtmp = BCX_TmpStr(tmplen);
  va_end(marker); // Reset variable arguments
  i = n;
  va_start(marker, n); // Initialize variable arguments
  while(i-- > 0)
  {
    s_ = va_arg(marker, char *);
    if(s_) strcat(strtmp, s_);
  }
  va_end(marker); // Reset variable arguments
  return strtmp;
}


BOOL Exist (char *szFilePath)
{
  if(strchr(szFilePath, '*') || strchr(szFilePath, '?'))
  return Exist_A (szFilePath);
  return Exist_B (szFilePath);
}

BOOL Exist_A (char *szFilePath)
{
  WIN32_FIND_DATA W32FindData;
  HANDLE rc;
  rc = FindFirstFile(szFilePath, &W32FindData);
  if(rc == INVALID_HANDLE_VALUE) return FALSE;
  FindClose(rc);
  return TRUE;
}

BOOL Exist_B (char *szFilePath)
{
  DWORD ret;
  ret = GetFileAttributes(szFilePath);
  if (ret != 0xffffffff) return TRUE;
  return FALSE;
}


DWORD lof (char *FileName)
{
  WIN32_FIND_DATA W32FD;
  HANDLE hFile;
  int FSize;
  if(strlen(FileName)==0) return 0;
  hFile=FindFirstFile(FileName,&W32FD);
  if(hFile!=INVALID_HANDLE_VALUE)
   {
     FSize=W32FD.nFileSizeLow;
     FindClose(hFile);
     return FSize;
   }
  return 0;
}


char* WideToAnsi (BSTR WideStr, UINT CodePage,DWORD dwFlags)
{
  char *BCX_RetStr={0};
  UINT uLen;
  uLen=WideCharToMultiByte(CodePage,dwFlags,WideStr,-1,0,0,0,0);
  BCX_RetStr=(char*)BCX_TmpStr(uLen);
  WideCharToMultiByte(CodePage,dwFlags,WideStr,-1,BCX_RetStr,uLen,0,0);
  return BCX_RetStr;
}



// ************************************
//       User Subs and Functions
// ************************************


int AddRes2File (char *ResFile, char *TgtFile)
{
  HANDLE    h;
  ResHead1  rh1;
  ResHead2 *rh2;
  DWORD  ResName;
  char   szResName[2048];
  DWORD  ResType;
  char   szResType[2048];
  PCHAR  unihead=NULL;
  PCHAR  rdata=NULL;
  int    HeadOffset;
  int    UniSize;
  int    FileEnd;
  int    FilePos=0;
  int    Result=0;
  
  if(!Exist(ResFile))
    {
      printf("%s%s\n","File not found: ",ResFile);
      return 1;
    }
  else if(!Exist(TgtFile))
    {
      printf("%s%s\n","File not found: ",TgtFile);
      return 1;
    }
  h=BeginUpdateResource(TgtFile,TRUE);
  if(h==NULL)
    {
      printf("%s\n","BeginUpdateResource failed...");
      return 1;
    }
  FileEnd=lof(ResFile);
  if((FP1=fopen(ResFile,"rb"))==0)
   {
  fprintf(stderr,"Can't open file %s\n",ResFile);exit(1);
   }
   
  while(FilePos<FileEnd)
    {
      GET(FP1,&rh1,sizeof(rh1));
      FilePos+=(sizeof(rh1));
      
      UniSize=dwAlign(rh1.HeaderSize-sizeof(rh1));
      unihead=(PCHAR)realloc(unihead,UniSize);
      GET(FP1,unihead,UniSize);
      FilePos+=(UniSize);
      rh2=(LPRESHEAD2)&unihead[UniSize-sizeof(*rh2)];
      
      HeadOffset=IsUnicode(unihead);
      if(HeadOffset==0)
        {
          ResType=HIWORD(*(DWORD*)unihead);
          strcpy(szResType,RetResType(ResType));
          HeadOffset=sizeof(DWORD);
        }
      else
        {
          strcpy(szResType,WideToAnsi((LPWSTR)unihead, CP_ACP, 0));
          ResType=(DWORD)szResType;
        }
        
      if(IsUnicode(&unihead[HeadOffset])==0)
        {
          ResName=HIWORD(*(DWORD*)(unihead+HeadOffset));
          strcpy(szResName,str(ResName));
        }
      else
        {
          strcpy(szResName,WideToAnsi((LPWSTR)(&unihead[HeadOffset]), CP_ACP, 0));
          ResName=(DWORD)szResName;
        }
        
      if(Verbose)
        {
          printf("%s% d\n","DataSize    =",(int)rh1.DataSize);
          printf("%s% d\n","HeaderSize  =",(int)rh1.HeaderSize);
          printf("%s%s\n","ResType     = ",szResType);
          printf("%s%s\n","ResName     = ",szResName);
          printf("%s% d\n","DataVersion =",(int)rh2->DataVersion);
          printf("%s%s\n","MemFlags    = ",RetMemFlags(rh2->MemFlags));
          printf("%s% d\n","LangId      =",(int)rh2->LangId);
          printf("%s% d\n","Version     =",(int)rh2->Version);
          printf("%s% d\n","Charistics  =",(int)rh2->Charistics);
          printf("%s\n",string(60,45));
        }
        
      HeadOffset=dwAlign(rh1.DataSize);
      rdata=(PCHAR)realloc(rdata,HeadOffset);
      GET(FP1,rdata,HeadOffset);
      FilePos+=(HeadOffset);
      
      if(rh1.DataSize)
        {
          if(UpdateResource(h,(PCHAR)ResType,(PCHAR)ResName,rh2->LangId,rdata,rh1.DataSize)==0)
            {
              printf("%s\n","UpdateResource failed...");
              Result=1;
              break;
            }
        }
    }
    
  free(unihead);
  free(rdata);
  fclose(FP1);

  if(!EndUpdateResource(h,Result))
    {
      printf("%s\n","EndUpdateResource failed...");
      Result=1;
    }
  return Result;
}


LPCSTR RetResType (int ResType)
{
  while(1)
  {
    if(ResType==0x0000)
      {
        return "NULL";
      }
    if(ResType==0x0001)
      {
        return "Cursor";
      }
    if(ResType==0x0002)
      {
        return "Bitmap";
      }
    if(ResType==0x0003)
      {
        return "Icon";
      }
    if(ResType==0x0004)
      {
        return "Menu";
      }
    if(ResType==0x0005)
      {
        return "Dialog";
      }
    if(ResType==0x0006)
      {
        return "String Table";
      }
    if(ResType==0x0007)
      {
        return "Font Directory";
      }
    if(ResType==0x0008)
      {
        return "Font";
      }
    if(ResType==0x0009)
      {
        return "Accelerators Table";
      }
    if(ResType==0x000A)
      {
        return "RC Data (custom binary data)";
      }
    if(ResType==0x000B)
      {
        return "Message table";
      }
    if(ResType==0x000C)
      {
        return "Group Cursor";
      }
    if(ResType==0x000E)
      {
        return "Group Icon";
      }
    if(ResType==0x0010)
      {
        return "Version Information";
      }
    if(ResType==0x0011)
      {
        return "Dialog Include";
      }
    if(ResType==0x0013)
      {
        return "Plug'n'Play";
      }
    if(ResType==0x0014)
      {
        return "VXD";
      }
    if(ResType==0x0015)
      {
        return "Animated Cursor";
      }
    if(ResType==0x0018)
      {
        return "Manifest";
      }
    if(ResType==0x2002)
      {
        return "Bitmap (new version)";
      }
    if(ResType==0x2004)
      {
        return "Menu (new version)";
      }
    if(ResType==0x2005)
      {
        return "Dialog (new version)";
      }
    break;
  }
  return "Unknown";
}


LPSTR RetMemFlags (int flags)
{
  static char MemFlags[256];
  MemFlags[0]=0;
  
    if(0x0010 & flags)
      {
        strcpy(MemFlags,"MOVEABLE");
      }
    if(0x0020 & flags)
      {
        strcat(MemFlags," PURE");
      }
    if(0x0040 & flags)
      {
        strcat(MemFlags," PRELOAD");
      }
    if(0x1000 & flags)
      {
        strcat(MemFlags," DISCARDABLE");
      }

  if(flags==0)
    {
      strcpy(MemFlags,"None");
    }
  return MemFlags;
}


DWORD IsUnicode (char *sz)
{
  if(LOWORD(*(DWORD*)sz)==0xFFFF)
    {
      return 0;
    }
  return dwAlign(2+(2*wcslen((LPCWSTR)sz)));
}


DWORD dwAlign (DWORD ul)
{
  ul+=3;
  ul>>=2;
  ul<<=2;
  return ul;
}


 