#pragma once

namespace inferno::helpers {

template <class T>
class Singleton
{
public:
	static T& GetInstance()
	{
		static T instance;
		return instance;
	}

	Singleton( Singleton const& ) = delete;
	void operator=( Singleton const& ) = delete;
protected:
	Singleton() = default;
};

}
