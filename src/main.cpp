#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <pwd.h>

using namespace std;

void parse(string letter, vector<char**> &vec_cmd, vector<char**> &para_cmd, vector<char> &p_con, vector<char> &con);
bool execute(char **argv);
bool test(const char* flag, char* path);
void evaluate(vector<char**>reg_cmd, vector<char**>para_cmd, vector<char>p_con, vector<char>con);
bool cD(char *dir);

void parse(string letter, vector<char**> &vec_cmd, vector<char**> &para_cmd, vector<char> &p_con, vector<char> &con)
{
	vector<string> cmd;
	vector<string> p_cmd;
	bool end_flag = false;
	bool bracket_flag = false;
	bool para_flag = false;
	bool para_end = false;
	bool letter_flag = false;
	int i;
	int para_count = 0;

	while (letter_flag == false)
	{
		string temp;

		for (i = 0; i < letter.length(); i++)
		{
			end_flag = false;                                   				// If the end of the command has been reached
			
			if (letter[i] == '#')                               				// If start of a comment disregard rest of line
			{
				letter_flag = true;
				break;
			}
			if (letter[i] == '(')
			{
				para_flag = true;												// OPEN Paranthesis is found
				p_con.push_back(letter[i]);
				para_count++;
				i++;
			}
			else if (letter[i] == ')')											// CLOSED Paranthesis is found
			{
				p_con.push_back(letter[i]);
				para_flag = false;
				para_end = true;
				i++; // Skip
			}
			else if (letter[i] == ';')                               			// If the char is ';' push into the connector stack
			{
				end_flag = true;
				if(para_flag == false || para_end == true)
					con.push_back(letter[i]);
				else
					p_con.push_back(letter[i]);
				i++;
			}
			else if(letter[i] == ' ')
			{
				end_flag = false;
			}
			else if(letter[i] == '[')
			{
				if(para_flag == false || para_end == true)
					con.push_back(letter[i]);
				else
					p_con.push_back(letter[i]);
				i++;
			}
			else if(letter[i] == ']')
			{
				bracket_flag == true;
				i++;
			}
			else if (letter[i] == '&' || letter[i] == '|')      				// If the char is '&' or '|' push into the connector stack 
			{
				end_flag = true;
				if(para_flag == false || para_end == true)
					con.push_back(letter[i]);
				else
					p_con.push_back(letter[i]);
				i++;                                            				// Skip the duplicate '&' or '|'
			}
			else
			{
				// Nothing
			}
			
			// If the current letter is a connector ----------------------------
			// AND Clean up the spaces at the front and end of the string
			if (end_flag == true || bracket_flag == true)                        
			{
				bool clean = false;
				while( clean == false )
				{
					if (temp[0] == ' ')
					{
						temp.erase(0, 1);
					}
					else if(temp[temp.size() - 1] == '|' || temp[temp.size() - 1] == '&' || temp[temp.size() - 1] == ' ')
					{
						temp.erase(temp.size() - 1, temp.size());
					}
					else
						clean = true;
				}
				if((para_flag == true && end_flag == true) || (para_flag == true && bracket_flag == true) || (para_flag == false && para_end == true))
				{
					p_cmd.push_back(temp);
					para_end = false;
				}
				else if (para_flag == false && para_end == false)
				{
					cmd.push_back(temp);
				}
				temp = "";
			}
			else
			{
				temp += letter[i];
			}
			
			end_flag = false;
		}
		bool clean = false;
		while( clean == false )
		{
			if (temp[0] == ' ')
			{
				temp.erase(0, 1);
			}
			else if(temp[temp.size() - 1] == '|' || temp[temp.size() - 1] == '&' || temp[temp.size() - 1] == ' ')
			{
				temp.erase(temp.size() - 1, temp.size());
			}
			else
				clean = true;
		}
		
		if((temp != "" || temp[0] != ' ') && para_end == false)
			cmd.push_back(temp);
		else if((temp != "" || temp[0] != ' ') && para_end == true)
			p_cmd.push_back(temp);
		letter_flag = true;
	
	}
	
	// Parse the Parse ( Vector<string> cmd into Char * Array into Vector of the Char * Array )
	
	 
	// // BREAK -----------------------------------------------------------------------------------
	for (int k = 0; k < cmd.size(); k++)
	{
		string hold = cmd.at(k);                // Temp holder for the first command

		vector<char*> temp;                     // Vector for the single command line
		
		// LOOP - Parses the spaces in the single command line.

		    char *temp_hold = new char[hold.length()];
			int index = 0;                                          			// Index for temp_hold
			bool quote_flag = false;
			int quote_count = 0;
			
			for (int z = 0; z < hold.length(); z++)
			{
				if (hold[z] == '"')
				{
					temp_hold[index] = hold[z];
					index++;
					quote_count++;
					if(quote_count == 1)
						quote_flag = true;
					else
						quote_flag = false;
				}
				else if (quote_flag == true)
				{
					temp_hold[index] = hold[z];
					index++;
				}
				else if(z == (hold.length() - 1) && hold[z] == ' ')
				{
					quote_count = 0;
					quote_flag = false;
				}
				else if (hold[z] != ' ')
				{
					temp_hold[index] = hold[z];
					index++;
				}
				else
				{
					if(temp_hold[index] != '\0')
					{
						temp_hold[index] = '\0';                        		// Add Null Character
					}
					if(temp_hold[0] != ' ')
						temp.push_back(temp_hold);                      		// Push the array onto the vector
					temp_hold = new char[hold.length()];            			// Reset the array
					if(z != (hold.length() - 1))
					{
						index = 0;
						quote_count = 0;
						quote_flag = false;
					}
				}
			}
			
			if(temp_hold[index] != '\0' && temp_hold[0] != ' ')
			{
				temp_hold[index]= '\0';                             			// Add Null Character on last string
			}
			
			if(temp_hold[0] != ' ')
				temp.push_back(temp_hold);                         				// Add last string
				
			temp.push_back('\0');                               				// Have the last index be '\0'
			
		char** argv;
		argv = new char *[temp.size()];
		char *copy; 
		int count;
		
 		for (int m = 0; m < (temp.size() - 1); m++)
 		{
 		    copy = temp.at(m);
 		    count = strlen(copy);
 			argv[m] = new char [count];
 			strcpy ( argv[m], copy );
 		}
 		
 		if( argv[temp.size() - 1] != '\0')
 		{
 			string null = "\0";														// ADD NULL CHAR TO END OF ARGV
 			char *nullchar = new char [null.length()];
 			nullchar[0] = null[0];
 			count = strlen(nullchar);
 			argv[temp.size() - 1] = new char[count];
 			strcpy ( argv[temp.size() - 1], nullchar );
 		}
 		
 		cout << endl;
 		
  		vec_cmd.push_back(argv);
 	}
 	
 	// ADD NULL CHARACTER TO END OF MAIN VECTOR COMMAND
 	if(!vec_cmd.empty())
 		vec_cmd.push_back('\0');
 	
 	// PARANTHESIS -------------------------------------------------------------

	bool newfx = false;
 	for (int q = 0; q < p_cmd.size(); q++)
	{
		string p_hold = p_cmd.at(q);                							// Temp holder for the first command

		vector<char*> para_temp;                    							// Vector for the single command line
		
		// LOOP - Parses the spaces in the single command line.

		    char *p_temp_hold = new char[p_hold.length()];
			int p_index = 0;                                          			// Index for temp_hold
			bool p_quote_flag = false;
			int p_quote_count = 0;
			newfx = false;
			
			for (int b = 0; b < p_hold.length(); b++)
			{
				if (p_hold[b] == '"')
				{
					p_temp_hold[p_index] = p_hold[b];
					p_index++;
					p_quote_count++;
					if(p_quote_count == 1)
						p_quote_flag = true;
					else
						p_quote_flag = false;
				}
				else if (p_quote_flag == true)
				{
					p_temp_hold[p_index] = p_hold[b];
					p_index++;
				}
				else if(b == (p_hold.length() - 1) && p_hold[b] == ' ')
				{
					p_quote_count = 0;
					p_quote_flag = false;
				}
				else if (p_hold[b] != ' ')
				{
					p_temp_hold[p_index] = p_hold[b];
					p_index++;
					newfx = false;
				}
				else
				{
					if(p_temp_hold[p_index] != '\0' && b != (p_hold.length() - 1))
					{
						p_temp_hold[p_index] = '\0';                        		// Add Null Character
					}
					
					if(p_temp_hold[0] != ' ' && p_temp_hold[1] == '\0' && b != (p_hold.length() - 1))
					{
						para_temp.push_back(p_temp_hold);                      		// Push the array onto the vector
						if(b != (p_hold.length() - 1))
						{
							p_temp_hold = new char[p_hold.length()];
							newfx = true;
						}
					}
					else if(p_temp_hold[0] != ' ' && b != (p_hold.length() - 1))
					{
						para_temp.push_back(p_temp_hold);
						if(b != (p_hold.length() - 1))
						{
							p_temp_hold = new char[p_hold.length()];
							newfx = true;
						}
					}
					
					if(b != (p_hold.length() - 1))
					{
						p_index = 0;
						p_quote_count = 0;
						p_quote_flag = false;
					}
				}
			}
			
			if(newfx == false)
			{
				if(p_temp_hold[0] != ' ' && p_temp_hold[p_index] != '\0')
				{
					p_temp_hold[p_index] = '\0';                             			// Add Null Character on last string
				}
			
				if(p_temp_hold[1] == '\0' && p_temp_hold[0] != ' ')
				{
					para_temp.push_back(p_temp_hold);                         					// Add last string
				}
			}

			para_temp.push_back('\0');                               				// Have the last index be '\0'
			
		char** p_argv;
		p_argv = new char *[para_temp.size()];
		char *p_copy; 
		int p_count;
		
 		for (int m = 0; m < (para_temp.size() - 1); m++)
 		{
 		    
 		    p_copy = para_temp.at(m);
 		    p_count = strlen(p_copy);
 			p_argv[m] = new char [p_count];
 			strcpy ( p_argv[m], p_copy );
 		}
 		
 		if( p_argv[para_temp.size() - 1] != '\0')
 		{
 			string null = "\0";														// ADD NULL CHAR TO END OF ARGV
 			char *nullchar = new char [null.length()];
 			nullchar[0] = null[0];
 			p_count = strlen(nullchar);
 			p_argv[para_temp.size() - 1] = new char[p_count];
 			strcpy ( p_argv[para_temp.size() - 1], nullchar );
 		}
 		
  		para_cmd.push_back(p_argv);
 	}
 	
 	// ADD NULL CHARACTER TO END OF PARANTHESIS VECTOR COMMAND
 	if(!para_cmd.empty())
 		para_cmd.push_back('\0');
} // END OF PARSE FUNCTION


 bool execute(char **argv)
 {
	pid_t pid;
    int status;
    bool executed;
    
	executed = false;

	string e = "-e";
	string f = "-f";
	string d = "-d";
	string chd = "cd";
		
	char* cd = new char[2];
	char* ce = new char[2];
	char* cf = new char[2];
	char* cchd = new char[2];
		
	cd[0] = d[0];
	cd[1] = d[1];
	ce[0] = e[0];
	ce[1] = e[1];
	cf[0] = f[0];
	cf[1] = f[1];
	cchd[0] = chd[0];
	cchd[1] = chd[1];	
		
	string t = "test";
	char* ct = new char[4];
		
	for (int i = 0; i < 4; i++)
	{
		ct[i] = t[i];
	}

	if (strcmp(argv[0],cd) == 0 || strcmp(argv[0],ce) == 0 || strcmp(argv[0],cf) == 0)
	{
		if (test(argv[0], argv[1]))
		{
			executed = true;
		}
	}
	else if (strcmp(argv[0], ct) == 0)
	{
		if (strcmp(argv[1],cd) == 0 || strcmp(argv[1],ce) == 0 || strcmp(argv[1],cf) == 0)
		{
			if (test(argv[1], argv[2]))
			{
				executed = true;
			}
		}
	}
	else if (strcmp(argv[0], cchd) == 0)
	{
		if (cD(argv[1]))
		{
			executed = true;
		}
	}
	else
	{
		char* hold = argv[0];
				
		if (hold[0] == '/')
		{
			string e = "-e";
			char *flag = new char[2];
			flag[0] = e[0];
			flag[1] = e[0];
					
			if (test(flag, argv[0]))
			{
				executed = true;
			}
		}
		else
		{
		    if ((pid = fork()) < 0)
		    {
		        perror("ERROR");
		    }
		    else if (pid == 0)
		    {
		        if (execvp(argv[0], argv) < 0)
		        {
		            perror("ERROR");
		        }
		    }
		    else
		    {
		        while(wait(&status) != pid) {}
				executed = true;
		    }			
		}
	}

	delete [] argv;		// reset argument values
	
	return executed;
}

