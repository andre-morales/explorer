#pragma once
#include <unordered_map>

template<class K, class V, auto defaultValue = 0> struct obs_unordered_map {
	std::unordered_map<K, V> map;

	typedef typename std::unordered_map<K, V>::iterator iterator_t;

	V operator[](const K& key) const {
		auto it = map.find(key);
		if(it != map.end()){
			return it->second;
		}
		return defaultValue;
	}
	template <class... Args>
	void emplace(Args&&... args){
		map.emplace(args...);
	}
	iterator_t find(const K& key){
		return map.find(key);
	}
	void erase(const K& k){
		map.erase(k);
	}
	void erase(const iterator_t& it){ map.erase(it); }
	void clear(){ map.clear(); }
	iterator_t begin(){ return map.begin(); }
	iterator_t end(){ return map.end(); }
};