#ifndef ASSIGNMENT_PARTICLES_INTERPOLATOR
#define ASSIGNMENT_PARTICLES_INTERPOLATOR

#include <map>

namespace particle
{
	/**
	 * Class for interpolating over a number of values.
	 *
	 * Primarily used for a colour interpolator for particle effects.
	 */
	template <typename T>
	class Interpolator
	{
    public:
		typedef std::map<float, typename T> Map;
		typedef typename Map::iterator Iterator;
		typedef typename Map::const_iterator ConstIterator;

		Interpolator(const T& value = T())
		{
			defaultValue = value;
		}

		void addValue(float time, const T& value)
		{
			values[time] = value;
		}

		T getValue(float time) const
		{
			// Case 1. There are no elements
			if (values.size() < 1) return defaultValue;
			// Case 2. There is exactly one element
			if (values.size() == 1) return values.begin()->second;
			// Case 3. There are 2 or more elements

			// Search through the map, stop when we found values between time.
			ConstIterator iter0 = values.begin();
			ConstIterator iter1 = iter0;

			while (iter1 != values.end())
			{
				if (iter1->first > time) break;
				iter0 = iter1;
				++iter1;
			}
      
			if (iter1 == values.end())
			{
				return iter0->second;
			}

			// Linear Interpolate between iter0 and iter1
			float t0 = iter0->first;
			float t1 = iter1->first;

			float ratio = (time - t0) / (t1 - t0);

			const T& value0 = iter0->second;
			const T& value1 = iter1->second;

			return value0 * (1.0f - ratio) + (value1 * ratio);
		}
    private:
		Map values;
		T defaultValue;
	};
}

#endif