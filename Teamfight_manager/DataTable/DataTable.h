#pragma once

class DataTable
{
public:
	enum class Ids
	{
		None = -1,
		String,
		Zombie
	};
	DataTable(const DataTable& other) = delete;
	bool operator== (const DataTable& other) const = delete;
protected:
	Ids tavleId = Ids::None;

	
public:
	static const int TotalIds = 1;

	DataTable(DataTable::Ids id) : tavleId(id) {};
	virtual ~DataTable() {}

	virtual bool Load() = 0;
	virtual void Release() = 0;
};

