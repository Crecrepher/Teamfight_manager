#pragma once
#include "Singleton.h"
#include "GameState.h"
#include "Champion.h"

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

};

#define SKILL_MGR (SkillMgr::Instance())
