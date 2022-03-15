#include <web3cpp/Web3.h>

Web3::Web3(Utils::Provider *provider, boost::filesystem::path *path) : wallet(&defaultProvider, &defaultPath) {
  // We cannot set provider = defaultProvider due to the mutex inside the provider itself.
  boost::nowide::nowide_filesystem();
  defaultProvider.m_lock.lock();
  if (provider == NULL) {
    defaultProvider.networkName = "Avalanche Mainnet";
    defaultProvider.rpcUrl = "https://api.avax.network/";
    defaultProvider.rpcTarget = "/ext/bc/C/rpc";
    defaultProvider.rpcPort = 443;
    defaultProvider.chainID = 43114;
    defaultProvider.currencySymbol = "AVAX";
    defaultProvider.blockExplorerUrl = "";
  } else {
    defaultProvider.networkName = provider->networkName;
    defaultProvider.rpcUrl = provider->rpcUrl;
    defaultProvider.rpcTarget = provider->rpcTarget;
    defaultProvider.rpcPort = provider->rpcPort;
    defaultProvider.chainID = provider->chainID;
    defaultProvider.currencySymbol = provider->currencySymbol;
    defaultProvider.blockExplorerUrl = provider->blockExplorerUrl;
  }
  defaultProvider.m_lock.unlock();
  defaultPath = (path == NULL) ? Utils::getDefaultDataDir() : *path;
}

