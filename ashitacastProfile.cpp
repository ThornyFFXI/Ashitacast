#include "ashitacast.h"
#include "ashitacastProfile.h"
#include "xmlHelpers.h"
#include <filesystem>
#include <fstream>
#include <stdio.h>

ashitacastProfile::ashitacastProfile(IAshitaCore* core, ashitacastConfig* config, OutputHelpers* output)
{
    m_AshitaCore = core;
    pConfig      = config;
    pOutput      = output;
}
ashitacastProfile::~ashitacastProfile(void)
{
    if (mIsLoaded)
    {
        delete[] mCompleteFile;
    }
}

rapidxml::xml_node<>* ashitacastProfile::getSection(string name)
{
    if (!mIsLoaded)
        return NULL;
    for (int x = 0; x < baseNodeMax; x++)
    {
        if (gBaseNodes[x] == name)
            return mXmlSections[x];
    }
    return NULL;
}
void ashitacastProfile::load(const char* playerName, int playerId, int job)
{
    if (mIsLoaded)
    {
        delete[] mCompleteFile;
        mIsLoaded = false;
    }

    const char* playerJob = m_AshitaCore->GetResourceManager()->GetString("jobs.names_abbr", job, 0);
    char fileName[1024];
    sprintf_s(fileName, 1024, "%sconfig\\ashitacast\\%s_%d\\%s.xml",
        m_AshitaCore->GetInstallPath(), playerName, playerId, playerJob);
    if (!std::filesystem::exists(fileName))
    {
        sprintf_s(fileName, 1024, "%sconfig\\ashitacast\\%s_%s.xml",
            m_AshitaCore->GetInstallPath(), playerName, playerJob);
    }
    if (!std::filesystem::exists(fileName))
    {
        pOutput->error_f("Could not find an XML to autoload for %s[%s].", playerName, playerJob);
        return;
    }

    load(fileName);
}
bool ashitacastProfile::load(const char* filename)
{
    if (mIsLoaded)
    {
        delete[] mCompleteFile;
        mIsLoaded = false;
    }

    ifstream file_reader(filename, ios::in | ios::binary | ios::ate);
    if (!file_reader.is_open())
    {
        pOutput->error_f("Failed to load XML.  [$H%s$R]", filename);
        mIsLoaded = false;
        return false;
    }

    mFileData.resize(file_reader.tellg());
    file_reader.seekg(0, ios::beg);
    file_reader.read(&mFileData[0], mFileData.size());
    file_reader.close();
    mFileData = XmlHelpers::stripComments(mFileData);
    mFilePath = std::string(filename);
    size_t ashitacastTagLocation;
    mFileData = XmlHelpers::getXmlTag(mFileData, "ashitacast", &ashitacastTagLocation);
    if (mFileData == "")
    {
        pOutput->error_f("Failed to load XML.  Could not find <ashitacast> tag.  [$H%s$R]", filename);
        mIsLoaded = false;
        return false;
    }

    for (int last = mFilePath.size() - 1; ((mFilePath[last] != '\\') && (mFilePath[last] != '/')); last--)
    {
        mDirectory = mFilePath.substr(0, last);
        mFileName  = mFilePath.substr(last);
    }

    if (!handleIncludes())
    {
        mIsLoaded = false;
        return false;
    }

    loadSettings(mFileData);

    mCompleteFile = new char[mFileData.length() + 1];
    strcpy_s(mCompleteFile, mFileData.length() + 1, mFileData.c_str());

    if ((mUsingIncludes) && (pConfig->mGenerateMergedFiles))
    {
        stringstream successloc;
        successloc << m_AshitaCore->GetInstallPath() << "config\\ashitacast\\merged\\" << mFileName;
        ofstream file_writer(successloc.str().c_str(), ios::binary);
        file_writer << mCompleteFile;
        file_writer.close();
    }

    try
    {
        mXmlDocument.parse<0>(mCompleteFile);
        xml_node<>* nodeAshitacast = mXmlDocument.first_node("ashitacast");
        for (int x = 0; x < baseNodeMax; x++)
        {
            mXmlSections[x] = NULL;
            if (nodeAshitacast)
            {
                mXmlSections[x] = nodeAshitacast->first_node(gBaseNodes[x].c_str());
            }
        }
        mIsLoaded = true;
        pOutput->message_f("XML loaded. [$H%s$R]", mFileName.c_str());
        return true;
    }
    catch (const rapidxml::parse_error& e)
    {
        int line = static_cast<long>(std::count(mCompleteFile, e.where<char>(), '\n') + 1);
        stringstream error;
        error << "Parse error[$H" << e.what() << "$R] at line $H" << line << "$R.";
        pOutput->error(error.str().c_str());
        if (mUsingIncludes)
        {
            if (pConfig->mGenerateMergedFiles)
                pOutput->error_f("This value is based on merged file.  See $HAshita/Config/Ashitacast/merged/%s$R for most recent merged file.", mFileName.c_str());
            else
                pOutput->error("This value is based on merged file and likely won't match the line in your original XML.  For easier debugging, enable GenerateMergedFiles.");

        }
        delete[] mCompleteFile;
        mIsLoaded = false;
    }
    catch (...)
    {
        delete[] mCompleteFile;
        pOutput->error("Failed to load XML.  Unknown error during XML parsing.");
        mIsLoaded = false;
    }
    return false;
}
void ashitacastProfile::reload()
{
    load(mFilePath.c_str());
}
void ashitacastProfile::unload()
{
    if (!mIsLoaded)
    {
        pOutput->error(" Could not unload XML.  No XML was loaded.");
        return;
    }
    delete[] mCompleteFile;
    mIsLoaded = false;
    pOutput->message_f("XML unloaded. [$H%s$R]", mFileName.c_str());
}
void ashitacastProfile::loadSettings(string ashitacastTag)
{
    *pConfig = ashitacastConfig();
    size_t position;

    string settingsTag = XmlHelpers::getXmlTag(ashitacastTag, "settings", &position);
    if (position == string::npos)
        return;

    string pendingTag = XmlHelpers::getXmlTag(settingsTag, "equipbags", &position);
    if (position != string::npos)
    {
        pConfig->mEquipBags.clear();

        stringstream stream(XmlHelpers::getTagContent(pendingTag));
        while (stream.good())
        {
            string bag;
            getline(stream, bag, ',');
            int bagIndex = atoi(bag.c_str());
            if ((bagIndex < 0) || (bagIndex > 12))
                continue;
            pConfig->mEquipBags.push_back(bagIndex);
        }
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "enablewardrobecheck", &position);
    if (position != string::npos)
        pConfig->mEnableWardrobeCheck = (XmlHelpers::getTagContent(pendingTag) != "false");

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "orderbyindex", &position);
    if (position != string::npos)
        pConfig->mOrderByIndex = (XmlHelpers::getTagContent(pendingTag) == "true");

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "includeemptyslots", &position);
    if (position != string::npos)
        pConfig->mIncludeEmptySlots = (XmlHelpers::getTagContent(pendingTag) == "true");

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "generatemergedfiles", &position);
    if (position != string::npos)
        pConfig->mGenerateMergedFiles = (XmlHelpers::getTagContent(pendingTag) != "false");

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "nakedforpacker", &position);
    if (position != string::npos)
        pConfig->mNakedForPacker = (XmlHelpers::getTagContent(pendingTag) != "false");

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "petskilldelay", &position);
    if (position != string::npos)
    {
        pConfig->mPetSkillDelay = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "weaponskilldelay", &position);
    if (position != string::npos)
    {
        pConfig->mWeaponskillDelay = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "abilitydelay", &position);
    if (position != string::npos)
    {
        pConfig->mJobAbilityDelay = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "spelloffset", &position);
    if (position != string::npos)
    {
        pConfig->mSpellOffset = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "petspelloffset", &position);
    if (position != string::npos)
    {
        pConfig->mPetSpellOffset = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "rangeddelay", &position);
    if (position != string::npos)
    {
        pConfig->mRangedDelay = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "rangedoffset", &position);
    if (position != string::npos)
    {
        pConfig->mRangedOffset = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "itemoffset", &position);
    if (position != string::npos)
    {
        pConfig->mItemOffset = max(1, min(3600000, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "fastcast", &position);
    if (position != string::npos)
    {
        pConfig->mFastCast = max(80, min(0, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }

    pendingTag = XmlHelpers::getXmlTag(settingsTag, "snapshot", &position);
    if (position != string::npos)
    {
        pConfig->mSnapShot = max(80, min(0, atoi(XmlHelpers::getTagContent(pendingTag).c_str())));
    }
}

void ashitacastProfile::addSet(string name, string content)
{
    if (!mIsLoaded)
    {
        pOutput->error("Failed to add set.  No XML was loaded.");
        return;
    }

    ifstream fileReader(mFilePath.c_str(), ios::in | ios::binary | ios::ate);
    if (!fileReader.is_open())
    {
        pOutput->error_f("Failed to add set.  File could not be opened for reading. [$H%s$R]", mFilePath.c_str());
        return;
    }

    string fileBuffer;
    fileBuffer.resize(fileReader.tellg());
    fileReader.seekg(0, ios::beg);
    fileReader.read(&fileBuffer[0], fileBuffer.size());
    fileReader.close();

    size_t setsLocation;
    string setsTag = XmlHelpers::getXmlTag(fileBuffer, "sets", &setsLocation);
    string resultMessage;
    if (setsLocation == string::npos)
    {
        size_t locationBase = fileBuffer.find("<ashitacast>");
        if (locationBase == string::npos)
        {
            pOutput->error_f("Failed to add set.  Neither <sets> tag nor <ashitacast> tag could be located. [$H%s$R]", mFilePath.c_str());
            return;
        }

        content.insert(0, "\r\n\t<sets>\r\n\t\t");
        content.append("\r\n\t</sets>");
        fileBuffer.insert(locationBase + 12, content);
        resultMessage = "Sets section added and set inserted.";
    }
    else
    {
        //Sets section exists, look for the set..
        size_t setsSize = setsTag.length();
        size_t setLocation;
        bool setFound = false;
        string setTag = XmlHelpers::getXmlTag(setsTag, "set", &setLocation);
        while (setLocation != string::npos)
        {
            //If set exists, replace it.
            if (XmlHelpers::getXmlAttr(setTag, "name") == name)
            {
                setsTag.erase(setLocation, setTag.length());
                setsTag.insert(setLocation, content);
                setFound      = true;
                resultMessage = "Set replaced.";
                break;
            }
            setTag = XmlHelpers::getXmlTag(setsTag, "set", &setLocation, ++setLocation);
        }

        //If set doesn't exist, insert it at the end.
        if (!setFound)
        {
            size_t insertPoint = setsTag.rfind('\n');
            if (insertPoint == string::npos)
                insertPoint = setsTag.rfind('<');
            if ((insertPoint == string::npos) || (insertPoint == 0))
            {
                //If we have a self closing sets tag(WHY?) delete it and make a not-self closing tag to insert sets into.
                setsTag     = "<sets>\r\n\t</sets>";
                insertPoint = setsTag.rfind('\n');
            }
            if ((insertPoint >= 1) && (setsTag[insertPoint - 1] == '\r'))
                insertPoint--;

            //Add a line break and indentations prior to the set.  We don't need a line break after because we're inserting right before last line break.
            content.insert(0, "\r\n\t\t");
            setsTag.insert(insertPoint, content);
            resultMessage = "Set inserted.";
        }

        //Set has been modified in sets tag, add it back to the full XML.
        fileBuffer.erase(setsLocation, setsSize);
        fileBuffer.insert(setsLocation, setsTag);
    }

    //File buffer has been modified, write it back to actual file.
    ofstream fileWriter(mFilePath.c_str(), ios::binary);
    if (!fileWriter)
    {
        pOutput->error_f("Failed to add/update set.  File could not be opened for writing. [$H%s$R]", mFilePath.c_str());
        return;
    }
    fileWriter << fileBuffer;
    fileWriter.close();
    pOutput->message_f("%s", resultMessage.c_str());

    //Attempt to reload.
    load(mFilePath.c_str());
}

bool ashitacastProfile::handleIncludes()
{
    size_t location;
    std::string tag = XmlHelpers::getXmlTag(mFileData, "include", &location);

    //handle simple includes..
    while (location != string::npos)
    {
        if (XmlHelpers::getXmlAttr(tag, "merge") != "true")
        {
            string filename = XmlHelpers::getTagContent(tag);
            stringstream file(filename);
            ifstream file_reader(file.str().c_str(), ios::in | ios::binary | ios::ate);
            if (!file_reader.is_open())
            {
                file.str(std::string());
                file << mDirectory;
                file << filename;
                file_reader = ifstream(file.str().c_str(), ios::in | ios::binary | ios::ate);
                if (!file_reader.is_open())
                {
                    pOutput->error_f("Failed to load XML.  Could not locate include file. [$H%s$R]", filename.c_str());
                    return false;
                }
            }

            mFileData.erase(location, tag.length());

            std::string newFile;
            newFile.resize(file_reader.tellg());
            file_reader.seekg(0, ios::beg);
            file_reader.read(&newFile[0], newFile.size());
            file_reader.close();
            mFileData.insert(location, newFile);
            mUsingIncludes = true;
        }
        else
        {
            location++;
        }
        tag = XmlHelpers::getXmlTag(mFileData, "include", &location, location);
    }

    //Make sure base xml doesn't have multiple instances of any section after base includes complete.
    for (int x = 0; x < baseNodeMax; x++)
    {
        size_t location;
        std::string tag = XmlHelpers::getXmlTag(mFileData, gBaseNodes[x], &location);
        if (location != string::npos)
        {
            XmlHelpers::getXmlTag(mFileData, gBaseNodes[x], &location, location + 1);
            if ((location != string::npos) && (gBaseNodes[x] != "item"))
            {
                pOutput->error_f("Ashitacast: Failed to load XML.  Multiple instances of section found in XML. [$H<%s>$R]", gBaseNodes[x].c_str());
                return false;
            }
        }
    }

    //Merge complex includes..
    tag = XmlHelpers::getXmlTag(mFileData, "include", &location);
    while (location != string::npos)
    {
        string filename = XmlHelpers::getTagContent(tag);
        stringstream file(filename);
        ifstream file_reader(file.str().c_str(), ios::in | ios::binary | ios::ate);
        if (!file_reader.is_open())
        {
            file.str(std::string());
            file << mDirectory;
            file << filename;
            file_reader = ifstream(file.str().c_str(), ios::in | ios::binary | ios::ate);
            if (!file_reader.is_open())
            {
                pOutput->error_f("Failed to load XML.  Could not locate include file. [$H%s$R]",filename.c_str());
                return false;
            }
        }

        mFileData.erase(location, tag.length());

        std::string newFile;
        newFile.resize(file_reader.tellg());
        file_reader.seekg(0, ios::beg);
        file_reader.read(&newFile[0], newFile.size());
        file_reader.close();

        //Make sure xml to be merged doesn't have multiple instances of any section.
        for (int x = 0; x < baseNodeMax; x++)
        {
            size_t location;
            std::string tag = XmlHelpers::getXmlTag(newFile, gBaseNodes[x], &location);
            if (location != string::npos)
            {
                XmlHelpers::getXmlTag(newFile, gBaseNodes[x], &location, location + 1);
                if (location != string::npos)
                {
                    pOutput->error_f("Failed to load XML.  Multiple instances of section found in include file. [$H%s$R] [$H<%s>$R]", filename.c_str(), gBaseNodes[x].c_str());
                    return false;
                }
            }
        }

        mergeTags(newFile, location);
        mUsingIncludes = true;

        tag = XmlHelpers::getXmlTag(mFileData, "include", &location);
    }

    return true;
}
void ashitacastProfile::mergeTags(string file, size_t location)
{
    enum xmlMergeMode
    {
        before = 0,
        after  = 1,
        create = 2
    };
    xmlMergeMode mergeModes[baseNodeMax];

    //Determine if the inserted nodes will be going at the start of a section(include listed before section), the end of a section(include listed after section), or inserting a new section(base xml doesn't have section)
    for (int x = 0; x < baseNodeMax; x++)
    {
        size_t locationBase;
        std::string tagBase = XmlHelpers::getXmlTag(mFileData, gBaseNodes[x], &locationBase);
        if (locationBase == string::npos)
            mergeModes[x] = xmlMergeMode::create;
        else if (locationBase > location)
            mergeModes[x] = xmlMergeMode::before;
        else
            mergeModes[x] = xmlMergeMode::after;
    }

    for (int x = 0; x < baseNodeMax; x++)
    {
        size_t locationNew;
        std::string tag = XmlHelpers::getXmlTag(file, gBaseNodes[x], &locationNew);
        if (locationNew == string::npos)
            continue;

        if (mergeModes[x] == xmlMergeMode::create)
        {
            size_t locationBase = mFileData.find("<ashitacast>");
            mFileData.insert(locationBase + 12, std::string("\r\n") + tag);
        }

        else if (mergeModes[x] == xmlMergeMode::before)
        {
            size_t locationBase;
            std::string tagBase = XmlHelpers::getXmlTag(mFileData, gBaseNodes[x], &locationBase);
            size_t lastNode     = tagBase.rfind('<');
            if (lastNode == 0)
            {
                size_t endTag = tagBase.find("/>");
                std::stringstream buffer;
                buffer << "<" << gBaseNodes[x] << ">\r\n"
                       << XmlHelpers::getTagContent(tag) << "\r\n</" << gBaseNodes[x] << ">";
                mFileData.erase(locationBase, tagBase.size());
                mFileData.insert(locationBase, buffer.str());
            }
            else
            {
                locationBase += tagBase.find('>');
                locationBase++;
                mFileData.insert(locationBase, XmlHelpers::getTagContent(tag));
            }
        }

        else if (mergeModes[x] == xmlMergeMode::after)
        {
            size_t locationBase;
            std::string tagBase = XmlHelpers::getXmlTag(mFileData, gBaseNodes[x], &locationBase);
            size_t lastNode     = tagBase.rfind('<');
            if (lastNode == 0)
            {
                size_t endTag = tagBase.find("/>");
                std::stringstream buffer;
                buffer << "<" << gBaseNodes[x] << ">\r\n"
                       << XmlHelpers::getTagContent(tag) << "\r\n</" << gBaseNodes[x] << ">";
                mFileData.erase(locationBase, tagBase.size());
                mFileData.insert(locationBase, buffer.str());
            }
            else
            {
                mFileData.insert(locationBase + lastNode, XmlHelpers::getTagContent(tag));
            }
        }
    }
}