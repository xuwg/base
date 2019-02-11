#ifndef StringOperate_h__
#define StringOperate_h__

#include <string>
#include <vector>

void StringSplit(std::string s,char splitchar,std::vector<std::string>& vec);
void Split(const std::string &s, const std::string &delim, std::vector< std::string > &oSplitVec);
void FilterSpace(char *pSrc);

#endif // StringOperate_h__