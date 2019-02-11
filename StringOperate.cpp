#include "StringOperate.h"

void StringSplit(std::string s,char splitchar,std::vector<std::string>& vec)
{  
	if(vec.size()>0)//保证vec是空的  
		vec.clear();  
	int length = s.length();  
	int start=0;  
	for(int i=0;i<length;i++)  
	{  
		if(s[i] == splitchar && i == 0)//第一个就遇到分割符  
		{  
			start += 1;  
		}  
		else if(s[i] == splitchar)  
		{  
			vec.push_back(s.substr(start,i - start));  
			start = i+1;  
		}  
		else if(i == length-1)//到达尾部  
		{  
			vec.push_back(s.substr(start,i+1 - start));  
		}  
	}  
}

void   FilterSpace(char *pSrc)
{
	char *pTemp = pSrc;
	if (pSrc == NULL )
	{
		return ;
	}
	while (*pSrc != '\0')
	{
		if (*pSrc != ' ')
		{
			(*pTemp) = (*pSrc);
			++pTemp;
		}
		++pSrc;
	}
	*pTemp = '\0';
}


//将参数一字符串中按照参数二的字符串截断，并且去掉参数二，将截断的各个部分保存到参数三vector中
void Split(const std::string &s, const std::string &delim, std::vector< std::string > &oSplitVec)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		oSplitVec.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		oSplitVec.push_back(s.substr(last, index - last));
	}
}