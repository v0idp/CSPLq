#pragma once
#include <exception>
#include <map>
#include <string>
#include <filesystem>
#include "UnityEngine/AudioClip.hpp"
#include "VROSC/SampleDatabase.hpp"
#include "VROSC/SampleGroup.hpp"
#include "VROSC/SampleData.hpp"
#include "AudioFile.hpp"
#include "main.hpp"

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

UnityEngine::AudioClip* loadAudioClip(std::filesystem::path path)
{
    UnityEngine::AudioClip* audioClip = UnityEngine::AudioClip::New_ctor();
    AudioFile<float> audioFile;
    
    audioFile.load(path.string());

    audioClip = audioClip->Create(path.stem().string(), audioFile.getNumSamplesPerChannel(), audioFile.getNumChannels(), audioFile.getSampleRate(), false);
    std::vector<float> flatSamples;
    for(size_t i = 0; i < audioFile.getNumSamplesPerChannel(); i++) {
        for(size_t j = 0; j < audioFile.getNumChannels(); j++) {
            flatSamples.push_back(audioFile.samples[j][i]);
        }
    }

    audioClip->SetData(ArrayW<float>(flatSamples), 0);
    return audioClip;
}

template <typename DirectoryIterator>
std::vector<std::filesystem::path> getWavFiles(const std::filesystem::path& dir) {
    std::vector<std::filesystem::path> wavFiles;
    for (const auto& entry : DirectoryIterator(dir))
    {
        if (entry.path().extension() == ".wav") {
            wavFiles.push_back(entry.path());
        }
    }
    return wavFiles;
}

inline void ImportAudioFilesFromDirectory(std::string directory, bool recursive)
{
    std::filesystem::path fsPath(directory);
    std::string dirName = (fsPath.filename().string().size() > 0) ? fsPath.filename().string() : "root";

    int groupId = sampleGroups["Effects"];
    if (sampleGroups.count(dirName) > 0)
        groupId = sampleGroups[dirName];

    int index = groupId + 100;

    PaperLogger.info("[{0}] Checking path: {1}", dirName, directory);
    std::vector<std::filesystem::path> files;
    if (recursive) files = getWavFiles<std::filesystem::recursive_directory_iterator>(directory);
    else files = getWavFiles<std::filesystem::directory_iterator>(directory);

    
    for (const auto& file : files)
    {
        try
        {
            std::string fileName = file.stem().string();
            UnityEngine::AudioClip* audioClip = loadAudioClip(file.string());
            VROSC::SampleData* sampleData = VROSC::SampleData::New_ctor();
            sampleData->_id = index++;
            sampleData->set_name(fileName);
            sampleData->_displayName = fileName;
            sampleData->_audioClip = audioClip;
            samples[groupId].push_back(sampleData);
            
            PaperLogger.info("[{0}] Imported {1} with id {2}", dirName, fileName, index);
        } catch (std::exception* e)
        {
            PaperLogger.error("[{0}] Error importing {1}: {2}", dirName, file.stem().string(), e->what());
            PaperLogger.error("{}", e->what());
            continue;
        }
    }
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
        LoadCustomSamples();
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