#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <vector>

using namespace std;

namespace utils
{
	class MemUtils
	{
		public:
			template<typename T>
			static void deleteVector(const vector<T>& data);
	};

	template<typename T>
	void MemUtils::deleteVector(const vector<T>& data)
	{
		typedef typename vector<T>::size_type vector_size_type;

		for(vector_size_type i = 0; i < data.size(); i++)
		{
			delete data.at(i);
		}
	}
}

#endif // MEMUTILS_H
