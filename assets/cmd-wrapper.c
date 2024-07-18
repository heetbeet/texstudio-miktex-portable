//#define NOSHELL
#define _UNICODE
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdbool.h>
#include <tchar.h>

#ifdef NOSHELL
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
    int main( int argc, char ** argv ) 
#endif
{
    // *******************************************
    //Get a direct path to the current running exe
    // *******************************************
    int size = 125;
    TCHAR* cmdPath = (TCHAR*)malloc(1);

    // read until GetModuleFileNameW writes less than its cap (of size)
    do {
        size *= 2;
        free(cmdPath);
        cmdPath = (TCHAR*)malloc(size*2);

    } while (GetModuleFileNameW(NULL, cmdPath, size) == size);


    // *******************************************
    // Get commandline string as a whole
    // *******************************************
    TCHAR* cmdArgs = GetCommandLineW();

    // *******************************************
    // Remove argument 0 from the commandline string
    // http://www.windowsinspired.com/how-a-windows-programs-splits-its-command-line-into-individual-arguments/
    // *******************************************
    bool inQuote = false;
    bool isArgs = false;
    int j = 0;

    // +1 includes the null termination character
    for(int i=0; i<_tcslen(cmdArgs)+1; i++){
      //must be easier way to index unicode string
      TCHAR c = *(TCHAR *)(&cmdArgs[i*2]);
      
      if(c == L'"'){inQuote = !inQuote;}
      if(c == L' ' && !inQuote){ isArgs = true;}

      //do for both unicode bits
      cmdArgs[j*2  ] = cmdArgs[i*2  ];
      cmdArgs[j*2+1] = cmdArgs[i*2+1];

      //sync j with i after filepath
      if(isArgs){ j++; }
    }

    
    // *******************************************
    // Find basedir of cmdPath
    // *******************************************
    TCHAR* cmdBaseDir;
    cmdBaseDir = (TCHAR*) malloc((_tcslen(cmdPath)+1)*2);
    cmdBaseDir[0] = '\0';
    cmdBaseDir[1] = '\0';

    _tcscpy(cmdBaseDir, cmdPath);

    int nrOfSlashed = 0;
    int slashLoc = 0;
    for(int i=0; i<_tcslen(cmdBaseDir); i++){
      //must be easier way to index unicode string
      TCHAR c = *(TCHAR *)(&cmdBaseDir[i*2]);
      if(c == L'\\' || c == L'//'){
        nrOfSlashed+=1;
        slashLoc=i;
      }
    }

    if(nrOfSlashed==0){
      _tcscpy(cmdBaseDir, L".");
    }else{
      cmdBaseDir[2*slashLoc] = '\0';
      cmdBaseDir[2*slashLoc+1] = '\0';  
    }
    

    // *******************************************
    // Find filename without .exe
    // *******************************************
    TCHAR* cmdName;
    cmdName = (TCHAR*) malloc((_tcslen(cmdPath)+1)*2);
    cmdName[0] = '\0';
    cmdName[1] = '\0';

    _tcscpy(cmdName, cmdPath);

    cmdName = &cmdPath[slashLoc==0?0:slashLoc*2+2];
    int fnameend = _tcslen(cmdName);
    
    // if we run as path\program.exe then we need to truncate the .exe part
    if(0 < fnameend-4 &&  cmdName[(fnameend-4)*2] == '.' && 
                          (cmdName[(fnameend-3)*2] == 'e' || cmdName[(fnameend-3)*2] == 'E') &&
                          (cmdName[(fnameend-2)*2] == 'x' || cmdName[(fnameend-2)*2] == 'X') &&
                          (cmdName[(fnameend-1)*2] == 'e' || cmdName[(fnameend-1)*2] == 'E') ){
        cmdName[(fnameend-4)*2]   = '\0';
        cmdName[(fnameend-4)*2+1] = '\0';
    }

    int totlen;


    // ******************************************
    // Find NAME.cmd path ...\bin\NAME.cmd
    // ******************************************
    TCHAR* programPath;
    TCHAR* subDir  = L"\\bin\\";
    TCHAR* subExt  = L".cmd";
    
    programPath = (TCHAR*) malloc((_tcslen(cmdBaseDir)+_tcslen(subDir)+_tcslen(cmdName)+_tcslen(subExt)+3)*2);
    programPath[0] = '\0';
    programPath[1] = '\0';

    _tcscpy(programPath, cmdBaseDir);
    _tcscat(programPath, subDir);
    _tcscat(programPath, cmdName);
    _tcscat(programPath, subExt);

    if(0 == _waccess(programPath, 0)){
        goto breakout_runcmd;
    }

    err_runcmd:;
    TCHAR* err1 = L"powershell -nop -command \"[reflection.assembly]::LoadWithPartialName('System.Windows.Forms')|out-null;[windows.forms.messagebox]::Show('Cannot find: ";
    TCHAR* err2 = L"', 'Execution error')\" ";

    TCHAR* err;
    err = (TCHAR*) malloc((_tcslen(err1)+_tcslen(programPath)+_tcslen(err2)+3)*2);
    _tcscpy(err, err1);
    _tcscat(err, programPath);
    _tcscat(err, err2);

    _tsystem(err);

    exit(-1);
    breakout_runcmd:;


    // *******************************************
    // Get into this form: cmd.exe /c ""c:\path\...\bin\NAME.cmd" arg1 ... "
    // *******************************************
    TCHAR* cmdLine1  = L"cmd.exe /c \"";
    TCHAR* cmdLine2  = L"\"";
    TCHAR* cmdLine3  = programPath;
    TCHAR* cmdLine4  = L"\" ";
    TCHAR* cmdLine5  = cmdArgs;
    TCHAR* cmdLine6 = L"\"";

    totlen = (_tcslen(cmdLine1)+_tcslen(cmdLine2)+_tcslen(cmdLine3)+_tcslen(cmdLine4)+_tcslen(cmdLine5)+_tcslen(cmdLine6));

    TCHAR* cmdLine;
    cmdLine = (TCHAR*) malloc((totlen+3)*2);
    cmdLine[0] = '\0';
    cmdLine[1] = '\0';

    //Pick correct cmd sequence sequence
    _tcscat(cmdLine, cmdLine1);
    _tcscat(cmdLine, cmdLine2);
    _tcscat(cmdLine, cmdLine3);
    _tcscat(cmdLine, cmdLine4);
    _tcscat(cmdLine, cmdLine5);
    _tcscat(cmdLine, cmdLine6);
    
    // ************************************
    // Prepare and run CreateProcessW
    // ************************************
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
        
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    #ifdef NOSHELL
        CreateProcessW(NULL, cmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    #else
        CreateProcessW(NULL, cmdLine, NULL, NULL, TRUE, NULL,             NULL, NULL, &si, &pi);
    #endif

    // ************************************
    // Return ErrorLevel
    // ************************************
    DWORD result = WaitForSingleObject(pi.hProcess, INFINITE);

    if(result == WAIT_TIMEOUT){return -2;} //Timeout error

    DWORD exitCode=0;
    if(!GetExitCodeProcess(pi.hProcess, &exitCode) ){return -1;} //Cannot get exitcode

    return exitCode; //Correct exitcode
}

