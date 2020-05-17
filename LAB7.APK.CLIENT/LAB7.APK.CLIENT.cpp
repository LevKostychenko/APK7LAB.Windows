#include <iostream>
#include<stdio.h>
#include<windows.h>
#include "SerialPort.h"
#include<conio.h>
#include<time.h>
#include<string>
#include <vector>

using namespace std;
void GetApplicationList(SerialPort port);
string SelectApplication(vector<string> apps);
void SentChoice(string choice, SerialPort port);
char output[MAX_DATA_LENGTH];
char incoming[MAX_DATA_LENGTH];

int main()
{
	SerialPort com_port("COM1");
	if (com_port.isConnected())
	{
		cout << "Connection is established" << endl;
	}
	else
	{
		cout << "Cannot establish connection" << endl;
	}

	while (com_port.isConnected())
	{
		GetApplicationList(com_port);
	}
}



void GetApplicationList(SerialPort port)
{
	string choice;

	while (port.isConnected())
	{
		char* buff = new char[1];

		port.readSerialPort(buff, 1);

		if (buff[0] == '~')
		{
			vector<string> applications;
			string new_app;
			Sleep(100);
			port.readSerialPort(buff, 1);

			while (buff[0] != '\0')
			{				
				Sleep(100);
				if (buff[0] == '\r')
				{
					applications.push_back(new_app);
					new_app.clear();
					port.readSerialPort(buff, 1);
					continue;
				}

				new_app.push_back(buff[0]);

				port.readSerialPort(buff, 1);
			}

			choice = SelectApplication(applications);
			break;
		}
	}

	SentChoice(choice, port);
}

string SelectApplication(vector<string> apps)
{
	cout << "Select number of app to run on DOS: " << endl;
	int i = 1, choice = 0;

	for (string app : apps)
	{
		cout << to_string(i) << ") " << app << endl;
		i++;
	}

	while (choice <= 0 || choice > i)
	{
		cin >> choice;
	}

	return apps[choice - 1];
}

void SentChoice(string choice, SerialPort port)
{
	if (port.isConnected())
	{
		char* char_array = new char[choice.size() + 1];
		copy(choice.begin(), choice.end(), char_array);
		char_array[choice.size()] = '\0';

		port.writeSerialPort(char_array, MAX_DATA_LENGTH);
	}
	else
	{
		cout << "COM2 close the connection" << endl;
	}
}
