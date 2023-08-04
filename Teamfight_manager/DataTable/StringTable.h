#pragma once
#include "DataTable.h"

class StringTable : public DataTable
{
protected:
	std::vector<std::unordered_map<std::string, std::string>> tables;
public:
	StringTable() : DataTable(DataTable::Ids::String), tables((int)Languages::COUNT) {}
	virtual ~StringTable() override { Release(); }

	virtual bool Load() override;
	virtual void Release() override;

	const std::string& Get(const std::string& id, Languages lang)const;
	const std::wstring GetW(const std::string& id, Languages lang);
	DWORD convert_ansi_to_unicode_string(std::wstring& unicode, const char* ansi, const size_t ansi_size);
	const std::string& Get(const std::string& id)const
	{
		return Get(id, Variables::CurrntLang);
	}
	const std::wstring GetW(const std::string& id)
	{
		return GetW(id, Variables::CurrntLang);
	}

};

