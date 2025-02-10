#pragma once
#include "VROSC/SynthController.hpp"
#include "AudioHelm/HelmPatchFormat.hpp"
#include "paper2_scotland2/shared/logger.hpp"

MAKE_HOOK_MATCH(SynthController_SynthsDataLoaded, &VROSC::SynthController::SynthsDataLoaded, void, VROSC::SynthController* self, VROSC::InstrumentDataController*  dataController) {
    SynthController_SynthsDataLoaded(self, dataController);

    // set up directory to load helm patches from
    std::string directory = "/sdcard/Android/data/com.Really.Virtuoso/files/mods/import/patches";

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
            // TODO: load the helm patch here
        }
    } else {
        PaperLogger.info("No custom patches found.");
    }
}