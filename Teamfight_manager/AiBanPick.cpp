#include "stdafx.h"
#include "AiBanPick.h"

AiBanPick::AiBanPick()
{


    //champions =
    //{
    //    {1, 10, 5, 80, 0.6}, // Champion 1~8
    //    {2, 8, 7, 90, 0.6},
    //    {3, 12, 3, 120, 0.6},
    //    {4, 6, 9, 110, 0.6},
    //    {5, 15, 2, 75, 0.6},
    //    {6, 11, 4, 140, 0.6},
    //    {7, 9, 6, 85, 0.6},
    //    {8, 7, 8, 100, 0.6},
    //};
}

void AiBanPick::Init()
{
    champions.clear();
}

AiBanPick::~AiBanPick()
{

}

void AiBanPick::isBan(const State& highStatChampion, bool isMistake)
{
    if (isAiMistake) // 트루가 되면 Ai 실수 작동
    {
        std::cout << "\n AI Champion " << secondStatChampion.charId << "BanPick." << std::endl;
    }
    std::cout << "\n AI Champion " << highStatChampion.charId << "BanPick." << std::endl;
}

void AiBanPick::isPick(const State&, bool isMistake)
{
    if (isAiMistake) // 트루가 되면 Ai 실수 작동
    {
        std::cout << "\n AI Champion " << secondStatChampion.charId << "Pick." << std::endl;
    }
    std::cout << "\n AI Champion " << highStatChampion.charId << "Pick." << std::endl;
}

int AiBanPick::AddStat(const State& champion)
{
    return (champion.attack * weight) + (champion.defend * weight) + (champion.maxHp / weight) /*+ (champion.WinRate * weight)*/;
}

void AiBanPick::printStats(const std::vector<State>& champions)
{
    // 확인
    for (int i = 0; i < champions.size(); ++i)
    {
        std::cout << "Champion " << i + 1 << ": 공격력 - " << champions[i].attack << ", 방어력 - " << champions[i].defend
            << ", 체력 - " << champions[i].maxHp /*<< ", 승률 - " << champions[i].WinRate*/ << std::endl;
    }
}

State AiBanPick::CompareHighStatChampion(const State& a, const State& b)
{
    std::sort(champions.begin(), champions.end(), [this](const State& a, const State& b)
        {
        return AddStat(a) > AddStat(b);
        });

    return highStatChampion = champions.front();
}

State AiBanPick::CompareSecondStatChampion(const State& a, const State& b)
{
    std::sort(champions.begin(), champions.end(), [this](const State& a, const State& b)
        {
            return AddStat(a) > AddStat(b);
        });

    secondStatChampion = champions.front();

    return secondStatChampion; // 두 번째로 높은 스텟 챔피언 반환
}

int AiBanPick::CompareHighStatChampionIndex()
{
    int highStat = -1;
    State champ;

    for (int i = 0; i < champions.size(); ++i)
    {
        int currentStat = AddStat(champions[i]);
        if (currentStat > highStat)
        {
            highStat = currentStat;
            champ = champions[i];
        }
    }

    for (int i = 0; i < champIndex.size(); ++i)
    {
        if (champIndex[i] == champ.charId)
        {
            PickChamp(champ);
            return i;
        }
    }
}


void AiBanPick::PickChamp(State state)
{
    for (auto it = champions.begin(); it != champions.end(); ++it)
    {
        if ((*it).charId == state.charId)
        {
            it = champions.erase(it);
            return;
        }
    }
}

void AiBanPick::PickChamp(int i)
{

}


void AiBanPick::CompareNextStatChampion()
{

}


