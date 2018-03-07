#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include <map>
#include <experimental/filesystem>


#include "Module.h"

template <typename T>
class AssetManager : public Module
{
public:
	AssetManager();
	virtual ~AssetManager();

	T* GetAsset(const std::experimental::filesystem::path& path);
	void Subscribe(T* asset);

	void Release(T* asset);

	size_t Size() const;
	const std::experimental::filesystem::path& GetPath(T* asset) const;

protected:
	void Register(const std::experimental::filesystem::path& path, T* asset);
	virtual T * Load(const std::experimental::filesystem::path& path) = 0;
	virtual void Unload(T* asset) = 0;

private:
	typename std::map<std::experimental::filesystem::path, T*>::const_iterator FindAssetIteratorByReference(T * asset) const;

private:
	std::map<std::experimental::filesystem::path, T*> assets;
	std::map<T*,unsigned int> assetUsage;
};

#endif