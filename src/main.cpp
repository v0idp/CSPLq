#include "main.hpp"

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

Configuration &getConfig()
{
  static Configuration config(modInfo);
  return config;
}

MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
  *info = modInfo.to_c();
  getConfig().Load();

  Paper::Logger::RegisterFileContextId(PaperLogger.tag);
  
  PaperLogger.info("Completed setup!");
}

MOD_EXTERN_FUNC void late_load() noexcept
{
  il2cpp_functions::Init();

  PaperLogger.info("Installing hooks...");
  
  INSTALL_HOOK(PaperLogger, SessionManager_SaveSessionAs);
  INSTALL_HOOK(PaperLogger, SynthController_SynthsDataLoaded);
  INSTALL_HOOK(PaperLogger, SampleDatabase_GetGroup);

  PaperLogger.info("Installed all hooks!");
}