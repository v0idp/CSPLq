#pragma once
#include <exception>
#include "VROSC/SynthController.hpp"
#include "VROSC/SynthDataController.hpp"
#include "VROSC/PatchSettings.hpp"
#include "VROSC/InstrumentDataController.hpp"
#include "AudioHelm/HelmPatchFormat.hpp"
#include "UnityEngine/JsonUtility.hpp"

MAKE_HOOK_MATCH(SynthController_SynthsDataLoaded, &VROSC::SynthController::SynthsDataLoaded, void, VROSC::SynthController* self, VROSC::InstrumentDataController*  dataController) {
    SynthController_SynthsDataLoaded(self, dataController);

    // set up directory to load helm patches from
    std::string directory = "/sdcard/ModData/com.Really.Virtuoso/import/patches";

    // load all helm patches from the directory into a vector
    std::vector<std::filesystem::path> files;
    for (const auto& file : std::filesystem::recursive_directory_iterator(directory)) {
        if (file.is_regular_file() && file.path().extension() == ".helm") {
            files.push_back(file.path());
        }
    }
    
    if (!files.empty()) {
        PaperLogger.info("Loading custom patches...");

        // load all helm patches into the synth controller
        for (const auto& file : files) {
            try
            {
                std::string content = readfile(file.string());
                AudioHelm::HelmPatchFormat* patch = UnityEngine::JsonUtility::FromJson<AudioHelm::HelmPatchFormat*>(content);
                patch->patch_name = file.stem().string();
                self->get_SynthDataController()->_defaultPatches->Add(VROSC::PatchSettings::New_ctor(patch));
                self->get_SynthDataController()->AddPatch(patch);
                PaperLogger.info("Added custom patch: {}", file.stem().string());
            }
            catch (std::exception* e)
            {
                PaperLogger.error("Failed to add patch: {}", file.stem().string());
                PaperLogger.error("{}", e->what());
            }
            
        }
    } else {
        PaperLogger.info("Couldn't find any custom helm patches!");
    }
}