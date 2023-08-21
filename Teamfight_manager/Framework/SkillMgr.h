#pragma once
#include "Singleton.h"
#include "GameState.h"
#include "Champion.h"
#include "BuffState.h"

class SkillMgr : public Singleton<SkillMgr>
{
	friend Singleton<SkillMgr>;
private:
	SkillMgr() = default;
	virtual ~SkillMgr() = default;

	std::unordered_map<int, ChampionSkill> ChampionSkills;
	
public:
	void Init();
	void LoadFromCsv(const std::string path);

	ChampionSkill* GetSkill(const int code);

	void ActiveSkill(int code, Champion* champ);
	void passiveSkill(int code, Champion* champ);

	void ArcherSkill(Champion* champ);
	void ArcherUltiSkill(Champion* champ);
	void BerserkerSkill(Champion* champ);
	void FighterSkill(Champion* champ);
	void FighterUltiSkill(Champion* champ);
	void IcemageSkill(Champion* champ);
	void IcemageUltiSkill(Champion* champ);
	void KnightSkill(Champion* champ);
	void KnightUltiSkill(Champion* champ);
	void MagicknightSkill(Champion* champ);
	void MagicknightUltiSkill(Champion* champ);
	void MonkSkill(Champion* champ);
	void MonkUltiSkill(Champion* champ);
	void NinjaSkill(Champion* champ);
	void NinjaUltiSkill(Champion* champ);
	void PriestSkill(Champion* champ);
	void PriestUltiSkill(Champion* champ);
	void PyromancerSkill(Champion* champ);
	void PyromancerUltiSkill(Champion* champ);
	void PythonessSkill(Champion* champ);
	void PythonessUltiSkill(Champion* champ);
	void ShieldbearerUltiSkill(Champion* champ);
	void SoldierSkill(Champion* champ);
	void SoldierUltiSkill(Champion* champ);
	void SwordmanSkill(Champion* champ);
	void SwordmanUltiSkill(Champion* champ);
	void BerserKerUltiSkill(Champion* champ);
	void ShieldbearerSkill(Champion* champ);
};

#define SKILL_MGR (SkillMgr::Instance())
