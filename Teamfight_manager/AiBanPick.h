#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "GameState.h"
#include "ChampionMgr.h"

class AiBanPick
{
private:

    State highStatChampion;
    State secondStatChampion;

    std::vector<State> highStatChampions;
    std::vector<State> champions;
    std::vector<std::string> champIndex;

    int champCount = 14;
    int weight = 5;
    int winRateweight = 0.2;

    bool isAiMistake = false;

public:
    AiBanPick();
    ~AiBanPick();

    void Init();
    int AddStat(const State& champion);
    void printStats(const std::vector<State>& champions);
    State CompareHighStatChampion(const State& a, const State& b);
    State CompareSecondStatChampion(const State& a, const State& b);

    int CompareHighStatChampionIndex(const std::vector<int>& banChamps);
    int CompareNextStatChampion(std::vector<int>& banChamps);

    void isBan(const State& highStatChampion, bool isAiMistake);
    void isPick(const State& highStatChampion, bool isAiMistake);

    void SetChampions()
    {
        champions = std::vector<State>(0);
        for (int i = 0; i < champCount; i++)
        {
            champions.push_back(*CHAMPION_MGR.GetChampion(i));
        }
    }

    
    void PickChamp(State state);

    const std::vector<State>& GetChampions() const
    {
        return champions;
    }



    void SetChampionIndex()
    {
        for (int i = 0; i < champCount; i++)
        {
            champIndex.push_back(CHAMPION_MGR.GetChampion(i)->charId);
        }
    }
};

