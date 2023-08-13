#pragma once
#include "Singleton.h"
#include "GameState.h"

class SkillMgr : public Singleton<SkillMgr>
{
	friend Singleton<SkillMgr>;
private:
	SkillMgr() = default;
	virtual ~SkillMgr() = default;

	std::unordered_map<int, Skill> Skills;
public:
	void Init();
	void LoadFromCsv(const std::string path);

};

#define SKILL_MGR (SkillMgr::Instance())
