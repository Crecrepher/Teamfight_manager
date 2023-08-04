#pragma once
#include "stdafx.h"

template <typename T>
class ObjectPool
{
protected:
	std::list<T*> pool;
	std::list<T*> useList;

	void CreateObjs(int count = 100);
public:
	ObjectPool() = default;
	~ObjectPool();

	void Init(int cacheSize=100);
	void Release();
	void Clear();	// pool로 전부 회수

	T* Get();
	void Return(T* obj);

	const std::list<T*>& GetPool() const;
	const std::list<T*>& GetUseList() const;

	std::function<void(T*)> OnCreate;
	//void를 반환하고 t*를 인자로 받는 함수
};

template<typename T>
inline void ObjectPool<T>::CreateObjs(int count)
{
	for (int  i = 0; i < count; ++i)
	{
		T* obj = new T();
		if (OnCreate != nullptr)
		{
			OnCreate(obj);
		}
		obj->Init();
		pool.push_back(obj);
	}
}

template<typename T>
inline ObjectPool<T>::~ObjectPool()
{
	Release();
}

template<typename T>
inline void ObjectPool<T>::Init(int cacheSize)
{
	Release();
	CreateObjs(cacheSize);
}

template<typename T>
inline void ObjectPool<T>::Release()
{
	Clear();
	for (auto obj : pool)
	{
		delete obj;
	}
	pool.clear();
}

template<typename T>
inline void ObjectPool<T>::Clear()
{
	for (auto obj : useList)
	{
		obj->SetActive(false);
		pool.push_back(obj);
	}
	useList.clear();
}

template<typename T>
inline T* ObjectPool<T>::Get()
{
	if (pool.empty())
	{
		CreateObjs();
	}
	T* obj = pool.front();
	pool.pop_front();

	obj->SetActive(true);
	obj->Reset();

	useList.push_back(obj);
	return obj;
}

template<typename T>
inline void ObjectPool<T>::Return(T* obj)
{
	if (std::find(useList.begin(), useList.end(), obj) == useList.end())
	{
		std::cout << "Return Error!!!" << std::endl;
		return;
	}
	useList.remove(obj);
	obj->SetActive(false);
	pool.push_back(obj);

}

template<typename T>
inline const std::list<T*>& ObjectPool<T>::GetPool() const
{
	return pool;
}

template<typename T>
inline const std::list<T*>& ObjectPool<T>::GetUseList() const
{
	return useList;
}
