#ifndef FREQUENCY_MAP
#define FREQUENCY_MAP

/*
* @author: KingEze
* @mail: kingeze@mail.ustc.edu.cn
* @date: 2020/12/11 6:32 AM
* @description: find the frequency of each element in the input stream and store in a map type
*/

#include <map>
#include<fstream>

typedef std::map<char, int> FrequencyMap;

FrequencyMap initFrequencyMap(std::ifstream& in) {
    // Basic Conditons
    if (!in.is_open())
        return FrequencyMap();
    FrequencyMap freqMap;

    // get each char frequency
    while (true) {
        char ch = in.get();
        // find current char
        FrequencyMap::iterator it = freqMap.find(ch);
        if (it == freqMap.end()) //not found
            freqMap.insert(std::pair<char, int>(ch, 1));
        else
            it->second++;  //freq++
        if (in.eof()) break; // in the end so EOF will be included in the map
    }
    return freqMap;
}

#endif // !FREQUENCY_MAP
