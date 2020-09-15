#include "pch.h"
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::ios;
using std::left;
using std::right;
using std::showpoint;

#include <fstream> // file stream
using std::ifstream; // input file stream
using std::ofstream; // output file stream

#include <iomanip>
using std::setw;
using std::setprecision;

#include <string>
using std::string;

#include <cstdlib>
using std::exit; // exit function prototype

#include <vector> 
#include <algorithm>
using namespace std;

class Note
{
	public:
		Note();
		Note(float,int,int,int,int);
		~Note();
		int getType() { return type; };
		float getTime() { return time; };
		int getLane() { return lane; };
		int getSize() { return size; };
		int getId() { return id; };
		void setType(int t) { type = t; };
		void setTime(float t) { time = t; };
		void setLane(int l) { lane = l; };
		void setSize(int s) { size = s; };
		void setId(int i) { id = i; };
		bool operator > (const Note& str) const {
			if (time < str.time) return true;
			if (time > str.time) return false;
			if (lane < str.lane) return true;
			if (lane > str.lane) return false;
			if (type > str.type) return true;
			if (type < str.type) return false;
			return false;
		}

	private:
		int type;
		float time;
		int lane;
		int size;
		int id;
};

Note::Note()
{
	type = 0;
	time = 0;
	lane = 0;
	size = 0;
	id = 0;
}

Note::Note(float ti, int ty, int l, int s, int i)
{
	int type = 0;

	switch (ty) {
	case 1:
		type = 2;
		break;
	case 5:
		type = 4;
		break;
	case 7:
		type = 3;
		break;
	case 8:
		type = 6;
		break;
	case 9:
	case 11:
		type = 5;
		break;
	case 10:
		type = 7;
		break;
	default: break;
	}

	setType(type);
	setTime(ti);
	setLane(l-8);
	setSize(s-1);
	setId(i);
}

Note::~Note()
{
}

string in = "chart.txt";
string out = "entities.txt";

void holdClear(vector<Note>& notes, vector<Note>& holds) {
	int pos = 0;
	for (auto i = holds.begin(); i != holds.end(); ++i) {
		int posJ = 0;
		for (auto j = notes.begin(); j != notes.end();) {
			if ((notes.at(posJ).getTime() == holds.at(pos).getTime()) && (notes.at(posJ).getLane() == holds.at(pos).getLane())) {
				if (notes.at(posJ).getType() == 2) j = notes.erase(j);
				else if (notes.at(posJ).getType() == 4 && ((notes.at(posJ).getTime() == holds.at(pos).getTime()) && (notes.at(posJ).getLane() == holds.at(pos).getLane()))) {
					holds.at(pos).setType(7);
					j = notes.erase(j);
				}
			}
			else {
				++j;
				++posJ;
			}
			if (posJ == notes.size()) break;
			if (pos == holds.size()) break;
		}
		++pos;
	}
}

