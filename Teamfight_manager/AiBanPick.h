#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

struct Champion
{
    int Id;
    int Attack;
    int Defense;
    int Hp;
    float WinRate;
};

class AiBanPick
{
private:

    Champion highStatChampion;
    Champion secondStatChampion;

    std::vector<Champion> highStatChampions;
    std::vector<Champion> champions;

    int weight = 5;
    int winRateweight = 0.2;

    bool isAiMistake = false;

public:
    AiBanPick();
    ~AiBanPick();

    int AddStat(const Champion& champion);
    void printStats(const std::vector<Champion>& champions);
    Champion CompareHighStatChampion(const Champion& a, const Champion& b);
    Champion CompareSecondStatChampion(const Champion& a, const Champion& b);

    void isBan(const Champion& highStatChampion, bool isAiMistake);
    void isPick(const Champion& highStatChampion, bool isAiMistake);

    const std::vector<Champion>& getChampions() const
    {
        return champions;
    }
};

