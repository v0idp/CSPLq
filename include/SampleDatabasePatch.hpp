#pragma once
#include <map>
#include <string>
#include <filesystem>
#include "VROSC/SampleDatabase.hpp"
#include "VROSC/SampleGroup.hpp"
#include "VROSC/SampleData.hpp"
#include "AudioFile.hpp"

std::map<std::string, int> sampleGroups = {
    { "Bass Drums", 0 },
    { "Snares", 1000 },
    { "Hi-hats", 2000 },
    { "Rides", 3000 },
    { "Car Doors", 4000 },
    { "Toms", 5000 },
    { "Crashes", 6000 },
    { "Effects", 7000 }
};

std::map<int, std::vector<VROSC::SampleData*>> samples = {
    { 0,    std::vector<VROSC::SampleData*>() },
    { 1000, std::vector<VROSC::SampleData*>() },
    { 2000, std::vector<VROSC::SampleData*>() },
    { 3000, std::vector<VROSC::SampleData*>() },
    { 4000, std::vector<VROSC::SampleData*>() },
    { 5000, std::vector<VROSC::SampleData*>() },
    { 6000, std::vector<VROSC::SampleData*>() },
    { 7000, std::vector<VROSC::SampleData*>() }
};

bool IsSamplesInjected = false;

void ImportAudioFilesFromDirectory(std::string directory, bool recursive)
{
    //TODO: Implement this function
}

void LoadCustomSamples()
{
    PaperLogger.info("Loading custom samples...");
    std::string directory = "/sdcard/ModData/com.Really.Virtuoso/import/samples/";
    ImportAudioFilesFromDirectory(directory, false);
    for (const std::filesystem::directory_entry d : std::filesystem::directory_iterator(directory))
        if (d.is_directory()) ImportAudioFilesFromDirectory(d.path().string(), true);
}

MAKE_HOOK_MATCH(SampleDatabase_GetGroup, &VROSC::SampleDatabase::GetGroup, UnityW<::VROSC::SampleGroup>, VROSC::SampleDatabase* self, int32_t  groupId)
{
    if (!IsSamplesInjected)
    {
        for (int i = 0; i < self->_samplegroups.size(); i++)
        {
            int id = self->_samplegroups.get(i)->Id;
            if (samples.count(id) > 0)
                for (int j = 0; j < samples[id].size(); j++)
                    self->_samplegroups.get(i)->get_Samples()->Add(samples[id][j]);
        }
        IsSamplesInjected = true;
    }
    return SampleDatabase_GetGroup(self, groupId);
}