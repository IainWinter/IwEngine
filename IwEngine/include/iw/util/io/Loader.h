#pragma once

namespace IwUtil {
	class ILoader {
	public:
		virtual ~ILoader() {}
	};

	template<
		typename _t>
	class Loader 
		: public ILoader
	{
	public:
		_t Load(const char* fileName);
	};

	template<
		typename _t>
	_t Load(const char* filePath) {
		return _t();
	}
}
