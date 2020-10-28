#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <cctype>
#include <algorithm>
#include <cmath>

using namespace std;

struct FA { //structure for the finite automata
	int nOfs;
	int sym;

	int transT[10][10] = { 0 };
} nfa, dfa;

void NFAinp()
{
	cout << "Input the number of states for the NFA: ";
	cin >> nfa.nOfs;

	cout << "Input the number of symbols: ";
	cin >> nfa.sym;
	nfa.sym++;

	cout << endl;
	cout << endl;


	if (nfa.nOfs < 1 || nfa.sym < 1)
	{
		cout << "Incorrect value of input. Terminating";
		exit(0);
	}

	cout << "Input transitions for the following \n('0' means phi or no transition for the input):\n\n";
	int i, j;
	int temp = 1;
	for (i = 0; i < nfa.nOfs; i++) //input for the nfa table
	{
		for (j = 0; j < nfa.sym; j++)
		{
			if (j == 0)
			{
				nfa.transT[i][0] = temp;
				cout << "Input " << (nfa.sym - 1) << " transitions for " << temp << ": ";
				continue;
			}

			cin >> nfa.transT[i][j];
		}
		temp++;
	}
}

void NFAout()
{
	cout << "\n\n---------  NFA table  ---------\n\n";
	for (int i = 0; i < nfa.nOfs; i++) //printing out the nfa table
	{
		if (i == 0)
		{
			cout << "\t| ";   //printing the lines for the table
			for (int k = 0; k < nfa.sym - 1; k++)
				cout << k << '\t';
			
			cout << endl;
			for (int k = 0; k < nfa.sym + 20; k++)
				cout << '-';
			
			cout << endl;
		}

		for (int j = 0; j < nfa.sym; j++)
		{
			if (j == 0)
				cout << nfa.transT[i][j] << '\t';
			else
				cout << nfa.transT[i][j] << '\t';

			if (j == 0)
				cout << "| ";
		}
		cout << endl;
	}
}

void multiStateCon(int a, int z)
{
	int a_len = trunc(log10(abs(a))) + 1; //to find the length of the integer
	int a_digs[10] = { 0 };

	int temp = a;

	for (int i = 0; i < a_len; i++)
	{
		a_digs[i] = temp % 10;
		temp = temp / 10;
	}

	int nfaStateNo[10] = { -1 };
	int nfaState_quant = 0;
	for (int i = 0; i < a_len; i++)
	{
		for (int j = 0; j < nfa.nOfs; j++)
			if (a_digs[i] == nfa.transT[j][0])
			{
				nfaStateNo[i] = j;
				nfaState_quant++;
				break;
			}
	}
	cout << endl;
	for (int i = 0; i < nfaState_quant; i++)
	{
		for (int j = 1; j < nfa.sym; j++)
		{	
			if (nfa.transT[i][j] == 0)
				continue;
			if (i > 0)
			{
				while (dfa.transT[z][j] % 10 != 0) 
					dfa.transT[z][j] *= 10;
				
				dfa.transT[z][j] += nfa.transT[nfaStateNo[i]][j];
				if (dfa.transT[z][j] % 11 == 0) dfa.transT[z][j] /= 11;
				continue;
			}
			dfa.transT[z][j] += nfa.transT[nfaStateNo[i]][j];
		}
	}
}

void convert(int z)
{
	int state = dfa.transT[z][0];
	int state_len = trunc(log10(abs(state))) + 1;
	int i, j;

	if (state_len > 1)
		multiStateCon(dfa.transT[z][0], z);

	else if (state_len == 1) //process to convert transitions which don't have multiple states
	{
		for (i = 0; i < nfa.nOfs; i++)
		{
			int s2 = nfa.transT[i][0];

			if (s2 == state)
			{
				for (j = 0; j < dfa.sym; j++)
					dfa.transT[z][j] = nfa.transT[i][j];
				return;
			}
		}
	}

	return;
}

void DFAcon()
{
	dfa.nOfs = nfa.nOfs;
	dfa.sym = nfa.sym;

	int i, j;
	int dead_flag = 0; // flag to check for phi transitions
	vector<int> temp;

	for (i = 0; i < dfa.sym; i++) //copying first line of nfa to dfa since they will be same
		dfa.transT[0][i] = nfa.transT[0][i];

	temp.push_back(nfa.transT[0][0]);

	for (i = 0; i < dfa.nOfs; i++)//selecting the required states for the dfa
	{
		for (j = 1; j < dfa.sym; j++){
			if (!(nfa.transT[i][j] == 0)){
				if (find(temp.begin(), temp.end(), nfa.transT[i][j]) != temp.end())
					continue;
				else
					temp.push_back(nfa.transT[i][j]);
				continue;
			}
			else
				dead_flag = 1;
		}
	}

	dfa.nOfs = temp.size();	 // copying required states for dfa table
	for (i = 0; i < dfa.nOfs; i++)
		dfa.transT[i][0] = temp[i];

	for (i = 1; i < dfa.nOfs; i++)       //-------------------main conversion
		convert(i);
	

	int tmp = 1;  //process to add dead or trap state for phi transitions
	if (dead_flag == 1){
		for (i = 0; i < nfa.nOfs; i++)
			tmp++;
		for (i = 0; i < dfa.sym; i++)
			dfa.transT[dfa.nOfs][i] = tmp;
	}

	for (i = 0; i < dfa.nOfs; i++) // replacing all '-' with the dead state
	{
		for (j = 1; j < dfa.sym; j++)
			if (dfa.transT[i][j] == 0)
				dfa.transT[i][j] = tmp;
		
	}

	if (dead_flag == 1) dfa.nOfs++; // increasing number of states to accomodate the dead state
}

void DFAout()
{

	cout << "\n\n---------  DFA table  ---------\n";

	for (int i = 0; i < dfa.nOfs; i++) //printing out the nfa table
	{
		if (i == 0)
		{
			cout << "\t| ";   //printing the lines for the table
			for (int k = 0; k < dfa.sym - 1; k++)
				cout << k << '\t';
			
			cout << endl;
			for (int k = 0; k < dfa.sym + 20; k++)
				cout << '-';
			cout << endl;
		}

		for (int j = 0; j < dfa.sym; j++)
		{
			if (j == 0)
				cout << dfa.transT[i][j] << '\t';
			else
				cout << dfa.transT[i][j] << '\t';
			if (j == 0) 
				cout << "| ";
		}
		cout << endl;
	}
}

int main()
{
	NFAinp();
	NFAout();

	DFAcon();
	DFAout();

	return 0;
}