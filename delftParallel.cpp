#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <shlwapi.h>
#include "Win32InputBox.h"

using namespace std;

#define FLOWPATH "\\flow2d3d\\bin\\d_hydro.exe"
#define WAVEPATH "\\wave\\bin\\wave.exe"
#define TITLE "Delft3d Parallel Run"

const char XMLHeader[] =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
"<deltaresHydro xmlns=\"http://schemas.deltares.nl/deltaresHydro\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://schemas.deltares.nl/deltaresHydro http://content.oss.deltares.nl/schemas/d_hydro-1.00.xsd\">"
"<control> <sequence> <start>myNameFlow</start> </sequence> </control>"
"<flow2D3D name=\"myNameFlow\"> <library>flow2d3d</library>";
const char XMLFooter[] = "</flow2D3D></deltaresHydro>";
const char INITMSG[] = "Instructions:\nDelft3d folder should be at c:\\delft3d and Intel MPI Runtime 4.1.3.047 should be installed. Remember, some features as DD does not run in parallel. \nRun procedure:\n 1) Choose the mdf file\n 2) Choose the number of processors\n 3) If you are luck Delft3d will run in Parallel!\n\nP.S. Answer following questions correctly. There is no warranty.\n Developed by: juliowerner at ufpr.br";


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR szCmdLine, int nCmdShow)
{
    OPENFILENAME ofn;
    char szFile[100] ;
    int output;
    char numProc[]="\0\0";
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    string mdffname;
    string command;

    string MPIPATH = "c:\\Program Files (x86)\\Intel\\MPI-RT\\4.1.3.047\\em64t\\bin\\mpiexec";
    string D3DPATH = "c:\\delft3d\\win32";
    int numCPU = sysinfo.dwNumberOfProcessors;

    // Initial Instructions
    output = MessageBox(NULL, INITMSG, TITLE, MB_OKCANCEL);

    if (output == IDCANCEL) exit(1);
    // Ask for a mdf file name
    ZeroMemory( &ofn , sizeof( ofn));
    ofn.lStructSize = sizeof ( ofn );
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof( szFile );
    ofn.lpstrFilter = "MDFfile\0*.mdf\0";
    ofn.nFilterIndex =1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir=NULL;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ;
    GetOpenFileName(&ofn);

    // Ask for a number of Processes
    string messageP = "There is/are ";
    messageP += to_string(numCPU) + " cores\n Type the number of  processors to use:";
    CWin32InputBox::InputBox("", messageP.c_str(), numProc, 2, false);

    // Get Path, File Name, Extension
    string fname = PathFindFileName(ofn.lpstrFile);
    string ext = PathFindExtension(ofn.lpstrFile);
    PathRemoveFileSpec(ofn.lpstrFile);
    string fpath = ofn.lpstrFile;

    // Enter in the workspace directory
    command = "cd ";
    command.append(fpath);
    system(command.c_str());

    // Create tmp xml file in the workspace directory
    ofstream xmlFile;
    system("del tmp.xml");
    xmlFile.open("tmp.xml");
    xmlFile << XMLHeader << endl;
    if(ext == ".mdf")
        xmlFile << "<mdfFile>" << fname << "</mdfFile>" << endl;
    else if (ext == ".ddb")
        xmlFile << "<ddbFile>" << fname << "</ddbFile>" <<endl;
    xmlFile << XMLFooter << endl;

    // Ask for Wave Model
    output = MessageBox(NULL, "Would you like to run WAVE package?", TITLE,  MB_YESNO);
    if (output == IDYES)
    {
        output = MessageBox(NULL, "Choose the MDW file in following window.", TITLE, MB_OK);
        OPENFILENAME ofnmdw;
        ZeroMemory( &ofnmdw , sizeof(ofnmdw));
        ofnmdw.lStructSize = sizeof (ofnmdw);
        ofnmdw.hwndOwner = NULL;
        ofnmdw.lpstrFile = szFile;
        ofnmdw.lpstrFile[0] = '\0';
        ofnmdw.nMaxFile = sizeof( szFile );
        ofnmdw.lpstrFilter = "MDWfile\0*.mdw\0";
        ofnmdw.nFilterIndex =1;
        ofnmdw.lpstrFileTitle = NULL;
        ofnmdw.nMaxFileTitle = 0;
        ofnmdw.lpstrInitialDir=NULL;
        ofnmdw.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
        GetOpenFileName(&ofnmdw);
        string mdwfname = ofnmdw.lpstrFile;
        // Start wave in new window
        command = "start \"Wave Simulation\" \"";
        command += D3DPATH + WAVEPATH + "\"";
        command += " " + mdwfname + " 1";
        system(command.c_str());
    }
    // Run Delft3d
    command = "\"\"";
    command += MPIPATH;
    command += "\"\"";
    command += " -localonly ";
    command += numProc;
    command += " ";
    command += D3DPATH;
    command += FLOWPATH;
    command.append(" tmp.xml");
    system(command.c_str());
    system("pause");
    return 0;
}
