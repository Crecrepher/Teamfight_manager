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
    if (isAiMistake) // Ʈ�簡 �Ǹ� Ai �Ǽ� �۵�
    {
        std::cout << "\n AI Champion " << secondStatChampion.Id << "BanPick." << std::endl;
    }
    std::cout << "\n AI Champion " << highStatChampion.Id << "BanPick." << std::endl;
}

void AiBanPick::isPick(const Champion&, bool isMistake)
{
    if (isAiMistake) // Ʈ�簡 �Ǹ� Ai �Ǽ� �۵�
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
    // Ȯ��
    for (int i = 0; i < champions.size(); ++i)
    {
        std::cout << "Champion " << i + 1 << ": ���ݷ� - " << champions[i].Attack << ", ���� - " << champions[i].Defense
            << ", ü�� - " << champions[i].Hp << ", �·� - " << champions[i].WinRate << std::endl;
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

    return secondStatChampion; // �� ��°�� ���� ���� è�Ǿ� ��ȯ
}

// �ٵ� �ʹ� ���ŷ����ʳ�?