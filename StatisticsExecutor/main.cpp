#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <chrono>

#include "../Libraries/rapidjson/include/rapidjson/document.h"

struct PROFILE_INFO
{
    std::string szName;
    std::string szExecPath;
    std::string szUsage;
};

std::vector<PROFILE_INFO> vProfiles;

void printUsage()
{
	printf("StatisticsExecutor config_file\n");
	printf("config_file: a json format for Executor profile\n");
}

void showHelp()
{
	std::cout << "01. List all avalaible profiles. Usage: list" << std::endl;
	std::cout << "02. Quit. Usage: quit / exit" << std::endl;
}

void showProfiles()
{
    std::cout << "Profiles: " << std::to_string(vProfiles.size()) << std::endl;
    for (size_t i = 0; i < vProfiles.size(); i++)
    {
        const PROFILE_INFO &info = vProfiles[i];

        std::cout << "Name: " << info.szName << std::endl;
        std::cout << "Usage: " << info.szUsage << std::endl;
    }
}

bool parseConfigFile(std::string szFile)
{
    bool bRet;
    rapidjson::Document doc;
    std::ifstream infile(szFile);
    char *pszBuffer;
    size_t nBufferLen;
    std::string szName, szExecPath, szUsage;

    // Get length of file
    nBufferLen = 0;
    infile.seekg(0, std::ios::end);
    nBufferLen = infile.tellg();
    infile.seekg(0, std::ios::beg);

    pszBuffer = new char [nBufferLen + 1];
    infile.read(pszBuffer, nBufferLen);
    pszBuffer[nBufferLen] = '\0';

    bRet = false;
    if (doc.Parse(pszBuffer).HasParseError() == false)
    {
        if (doc.IsArray() == true)
        {
            for (size_t i = 0; i < doc.Size(); i++)
            {
                PROFILE_INFO info;

                const rapidjson::Value &item = doc[i];
                
                if (item.HasMember("Name") == true)
                    szName = item["Name"].GetString();
                if (item.HasMember("ExecPath") == true)
                    szExecPath = item["ExecPath"].GetString();
                if (item.HasMember("Usage") == true)
                    szExecPath = item["Usage"].GetString();

                info.szName = szName;
                info.szExecPath = szExecPath;
                info.szUsage = szUsage;
                vProfiles.push_back(info);
            }
            bRet = true;
        }
    }

    delete []pszBuffer;
    pszBuffer = nullptr;

    return bRet;
}

int getProfileCommand(std::string &szCmd)
{
    for (size_t i = 0; i < vProfiles.size(); i++)
    {
        const PROFILE_INFO &profile = vProfiles[i];
        if (profile.szName == szCmd)
            return i;
    }
    return -1;
}

int main(int argc, char** argv)
{
    int nRetCode;

    nRetCode = -1;
	bool bExit;
	std::string szExe, szConfigFile, szCmd;

	nRetCode = 0;
    if (argc < 2)
        return nRetCode;
    
	szExe = argv[0];
    szConfigFile = argv[1];
	if (parseConfigFile(szConfigFile) == false)
	{
		printUsage();
		return nRetCode;
	}

	bExit = false;
	while (!bExit)
	{
		std::cout << ">";
		std::cin >> szCmd;
		if (szCmd == "quit" || szCmd == "exit")
			bExit = true;
		else if (szCmd == "help")
			showHelp();
        else if (szCmd == "list")
            showProfiles();
		else
		{
            int nIndex;
            std::string szInputFolder, szOutputFolder, szOutputResult, szExec;
            std::vector<std::string> vFiles;
            std::ofstream os;
            std::chrono::milliseconds nTotalTime, nTime;

            nIndex = getProfileCommand(szCmd);
            if (nIndex != -1)
            {
                std::cin >> szInputFolder;
                std::cin >> szOutputFolder;

                //  list all files
                for (const auto & entry : std::filesystem::directory_iterator(szInputFolder))
                {
                    const std::filesystem::path &path = entry.path();

                    if (!path.has_extension())
                        continue;

                    if (path.extension() == ".jpg")
                        vFiles.push_back(path.string());
                }
                
                szOutputResult = szOutputFolder + "/Result.txt";
                os.open(szOutputResult);
                if (os.is_open() == true)
                {
                    nTotalTime = std::chrono::milliseconds(0);
                    for (size_t i = 0; i < vFiles.size(); i++)
                    {
                        std::string szInput, szOutput, szFile;
                        size_t nPos;

                        szInput = vFiles[i];
                        szFile = szInput;
                        nPos = szFile.rfind('\\');
                        if (nPos > 0)
                            szFile = szFile.substr(nPos + 1);
                        nPos = szFile.rfind('.');
                        if (nPos > 0)
                            szFile = szFile.substr(0, nPos);

                        szOutput = szOutputFolder + "/" + szFile + ".jpg";
                        szExec = vProfiles[nIndex].szExecPath+ " " + szInput + " " + szOutput;

                        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
                        system(szExec.c_str());
                        std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();

                        nTime = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                        nTotalTime += nTime;

                        os << szInput << " " << nTime.count();
                        os << std::endl;
                    }
                    os.close();
                }
            }
		}
	}
   
    return 1;
}
