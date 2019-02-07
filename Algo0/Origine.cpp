#include <iostream>
#include <numeric>
#include <vector>
#include <Windows.h>
#include <algorithm>
#include <thread>

using namespace std;

pair<float, float> getLine(const pair<int, int>& p1, const pair<int, int>& p2)
{
	float m = (float)(p1.second - p2.second) / (float)(p1.first - p2.first);
	float q = p1.second - m * p1.first;

	return make_pair(m, q);

}

class OutScreen 
{
public:
	int sW, sH;
	HANDLE hConsole = NULL;
	DWORD dw = 0;
	wchar_t* screen = nullptr;
	vector<int>* table;

	OutScreen(int sW_, int sH_) : sW(sW_), sH(sH_) 
	{
		hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleActiveScreenBuffer(hConsole);
		screen = new wchar_t[sW*sH];
		table = new vector<int>(sW*sH, 0);
	};

	~OutScreen() 
	{
		delete screen;
	}

	void DrawPoint(const int& x,const int& y, int val=1)
	{
		if (x >= 0 && x*4 < sW && y >= 0 && y*4 < sH) 
			table->at(y*4*sW + x*4) = val;
	}

	void DrawLine(const pair<int, int>& a, const pair<int, int>& b, int val)
	{
		int x, y, px, py, xe, ye;
		int dx = a.first*4 - b.first*4;
		int dy = a.second*4 - b.second*4;

		if (abs(dx) >= abs(dy)) {

			if (dx >= 0)
			{
				x = b.first * 4;
				xe = a.first * 4;
				y = b.second * 4;
				ye = a.second * 4;
			}
			else
			{
				x = a.first * 4;
				xe = b.first * 4;
				y = a.second * 4;
				ye = b.second * 4;
			}

			float error = 0.;
			while (x<=xe)
			{
				table->at((y)* sW + x) = val;
				error += abs((float)dy / (float)dx);
				if (error >= 0.5)
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
					error -= 1.;
				}
				x++;
			}

		
			
				
				
			
		}
		else {
			
			if (dy >= 0)
			{
				x = b.first * 4;
				xe = a.first * 4;
				y = b.second * 4;
				ye = a.second * 4;
			}
			else
			{
				x = a.first * 4;
				xe = b.first * 4;
				y = a.second * 4;
				ye = b.second * 4;
			}

			float error = 0.;
			while (y<=ye)
			{
				table->at((y)* sW + x) = val;
				error += abs((float)dx / (float)dy);
				if (error >= 0.5)
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
					error -= 1.;
				}
				y++;
			}
				

			
		}
		
	}

	void Update() 
	{
		for (int y = 0; y < sH; y++) 
		{
			for (int x = 0; x < sW; x++) {
				switch (table->at(y*sW + x))
				{
				case 0:
					screen[y*sW + x] = L' ';
					break;
				case 1:
					screen[y*sW + x] = 0x2592;
					break;
				case 2:
					screen[y*sW + x] = 0x2588;
					break;
				case 3:
					screen[y*sW + x] = 0x2588;
					break;
				default:
					screen[y*sW + x] = L' ';
					break;
				}
				
			}
		}
		WriteConsoleOutputCharacter(hConsole, screen, sW*sH, {0,0}, &dw);
	}

};



bool checkSeparate(const vector<pair<int,int>>& v, const pair<int,int> a, const pair<int, int> b)
{
	int tmp = 0, min = 0, max = 0, tot = 0, th = 0;
	if (a.first != b.first && a.second != b.second)
	{
		for (auto& elem : v)
		{
			auto coeff = getLine(a, b);
			auto tmp = (coeff.first*elem.first + coeff.second - elem.second);
			if (tmp <= 0)
				min++;
			if (tmp >= 0)
				max++;
			tot++;
		}
	}
	else if (a.first != b.first && a.second == b.second)
	{
		for (auto& elem : v)
		{
			if (elem.second <= a.second)
				min++;
			if (elem.second >= a.second)
				max++;
			tot++;
		}
	}
	else if (a.first == b.first && a.second != b.second)
	{
		for (auto& elem : v)
		{
			if (elem.first <= a.first)
				min++;
			if (elem.first >= a.first)
				max++;
			tot++;
		}
	}
	return tot == max || tot == min;
}


int main() 
{	
	OutScreen os(120, 30);
	vector<pair<int, int>> points = { {0,3},{1,1},{2,2},{4,4},
	{0,0},{1,2},{3,1},{3,3}, {5,5} };
	vector<pair<int, int>> allPoints(points.begin(), points.end());
	vector<pair<int, int>> convexHull;
	
	int min_index = 0;
	int min_value= points[0].first;
	os.DrawPoint(points[0].first, points[0].second);
	

	for(int i = 1; i < points.size(); i++)
	{
		if (points[i].first < min_value)
		{
			min_value = points[i].first;
			min_index = i;

		}
		os.DrawPoint(points[i].first, points[i].second);
	}
		
	os.Update();

	convexHull.push_back(points[min_index]);
	points.erase(begin(points) + min_index);
	bool found = false;
	
	do
	{
		found = false;
		for (int i = 0; i < points.size(); i++)
		{
			os.DrawLine(convexHull[convexHull.size() - 1], points[i], 3);
			os.Update();
			this_thread::sleep_for(500ms);
			if (checkSeparate(allPoints, convexHull[convexHull.size() - 1], points[i]))
			{
				found = true;
				os.DrawLine(convexHull[convexHull.size() - 1], points[i], 2);
				convexHull.push_back(points[i]);
				points.erase(begin(points) + i);
				os.Update();
				this_thread::sleep_for(500ms);
				break;
			}
			os.DrawLine(convexHull[convexHull.size() - 1], points[i], 0);
			for (int j = 0; j < points.size(); j++) {
				os.DrawPoint(points[j].first, points[j].second);
			}
			for (int i = 0; i < convexHull.size() - 1; i++) {
				os.DrawLine(convexHull[i], convexHull[i + 1], 2);
			}
			

			os.Update();
			this_thread::sleep_for(500ms);
		}
		
	} while (found);


	/*for (int i = 0; i < convexHull.size() - 1; i++) {
		os.DrawLine(convexHull[i], convexHull[i + 1]);
		os.Update();
	}*/
	os.DrawLine(convexHull[convexHull.size() - 1], convexHull[0],2);

	os.Update();

	

	cout << "\n\nConvex Hull\n";
	for (auto& point : convexHull)
		cout << point.first << " " << point.second <<"\n";

	char c;
	cin >> c;

	return 0;
}