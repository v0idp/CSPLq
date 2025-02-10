#pragma once

#include "VROSC/SessionsManager.hpp"
#include "VROSC/SessionSaveData.hpp"

MAKE_HOOK_MATCH(SessionManager_SaveSessionAs, &VROSC::SessionsManager::SaveSessionAs, System::Collections::IEnumerator*, VROSC::SessionsManager* self, VROSC::SessionSaveData*  sessionSaveData) {

    if (sessionSaveData->IsShared)
    {
        sessionSaveData->IsShared = false;
        sessionSaveData->IsCloud = true;
        PaperLogger.info("Preventing session from being shared to community...");
        PaperLogger.info("Session will be uploaded to cloud instead!");
    }

    return SessionManager_SaveSessionAs(self, sessionSaveData);
}