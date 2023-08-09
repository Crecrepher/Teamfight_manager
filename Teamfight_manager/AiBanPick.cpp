#include "stdafx.h"
#include "AiBanPick.h"

AiBanPick::AiBanPick()
{
    champions =
    {
        {1, 10, 5, 80, 0.6}, // Champion 1~8
        {2, 8, 7, 90, 0.6},
        {3, 12, 3, 120, 0.6},
        {4, 6, 9, 110, 0.6},
        {5, 15, 2, 75, 0.6},
        {6, 11, 4, 140, 0.6},
        {7, 9, 6, 85, 0.6},
        {8, 7, 8, 100, 0.6},
    };
}

AiBanPick::~AiBanPick()
{
}

void AiBanPick::isBan(const Champion& highStatChampion, bool isMistake)
{
    if (isAiMistake) // 트루가 되면 Ai 실수 작동
    {
        std::cout << "\n AI Champion " << secondStatChampion.Id << "BanPick." << std::endl;
    }
    std::cout << "\n AI Champion " << highStatChampion.Id << "BanPick." << std::endl;
}

void AiBanPick::isPick(const Champion&, bool isMistake)
{
    if (isAiMistake) // 트루가 되면 Ai 실수 작동
    {
        std::cout << "\n AI Champion " << secondStatChampion.Id << "Pick." << std::endl;
    }
    std::cout << "\n AI Champion " << highStatChampion.Id << "Pick." << std::endl;
}

int AiBanPick::AddStat(const Champion& champion)
{
    return (champion.Attack * weight) + (champion.Defense * weight) + (champion.Hp / weight) + (champion.WinRate * weight);
}

void AiBanPick::printStats(const std::vector<Champion>& champions)
{
    // 확인
    for (int i = 0; i < champions.size(); ++i)
    {
        std::cout << "Champion " << i + 1 << ": 공격력 - " << champions[i].Attack << ", 방어력 - " << champions[i].Defense
            << ", 체력 - " << champions[i].Hp << ", 승률 - " << champions[i].WinRate << std::endl;
    }
}

Champion AiBanPick::CompareHighStatChampion(const Champion& a, const Champion& b)
{
    std::sort(champions.begin(), champions.end(), [this](const Champion& a, const Champion& b)
        {
        return AddStat(a) > AddStat(b);
        });
    return highStatChampion = champions.front();
}

Champion AiBanPick::CompareSecondStatChampion(const Champion& a, const Champion& b)
{
    std::sort(champions.begin(), champions.end(), [this](const Champion& a, const Champion& b)
        {
            return AddStat(a) > AddStat(b);
        });

    secondStatChampion = highStatChampions[1];

    return secondStatChampion; // 두 번째로 높은 스텟 챔피언 반환
}

// 근데 너무 번거롭지않나?