#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <functional>
#include <algorithm>
#include <future>
#include <filesystem>

/*
	Monitoring folder addition
*/

namespace fs = std::filesystem;
using namespace std::chrono_literals;
using namespace std;

class Users
{
private:
	string dirName;
	vector<string> activeUsers;

	vector<string> GetCurUsers()
	{
		vector<string> curUsers;

		for (const auto& entry : fs::directory_iterator(dirName))
		{
			curUsers.push_back(entry.path().generic_string().substr(dirName.size() + 1));
		}
		return curUsers;
	}

public:
	
	Users(string dirName)
	{
		this->dirName = dirName;
	};

	vector<string> PlusUser(vector<string>& curUsers)
	{
		vector<string> newUsers;
		for_each(curUsers.begin(), curUsers.end(), [&](const string& oldName)
			{
				if (ranges::find(activeUsers.begin(), activeUsers.end(), oldName) == activeUsers.end())
				{
					newUsers.push_back(oldName);
				}
			});
		return newUsers;
	}

	vector<string> MinusUser(vector<string>& curUsers)
	{
		vector<string> delUsers;
		for_each(activeUsers.begin(), activeUsers.end(), [&](const string& oldName)
			{
				if (ranges::find(curUsers.begin(), curUsers.end(), oldName) == curUsers.end())
				{
					delUsers.push_back(oldName);
				}
			});
		return delUsers;
	}

	void Try()
	{
		while (1)
		{
			auto curUsers = GetCurUsers();
			future<vector<string>> FutForPlus = async(&Users::PlusUser, this, ref(curUsers));
			future<vector<string>> FutForMinus = async(&Users::MinusUser, this, ref(curUsers));

			auto CurUsers = FutForPlus.get();

			if (CurUsers.size() != 0)
			{
				for (auto& user : CurUsers)
				{
					cout << "NEW: " << user << endl;
				}
			}

			auto delUsers = FutForMinus.get();

			if (delUsers.size() != 0)
			{
				for (auto& user : delUsers)
				{
					cout << "Del: " << user << endl;
				}			

				activeUsers = curUsers;
			}
		}
	}
};

int main()
{
	Users users(string("C:\\Use"));

	users.Try();
}