int main()
{
	cout << "Where is your .sus file?"<<endl;
	//cin >> in;
	cout << "Where is the destination of your file?" << endl;
	//cin >> out;

	// ifstream constructor opens the file
	ifstream inClientFile(in, ios::in);
	// ofstream constructor opens file
	ofstream outClientFile(out, ios::out);

	// exit program if ifstream could not open file
	if (!inClientFile)
	{
		cerr << "No read" << endl;
		exit(1);
	} // end if

	if (!outClientFile) // overloaded ! operator
	{
		cerr << "No write" << endl;
		exit(1);
	} // end if

	vector<string> file;
	vector<Note> chart;
	vector<Note> notes;
	vector<Note> holds0;
	vector<Note> holds1;

	string line;
	int filePos=0;
	int bpmPos = 0;
	int bpm = 0;

	while (inClientFile >> line) {
		file.push_back(line);
	}

	auto fpos = find(file.begin(), file.end(), "#MEASUREHS");
	auto bpmpos = find(file.begin(), file.end(), "#BPM01:");

	if(fpos!= file.end()){
		filePos = distance(file.begin(), fpos) + 2;
	}
	if (bpmpos != file.end()) {
		bpmPos = distance(file.begin(), bpmpos) + 1;
		bpm = stoi(file.at(bpmPos));
	}

	outClientFile << "[" << endl;

	cout << "Reading file..." << endl;

	for (auto i = filePos; i != file.size(); ++i) {	
		size_t pos = file.at(i).find(":");
		string pre = file.at(i).substr(0,pos);
		string post = file.at(i).substr(pos+1);
		if (pos == 6) {
			int seg = stoi(file.at(i).substr(1,3));
			int type = stoi(file.at(i).substr(4, 1));
			int lane = stoi(file.at(i).substr(5, 1), 0, 16);


			int count = post.size();

			for (int j = 0; j < count; j += 2) {
				int ntype = stoi(post.substr(j, 1));
				int size = stoi(post.substr(j+1, 1), 0, 16);
				if (type == 1 && ntype > 2)ntype = 0;
				if (type == 5 && (ntype == 2 || ntype > 4))ntype = 0;
				if (ntype != 0 && size != 0 && lane < 15) {
					float time = ((float)240 / (float)bpm) * ((float)seg + (float)j / (float)count);
					notes.push_back(Note(time,type,lane,size,0));
					//cout << type << " " << ntype << " " << setw(7)<< left << time << " " << lane << " " << size << endl;
				}
			}

		}
		if (pos == 7) {
			int seg = stoi(file.at(i).substr(1, 3));
			int type = stoi(file.at(i).substr(4, 1));
			int lane = stoi(file.at(i).substr(5, 1), 0, 16);
			int id = stoi(file.at(i).substr(6, 1));

			int count = post.size();

			for (int j = 0; j < count; j += 2) {
				int ntype = stoi(post.substr(j, 1));
				int size = stoi(post.substr(j + 1, 1), 0, 16);
				if (ntype != 0 && size != 0 && lane < 15) {
					float time = ((float)240 / (float)bpm) * ((float)seg + (float)j / (float)count);
					//cout << type << " " << ntype << " " << setw(7) << left << time << " " << lane << " " << size << " " << id << endl;
					if (id == 0) holds0.push_back(Note(time, (2 * type) + ntype, lane, size, id));
					if (id == 1) holds1.push_back(Note(time, (2 * type) + ntype, lane, size, id));
					//outClientFile << "{" << "\"archetype\":" << type << ",\"data\":{\"index\":1,\"values\":[" << time << "," << lane - 8 <<","<< size - 1 << "]}" << "}," << endl;
				}
			}
		}
	}

	cout << "Reading completed!" << endl;

	sort(notes.begin(),notes.end(),greater<Note>());
	sort(holds0.begin(), holds0.end(), greater<Note>());
	sort(holds1.begin(), holds1.end(), greater<Note>());

	cout << "Processing notes..." << endl;

	int pos = 0;
	for (auto i = notes.begin(); i != notes.end();) {
		if (notes.at(pos).getType() == 4 && (notes.at(pos).getTime() == notes.at(pos + 1).getTime())) {
			if (notes.at(pos).getLane() == notes.at(pos+1).getLane()) {
				i = notes.erase(i + 1);
			}
			else {
				++i;
			}
		}
		else {
			++i;
		}
		pos++;

		if (pos == notes.size()) break;
	}

	holdClear(notes, holds0);
	holdClear(notes, holds1);

	cout << "Processing Completed!" << endl;

	chart.insert(chart.end(), notes.begin(), notes.end());
	chart.insert(chart.end(), holds0.begin(), holds0.end());
	chart.insert(chart.end(), holds1.begin(), holds1.end());
	sort(chart.begin(), chart.end(), greater<Note>());

	pos = 0;
	int hold0pos = 0;
	int hold1pos = 0;
	for (auto i = chart.begin(); i != chart.end(); ++i) {
		if (chart.at(pos).getId() == 0) {
			if ((chart.at(pos).getType() == 5) || (chart.at(pos).getType() == 6) || (chart.at(pos).getType() == 7)) chart.at(pos).setId(hold0pos + 2);
			if ((chart.at(pos).getType() == 3) || (chart.at(pos).getType() == 5)) hold0pos = pos;
		}
		else if (chart.at(pos).getId() == 1) {
			if ((chart.at(pos).getType() == 5) || (chart.at(pos).getType() == 6) || (chart.at(pos).getType() == 7)) chart.at(pos).setId(hold1pos + 2);
			if ((chart.at(pos).getType() == 3) || (chart.at(pos).getType() == 5)) hold1pos = pos;
		}
		//cout << chart.at(pos).getType() << " " << setw(7) << left << chart.at(pos).getTime() << " " << chart.at(pos).getLane() << " " << chart.at(pos).getSize() << " " << chart.at(pos).getId() << endl;
		++pos;
	}


	pos = 0;
	for (auto i = chart.begin(); i != chart.end(); ++i) {
		if (chart.at(pos).getType() > 3) {
			outClientFile << "{" << "\"archetype\":" << chart.at(pos).getType() << ",\"data\":{\"index\":0,\"values\":[" << chart.at(pos).getId() << "," << chart.at(pos).getTime() << "," << chart.at(pos).getLane() << "," << chart.at(pos).getSize() << "]}" << "}" << endl;
		}
		else outClientFile << "{" << "\"archetype\":" << chart.at(pos).getType() << ",\"data\":{\"index\":1,\"values\":[" << chart.at(pos).getTime() << "," << chart.at(pos).getLane() << "," << chart.at(pos).getSize() << "]}" << "}" << endl;
		//cout << notes.at(pos).getType() << " " << ntype << " " << setw(7) << left << time << " " << lane << " " << size << endl;
		pos++;
		if (pos < chart.size()) outClientFile << "," << endl;
		//if (pos == notes.size()) break;
	}

	outClientFile << "]" << endl;

	cout << "You can now find your files at: " << out << endl;
	
	return 0;
}

