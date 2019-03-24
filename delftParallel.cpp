#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <shlwapi.h>
#include "Win32InputBox.h"

using namespace std;

#define FLOWPATH "\\dflow2d3d\\scripts\\run_dflow2d3d_parallel.bat"
#define WAVEPATH "\\dflow2d3d\\scripts\\run_dflow2d3d_parallel_dwaves.bat"
#define SMPDPATH "\\share\\bin\\smpd.exe"
#define TITLE "Delft3d Parallel Run"

const char XMLHeader[] =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
"<deltaresHydro xmlns=\"http://schemas.deltares.nl/deltaresHydro\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://schemas.deltares.nl/deltaresHydro http://content.oss.deltares.nl/schemas/d_hydro-1.00.xsd\">"
"<control> <sequence> <start>myNameFlow</start> </sequence> </control>"
"<flow2D3D name=\"myNameFlow\"> <library>flow2d3d</library>";
const char XMLFooter[] = "</flow2D3D></deltaresHydro>";
const char INITMSG[] = "Instructions:\nDelft3d folder should be atC:\\Program Files\\Deltares\\Delft3D 4.04.01\\x64 or in a config.ini file. Remember, some features as DD does not run in parallel. \nRun procedure:\n 1) Choose the mdf file\n 2) Choose the number of processors\n 3) If you are luck Delft3d will run in Parallel!\n\nP.S. Answer following questions correctly. There is no warranty.\n Developed by: juliowerner at ufpr.br";


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

    string D3DPATH = "C:\\Program Files\\Deltares\\Delft3D 4.04.01\\x64";
    int numCPU = sysinfo.dwNumberOfProcessors;

    ifstream file;
    string line;
    string varname;
    string value;
    int pos;

    // Get Path from config.ini
    file.open("config.ini");
    if (file.is_open())
    {
      while (getline(file, line))
      {
        if (line.length()>0)
        {
          pos = line.find("=");
          varname = line.substr(0, pos);
          if (varname.compare("D3DPATH") == 0)
            D3DPATH = line.substr(pos+1, line.length()-1);
        }
      }
    }
    file.close();

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
    system("del config_d_hydro.xml");
    xmlFile.open("config_d_hydro.xml");
    xmlFile << XMLHeader << endl;
    if(ext == ".mdf")
        xmlFile << "<mdfFile>" << fname << "</mdfFile>" << endl;
    else if (ext == ".ddb")
        xmlFile << "<ddbFile>" << fname << "</ddbFile>" <<endl;
    xmlFile << XMLFooter << endl;

    // Ask for Wave Model
    output = MessageBox(NULL, "Would you like to run WAVE package", TITLE,  MB_YESNO);
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
        command += D3DPATH + WAVEPATH + "\" ";
        command += "-w " + mdwfname;
		ofstream batFile;
		system("del tmp_wave.bat");
		batFile.open("tmp_wave.bat");
		batFile << command << endl;
		batFile << "pause" << endl;
		system("echo %cd%");
		system("tmp_wave.bat");
		return 0;
    }
    // Run Delft3d
	command = "\"";
	command += D3DPATH;
	command += SMPDPATH;
	command += "\" ";
	command += "-install";
	command += "\n";
    command += "\"";
    command += D3DPATH;
    command += FLOWPATH;
	command += "\"";
	command += " ";
	command += numProc;
    // Create bat file in the workspace directory
    ofstream batFile;
    system("del tmp.bat");
    batFile.open("tmp.bat");
    batFile << command << endl;
    batFile << "pause" << endl;
    system("echo %cd%");
    system("tmp.bat");
    //system(command.c_str());
    //system("pause");
    return 0;
}
