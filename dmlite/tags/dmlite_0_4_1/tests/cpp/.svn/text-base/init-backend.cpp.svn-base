#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/utils/urls.h>
#include <iostream>

int main(int argn, char** argv)
{
  if (argn < 2) {
    std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
    return -1;
  }
  
  const char* config = argv[1];
  
  std::cout << "Simple initialization program" << std::endl;
  std::cout << "This program will initialize the selected backend for running the tests" << std::endl;
  std::cout << "Using '" << config << '\'' << std::endl;
  std::cout << std::endl;
  
  const char* home = getenv("DPNS_HOME");
  if (home) {
    std::cout << "Using DPNS_HOME=" << home << std::endl;
  }
  else {
    home = getenv("LFC_HOME");
    if (!home) {
      std::cout << "Neither DPNS_HOME, nor LFC_HOME, are defined" << std::endl;
      return -1;
    }
    std::cout << "Using LFC_HOME=" << home << std::endl;
  }
  
  // Initialize dmlite
  dmlite::PluginManager manager;
  manager.loadConfiguration(config);
  
  dmlite::StackInstance stack(&manager);
  
  dmlite::SecurityContext rootCtx;
  dmlite::GroupInfo rootGroup;
  
  rootGroup["gid"] = 0u;
  
  rootCtx.user["uid"] = 0u;
  rootCtx.groups.push_back(rootGroup);
  
  stack.setSecurityContext(rootCtx);
  
  dmlite::Catalog* catalog = stack.getCatalog();
  
  // Split home into components
  std::vector<std::string> components = dmlite::Url::splitPath(home);
  if (components[0] != "/") {
    std::cout << "HOME path must be absolute" << std::endl;
    return -1;
  }
  dmlite::ExtendedStat stat;
  
  // Add dteam
  try {
    stack.getAuthn()->getGroup("dteam");
    std::cout << "Group dteam exists" << std::endl;
  }
  catch (dmlite::DmException &e) {
    if (e.code() != DM_NO_SUCH_GROUP);
    stack.getAuthn()->newGroup("dteam");
    std::cout << "Group dteam created" << std::endl;
  }
  dmlite::GroupInfo dteam = stack.getAuthn()->getGroup("dteam");
  
  // Root is a special case
  try {
    stat = catalog->extendedStat("/");
  }
  catch (dmlite::DmException& e) {
    if (e.code() != DM_NO_SUCH_FILE) throw;
    std::cout << "Root path does not exist" << std::endl;
    
    stat.name   = "/";
    stat.parent = 0;
    stat.status = dmlite::ExtendedStat::kOnline;
    memset(&stat.stat, 0, sizeof(stat.stat));
    stat.stat.st_mode = S_IFDIR | 0755;
    
    stack.getINode()->create(stat);
    std::cout << "Root path created" << std::endl;
  }
  
  // Populate the rest
  for (unsigned i = 0; i < components.size(); ++i) {
    try {
      stat = catalog->extendedStat(components[i]);
      std::cout << components[i] << " exists" << std::endl;
    }
    catch (dmlite::DmException& e) {
      if (e.code() != DM_NO_SUCH_FILE) throw;
      std::cout << components[i] << " does not exist" << std::endl;
      catalog->makeDir(components[i], 0755);
      std::cout << components[i] << " created" << std::endl;
    }
    catalog->changeDir(components[i]);
  }
  
  // Change owner of last one
  catalog->setOwner(home, 0, dteam.getUnsigned("gid"));
  std::cout << "Changed the owner of " << home
            << " to " << dteam.getUnsigned("gid") << std::endl;
  catalog->setMode(home, 0775);
  std::cout << "Set g+w on " << home << std::endl;
  
  return 0;
}
