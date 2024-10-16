#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "json.hpp"
#include "strops.h"
#include "Network.h"
#include "SettingsConsts.h"
#include "crypto.h"
#include "System.h"
#include "P2PClient.h"
#include "FileManip.h"

#include <iostream>
#include <fstream>
#include <map>

using json = nlohmann::json;
namespace fs = std::filesystem;



//P2P p2p;


int SyncPending(P2P& p2p, int whichBlock);
int SyncBlock(P2P& p2p, int whichBlock);
int Sync(P2P& p2p, json& walletInfo);
json ReadProgramConfig();
int WriteProgramConfig();
int GetProgram(json& walletInfo);
float GetProgramLifeLeft();
bool IsChainValid(P2P& p2p, json& walletInfo);
std::string CalculateDifficulty(json& walletInfo);
json UpgradeBlock(json& b);
void CreateSuperblock();

#endif