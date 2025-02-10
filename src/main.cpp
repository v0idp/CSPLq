#include "main.hpp"

#include "scotland2/shared/modloader.h"

#include "VROSC/SynthController.hpp"

MAKE_HOOK_MATCH(SynthController_SynthsDataLoaded, &VROSC::SynthController::SynthsDataLoaded, void, VROSC::SynthController* self, VROSC::InstrumentDataController*  dataController) {
  PaperLogger.info("SynthsDataLoaded called!");
  SynthController_SynthsDataLoaded(self, dataController);
}

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};
// Stores the ID and version of our mod, and is sent to
// the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be
// removed if those are in use
Configuration &getConfig() {
  static Configuration config(modInfo);
  return config;
}

// Called at the early stages of game loading
MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
  *info = modInfo.to_c();

  getConfig().Load();

  // File logging
  Paper::Logger::RegisterFileContextId(PaperLogger.tag);

  PaperLogger.info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
MOD_EXTERN_FUNC void late_load() noexcept {
  il2cpp_functions::Init();

  PaperLogger.info("Installing hooks...");
  
  INSTALL_HOOK(PaperLogger, SynthController_SynthsDataLoaded);

  PaperLogger.info("Installed all hooks!");
}