bool test(const char* flag, char* path)
{
	struct stat sb;
	string d = "-d";
	string f = "-f";
	
	char* cd = new char[2];
	char* cf = new char[2];
	
	cd[0] = d[0];
	cd[1] = d[1];
	cf[0] = f[0];
	cf[1] = f[1];
	
	if (stat(path, &sb) < 0)
	{
		perror("ERROR");
		cout << "(False)" << endl;
		return false;
	}
	else
	{
		if (strcmp(flag,cf) == 0)
		{
			if (S_ISREG(sb.st_mode) < 0)
			{
				perror("ERROR");
				cout << "(False)" << endl;
				return false;
			}
			else
			{
				cout << "(True)" << endl;
				return true;
			}
		}
		else if (strcmp(flag,cd) == 0)
		{
			if (S_ISDIR(sb.st_mode) < 0)
			{
				perror("ERROR");
				cout << "(False)" << endl;
				return false;
			}
			else
			{
				cout << "(True)" << endl;
				return true;
			}
		}
		else
		{
			cout << "(True)" << endl;
			return true;
		}
	}
}

void evaluate(vector<char**>reg_cmd, vector<char**>para_cmd, vector<char>p_con, vector<char>con)
{
	int p_count = 0;
	
	int pcmd_index = 0;
	int pcon_index = 0;
	int cmd_index = 0;
	int con_index = 0;
	
	bool tmp;
	
	vector<bool>p_outcomes;
	
	if (!para_cmd.empty())
	{
		for (int i = 0; i < p_con.size(); i++)
		{
			if (p_con.at(i) == '(')
			{
				p_count++;
			}
		}
		for (int i = 0; i < p_count; i++)
		{
			if (p_con.at(pcon_index) == ')' && pcon_index < p_con.size())
			{
				pcon_index++;
			}
			
			while (p_con.at(pcon_index) != ')' && pcon_index < p_con.size())
			{
				bool p_ex = execute(para_cmd.at(pcmd_index));
				p_outcomes.push_back(p_ex);
				if (p_outcomes.size() > 1)
				{
					if ((p_outcomes.at(p_outcomes.size() - 1) == false && p_con.at(pcon_index) == '&') 
						|| (p_outcomes.at(p_outcomes.size() - 1) == true && p_con.at(pcon_index) == '|'))
					{
						break;
					}
				}
				if (pcmd_index < para_cmd.size() - 1)
				{
					pcmd_index++;
				}
				
				pcon_index++;
			}
			if (i < con.size())
			{
				if (con.at(i) == ';' || con.at(i) == '&' || con.at(i) == '|')
				{
					int t_count = 0;
					int f_count = 0;
					if (p_outcomes.size() > 1)
					{
						for (int j = 0; j < p_outcomes.size() - 1; j++)
						{
							if (p_outcomes.at(j))
							{
								t_count++;
							}
							else
							{
								f_count++;
							}
						}
					}
					if (t_count >= f_count)
					{
						tmp = true;
					}
					else if (f_count > t_count)
					{
						tmp = false;
					}
					else
					{
						tmp = p_outcomes.at(0);
					}
				}
				if ((tmp == true && con.at(i) == '|') || (tmp == false && con.at(i) == '&'))
				{
					break;
				}			
			}
		}
	}
	if (con.empty() && !reg_cmd.empty())
	{
		bool b = execute(reg_cmd.at(0));
	}
	else if (!con.empty() && !reg_cmd.empty() && !para_cmd.empty())
	{
		if ((tmp == true && con.at(con_index) == '|') || (tmp == false && con.at(con_index) == '&'))
		{
			return;
		}
		else
		{
			for (int i = 0; i < reg_cmd.size() - 1; i++)
			{
				bool ex = execute(reg_cmd.at(i));
				if (!con.empty() && con_index < con.size())
				{
					if ((ex == true && con.at(con_index) == '|') || (ex == false && con.at(con_index) == '&'))
					{
						break;
					}
					else
					{
						if (con_index < con.size())
						{
							con_index++;
						}
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < reg_cmd.size() - 1; i++)
		{
			bool ex = execute(reg_cmd.at(i));
			if (!con.empty() && con_index < con.size())
			{
				if ((ex == true && con.at(con_index) == '|') || (ex == false && con.at(con_index) == '&'))
				{
					break;
				}
				else
				{
					if (con_index < con.size())
					{
						con_index++;
					}
				}
			}
		}
	}
}

bool cD(char *dir)
{

	string dash = "-";
	char *cdash = new char[1];
	bool changed = false;
	
	cdash[0] = dash[0];
	
	// If user input is "cd -"
	// Change to previous directory
	if (dir == '\0')
	{
		char *tmp_dir = getenv("PWD");
		if(getenv("PWD") == getenv("HOME"))
		{
			// Do Nothing
					cout << "e" << endl;
		}
		else if (chdir(getenv("HOME")) < 0 || setenv("PWD", getenv("HOME"), 1) < 0 || setenv("OLDPWD", tmp_dir, 1) < 1)
		{
			perror("cd home");
		}
		else
		{
			cout << "TOOK TO HOME" << endl;
			changed = true;
		}
	}
	else if (strcmp(dir, cdash) == 0)
	{
		char *tmp_dir = getenv("PWD");
		
		if (chdir(getenv("OLDPWD")) < 0 || setenv("PWD", getenv("OLDPWD"), 1) < 0 || setenv("OLDPWD", tmp_dir, 1) < 0)
		{
			perror("cd previous");
		}
		else
		{
			// setenv("PWD", getenv("OLDPWD"), 1);
			changed = true;
		}
	}

	// Else user input is "cd xxx/xxxx/xxx"
	// Change to inputted path
	else
	{
		// Hold current directory before modifying
		// If its the same directory
		cout << "ELSE" << endl;
			if (getenv("OLDPWD") == getenv("PWD"))
			{
				// Do Nothing
			}
			else
			{
				if (setenv("OLDPWD", getenv("PWD"), 1))
				{
					perror("cd save previous");
				}
			}
			
			if (chdir(dir) < 0 || setenv("PWD", dir, 1) < 0)
			{
				perror("cd");
			}
			else
			{
				setenv("PWD", dir, 1);
				changed = true;
			}
	}
	
	return changed;
}

int main()
{
    vector<char>con;
    vector<char>p_con;
    vector<char**> vec_cmd;
    vector<char**>para_cmd;

    while (true)
    {	   
    	string input;
  	    
    	vec_cmd.clear();
    	para_cmd.clear();
    	p_con.clear();
    	con.clear();
    	
	    cout << "$ ";

	    getline(cin, input);

	    if (input == "exit")
	    {
	    	exit(0);
	    }
	    
	  	parse(input, vec_cmd, para_cmd,p_con, con);
	  	evaluate(vec_cmd, para_cmd, p_con, con);
	}
      return 0;
}