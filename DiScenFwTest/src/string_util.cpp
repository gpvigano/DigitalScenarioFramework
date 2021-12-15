//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace discenfw_test
{
	std::string DiffString(const std::string& str1, const std::string& str2)
	{
		std::string strFrag1 = str1;
		std::string strFrag2 = str2;
		/*
		for (unsigned i = 0; i < str1.size() && i < str2.size(); i++)
		{
			if (str1[i] != str2[i])
			{
				strFrag1 = str1.substr(i);
				strFrag2 = str2.substr(i);
				break;
			}
		}
		int i1 = strFrag1.size();
		int i2 = strFrag2.size();
		for (unsigned i = 0; i < strFrag1.size() && i < strFrag2.size(); i++)
		{
			i1 = strFrag1.size() - 1 - i;
			i2 = strFrag2.size() - 1 - i;
			if (strFrag1[i1] != strFrag2[i2])
			{
				break;
			}
		}
		strFrag1 = strFrag1.substr(0, i1);
		strFrag2 = strFrag2.substr(0, i2);*/

		std::vector<std::string> lines1;
		std::vector<std::string> lines2;
		std::string s;
		std::istringstream ss1(strFrag1);
		while (std::getline(ss1, s, '\n'))
		{
			lines1.push_back(s);
		}
		std::istringstream ss2(strFrag2);
		while (std::getline(ss2, s, '\n'))
		{
			lines2.push_back(s);
		}

		std::string str;
		for (unsigned i = 0; i < lines1.size() || i < lines2.size(); i++)
		{
			if (i < lines1.size() && i < lines2.size())
			{
				if (lines1[i] != lines2[i])
				{
					str += std::to_string(i+1) + "(1)>\t" + lines1[i] + "\n";
					str += std::to_string(i+1) + "(2)>\t" + lines2[i] + "\n\n";
				}
				else
				{
					str += std::to_string(i+1) + "     \t" + lines1[i] + "\n";
				}
			}
			else
			{
				if (lines1.size() > lines2.size())
				{
					str += std::to_string(i+1) + "(1)>\t" + lines1[i];
				}
				else
				{
					str += std::to_string(i+1) + "(2)>\t" + lines2[i];
				}
			}
		}
		//std::string str = strFrag1.empty() ? "2> " + strFrag2 : "1> " + strFrag1;
		return str;
	}

}

