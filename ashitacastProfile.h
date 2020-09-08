#ifndef __ASHITA_ashitacastProfile_H_INCLUDED__
#define __ASHITA_ashitacastProfile_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "ashitacast.h"
#include "thirdparty\rapidxml.hpp"

using namespace rapidxml;
using namespace std;

class ashitacastProfile
{
public:
    bool mIsLoaded = false;
    rapidxml::xml_document<> mXmlDocument;

    ashitacastProfile(IAshitaCore* core, ashitacastConfig* config, OutputHelpers* output);
    ~ashitacastProfile(void);

    void addSet(string name, string content);
    void load(const char* filename);
    void reload();
    void unload();
    rapidxml::xml_node<>* getSection(string name);

private:
    string mFileData;
    string mFileName;
    string mFilePath;
    string mDirectory;
    char* mCompleteFile;
    rapidxml::xml_node<>* mXmlSections[baseNodeMax];
    bool mUsingIncludes = false;
    IAshitaCore* m_AshitaCore;
    ashitacastConfig* pConfig;
    OutputHelpers* pOutput;


    bool handleIncludes();
    void mergeTags(string file, size_t location);
    void loadSettings(string ashitacastTag);
};

#endif