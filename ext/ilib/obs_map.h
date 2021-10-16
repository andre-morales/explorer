#pragma once
#include <map>

template<class K, class V, auto defaultValue = 0> struct obs_map {
	std::map<K, V> map;

	typedef typename std::map<K, V>::iterator iterator_t;

	V operator[](const K& key){
		auto it = map.find(key);
		if(it != map.end()){
			return map[key];
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
	void erase(const iterator_t& it){
		map.erase(it);
	}
	void clear(){ map.clear(); }
	iterator_t begin(){ return map.begin(); }
	iterator_t end(){ return map.end(); }
};
