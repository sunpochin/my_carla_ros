// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "CarlaGameInstance.h"

#include "Game/MockGameController.h"
#include "Server/ServerGameController.h"
#include "Settings/CarlaSettings.h"

#include <thread>

static uint32 GetNumberOfThreadsForRPCServer()
{
  return std::max(std::thread::hardware_concurrency(), 4u) - 2u;
}

UCarlaGameInstance::UCarlaGameInstance() {
  CarlaSettings = CreateDefaultSubobject<UCarlaSettings>(TEXT("CarlaSettings"));
  check(CarlaSettings != nullptr);
  CarlaSettings->LoadSettings();
  CarlaSettings->LogSettings();
}

UCarlaGameInstance::~UCarlaGameInstance() {}

void UCarlaGameInstance::InitializeGameControllerIfNotPresent(
    const FMockGameControllerSettings &MockControllerSettings)
{
  if (GameController == nullptr) {
    if (CarlaSettings->bUseNetworking) {
      GameController = MakeUnique<FServerGameController>(DataRouter);
    } else {
      GameController = MakeUnique<MockGameController>(DataRouter, MockControllerSettings);
      UE_LOG(LogCarla, Log, TEXT("Using mock CARLA controller"));
    }
  }
}

void UCarlaGameInstance::NotifyBeginEpisode(UCarlaEpisode &Episode)
{
  if (!bServerIsRunning)
  {
    Server.Start(CarlaSettings->WorldPort);
    Server.AsyncRun(GetNumberOfThreadsForRPCServer());
    bServerIsRunning = true;
  }
  Server.NotifyBeginEpisode(Episode);
}

void UCarlaGameInstance::NotifyEndEpisode()
{
  Server.NotifyEndEpisode();
}
