#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

void parse(string letter, vector<char**> &vec_cmd, vector<char> &con);
bool execute(vector<char**>cmd, vector<char>con);
bool test(const char* flag, char* path);

void parse(string letter, vector<char**> &vec_cmd, vector<char> &con)
{
	vector<string> cmd;
	bool end_flag = false;
	bool bracket_flag = false;
	bool letter_flag = false;
	int i;
	while (letter_flag == false)
	{
		string temp;

		for (i = 0; i < letter.length(); i++)
		{
			end_flag = false;                                   				// If the end of the command has been reached

			if (letter[i] == '#')                               				// If start of a comment disregard rest of line
			{
				break;
			}
			if (letter[i] == ';')                               				// If the char is ';' push into the connector stack
			{
				end_flag = true;
				con.push_back(letter[i]);
			}
			else if(letter[i] == ' ')
			{
				end_flag = false;
			}
			else if(letter[i] == '[')
			{
				con.push_back(letter[i]);
			}
			else if(letter[i] == ']')
			{
				bracket_flag == true;
				i++;
			}
			else if (letter[i] == '&' || letter[i] == '|')      				// If the char is '&' or '|' push into the connector stack 
			{
				end_flag = true;
				con.push_back(letter[i]);
				i++;                                            				// Skip the duplicate '&' or '|'
			}
			else
			{
				// Nothing
			}
			if (end_flag == true || bracket_flag == true)                       // If the current letter is a connector 
			{
				bool clean = false;
				while( clean == false )
				{
					if (temp[0] == ' ')
					{
						temp.erase(0, 1);
						if (temp[0] == '[')
						{
							temp.erase(0, 1);
						}
					}
					else if(temp[0] == '[')
					{
						temp.erase(0,1);
						if (temp[0] == ' ')
						{
							temp.erase(0, 1);
						}
					}
					else if(temp[(temp.size() - 1)] == ' ')
					{
						temp.erase((temp.size() - 1), (temp.size()));
						if(temp[temp.size() - 1] == ']')
						{
							temp.erase((temp.size() - 1), (temp.size()));
						}
					}
					else if(temp[(temp.size() - 1)] == ']')
					{
						temp.erase((temp.size() - 1), (temp.size()));
						if(temp[temp.size() - 1] == ' ')
						{
							temp.erase((temp.size() - 1), (temp.size()));
						}
					}
					else
						clean = true;
				}
				cmd.push_back(temp);
				temp = "";
			}
			else
			{
				temp += letter[i];
			}
			
			end_flag = false;

		}
		if (temp[0] == ' ')                                      // Push back the last string that does not have a connector
		{
			temp.erase(0, 1);
		}
		else if(temp[0] == '[' && temp [1] != ' ')
		{
			temp.erase(0,1);
			temp.erase(temp.size() - 1, temp.size());
		}
		else if(temp[0] == '[' && temp [1] == ' ')
		{
			temp.erase(0,2);
			temp.erase(temp.size() - 2, temp.size());
		}
		cmd.push_back(temp);
		letter_flag = true;
	}

	//  // Print the vectors
	//  // cout << con.size() << endl;
	
	// for(int j = 0; j < cmd.size(); j++)
	// {
	//     cout << cmd.at(j) << endl;
	//     // if(j <= con.size())
	//     //     cout << con.at(j) << endl;
	//     // else
	//     // {
	//     //     // Nothing
	//     // }
	// }
	// for(int w = 0; w < con.size(); w++)
	// {
	// 	cout << con.at(w) << endl;
	// }
	
	
	// Parse the Parse ( Vector<string> cmd into Char * Array into Vector of the Char * Array )
	// bool cmd_flag = false;                      // While loop flag
	
	for (int k = 0; k < cmd.size(); k++)
	{
		// cout << cmd.at(k) << endl;
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
					// cout << "PUSHED" << endl;
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
			
			if(temp_hold[index] != '\0')
			{
				temp_hold[index]= '\0';                             			// Add Null Character on last string
			}
			
			temp.push_back(temp_hold);                         					// Add last string
			// cout << "PUSHED OUTSIDE" << endl;
			temp.push_back('\0');                               				// Have the last index be '\0'
			// cout << "PUSHED NULL" << endl;
			
			// temp_hold = new char[hold.length()];
			// index = 0;
		
		// for (int i = 0; i < temp.size(); i++)
		// {
		// 	cout << temp.at(i) << endl;
		// }
		// Then store the indexs into argv and push into a vector of argv
  		
  		// for(int k = 0; k < temp.size(); k++)
  		// {
  		// 	cout << temp.at(k) << endl;
  		// }
  		
		char** argv;
		argv = new char *[temp.size()];
		char *copy; 
		int count;
		
		// cout << endl << temp.size() << endl;
		
		// for ( int k = 0; k < temp.size(); k++)
		// {
		// 	cout << k << endl;
		// 	cout << temp.at(k) << endl;
		// }
// ROOT OF INPUT PARSING PROBLEMS...
//------------------------------------------------------------------------------------------------------------------		
 		int argv_count = 0;
 		for (int m = 0; m < (temp.size() - 1); m++)
 		{
 		    copy = temp.at(m);
 		    cout << copy << endl;	//<--comes out fine here
 		    count = strlen(copy);
 			argv[m] = new char [count];
 			strcpy ( argv[m], copy );	//<--something funny seems to happen here?
 			cout << argv[m] << endl;	//<--from [ -e /test/file/path ] to 0x1082e60??? :/   // ADDED the index - Mitch
 			argv_count++;
 		}
 		
 		if( argv[temp.size() - 1] != '\0')
 		{
 			string null = "\0";														// ADD NULL CHAR TO END OF ARGV
 			char *nullchar = new char [null.length()];
 			nullchar[0] = null[0];
 			count = strlen(nullchar);
 			argv[temp.size() - 1] = new char[count];
 			strcpy ( argv[temp.size() - 1], nullchar );
 			cout << "Before end of if statment that adds NULL to argv: " << argv[temp.size() - 1] << endl;	//<--not provoked // ADDED the index - Mitch
			argv_count++;
 		}
 		
 		// cout << "After if statement: " << argv << endl; //<--Still comes out weird here
 		// Need for loop here to print out argv
 		cout << "After if statement: " << endl;
 		for(int p = 0; p < argv_count; p++)
 		{
 			cout << p << endl;
 			cout << argv[p] << endl;
 		}
//-------------------------------------------------------------------------------------------------------------------

  		vec_cmd.push_back(argv);
  		
  		// cout << endl << "Pushed into vec_cmd" << endl;
  		// cmd_flag = false;
 	}


	// TEST PRINTS
 //   cout << endl << "vec_cmd Data:" << endl;
	// for (int w = 0; w < vec_cmd.size(); w++)
	// {
	// 	char ** test = vec_cmd.at(w);
	// 	cout << "Command Line #" << (w + 1) << endl;
	// 	for(int d = 0; test[d] != '\0' ; d++)
	// 	{
	// 		cout << d << endl;
	// 		cout << test[d] << endl;
	// 	}
	// 	cout << endl;
	// }
	
	// char ** test_t = vec_cmd.at(0);
	
	// for (int s = 0; test_t[s] != '\0'; s++)
	// {
	// 	cout << s << endl;
	// 	cout << test_t[s] << endl;
	// }
	
	// TEST SPECIFIC
	// cout << 2 << endl;
	// cout << test_t[2] << endl;
	bool ex = execute(vec_cmd, con);	// execute converted into a bool to accomodate precedence operators
	cout << "End\n";
	
}


 bool execute(vector<char**>cmd, vector<char>con)
 {
	int cmd_index = 0;
	int con_index = 0;
	pid_t pid;
    int status;
    bool executed;
    
	for (cmd_index = 0; cmd_index < cmd.size(); cmd_index++)	// Scan user input
	{
		executed = false;
		char **argv = new char *[cmd.size()];
		argv = cmd[cmd_index];
		cout << "argv inside execute(): " << argv << endl; //<--argv still comes out weird here :/
		if (con[con_index] == '[')
		{
			con_index++;
			string e = "-e";
			string f = "-f";
			string d = "-d";
			char* cd = new char[2];
			char* ce = new char[2];
			char* cf = new char[2];
			cd[0] = d[0];
			cd[1] = d[1];
			ce[0] = e[0];
			ce[1] = e[1];
			cf[0] = f[0];
			cf[1] = f[1];
			cout << "argv before test(): " << argv << endl;			
			//strcpy(cd,d.c_str());
			//strcpy(ce,e.c_str());
			//strcpy(cf,f.c_str());
			cout << cd << " " << ce << " " << cf << endl;
			cout << argv[0] << endl;
			if (/*argv[0] == cd ||*/ strcmp(argv[0],ce) == 0 /*|| argv[0] == cf*/)
			{
				cout << argv[0] << " " << argv[1] << endl;
				bool tested = test(argv[0], argv[1]);//<--function for test command
				if (!tested)
				{
					if (con[con_index] == '|')
            		{
            			con_index = con_index + 2;
            			goto skip;
            		}
            		else if (con[con_index] == ';')
            		{
            			con_index++;
            			goto skip;
        			}
        			else if (con[con_index] == '&')
        			{
        				con_index = con_index + 2;
        				break;
        			}
        			else
        			{
        				goto skip;
        			}
				}
				else
				{
					executed = true;
					if (con[con_index] == '|')
					{
						con_index = con_index + 2;
						break;
					}
					else if (con[con_index] == ';')
					{
						con_index++;
						goto skip;
					}
					else
					{
						con_index = con_index + 2;
						goto skip;
					}
				}
			}
			else
			{
				char* hold = argv[0];
				if (hold[0] == '/')
				{
					
					string e = "-e";
					//char *flag;
					//strcpy(flag, e.c_str());
					bool tested = test(e.c_str(), argv[0]);
					if (!tested)	//<--function for test command
					{
						if (con[con_index] == '|')
            			{
            				con_index = con_index + 2;
            				goto skip;
            			}
            			else if (con[con_index] == ';')
            			{
            				con_index++;
            				goto skip;
        				}
        				else if (con[con_index] == '&')
        				{
        					con_index = con_index + 2;
        					break;
        				}
        				else
        				{
        					goto skip;
        				}
					}
					else
					{
						executed = true;
						if (con[con_index] == '|')
						{
							con_index = con_index + 2;
							break;
						}
						else if (con[con_index] == ';')
						{
							con_index++;
							goto skip;
						}
						else
						{
							con_index = con_index + 2;
							goto skip;
						}
					}
				}
			}
		}
		cout << "argv before fork(): " << argv << endl;
        if ((pid = fork()) < 0)
        {
        	
            perror("ERROR");
            if(con[con_index] == '|')
            {
              con_index = con_index + 2;
              goto skip;
            }
            else if (con[con_index] == ';')
            {
            	con_index++;
            	goto skip;
            }
            else if (con[con_index] == '&')
            {
            	con_index = con_index + 2;
            	break;
            }
            else
            {
                goto skip;
            }
        }
        else if (pid == 0)
        {
        	cout << "argv after fork(): " << argv << endl;
            if (execvp(argv[0], argv) < 0)
            {
                perror("ERROR");
                if(con[con_index] == '|')
                {
                	con_index + 2;
                    goto skip;
                }
                else if (con[con_index] == ';')
                {
                	con_index++;
                	goto skip;
                }
                else if (con[con_index] == '&')
            	{
            		con_index = con_index + 2;
            		break;
            	}
                else
                {
                    goto skip;
                }
            }
        }
        else
        {
            while(wait(&status) != pid)
            {
	            if (con[con_index] == '|')
	            {
	            	con_index = con_index + 2;
	            	break;		// Break out if '|' is detected and previous command has succeeded
	            }
	            else if (con[con_index] == ';')
	            {
	            	con_index++;
	            	goto skip;
	            }
	            else
	            {
	            	con_index = con_index + 2;
	            	goto skip;
	            }
            }
            executed = true;
        }
        
        skip:
		delete [] argv;		// reset argument values
	}
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
	
	//strcpy(cd,d.c_str());
	//strcpy(cf,f.c_str());
	
	cout << "test: " << flag << " " << path << endl;	//<--check path and flag
	
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

int main()
{
    vector<char>con;
    vector<char**> vec_cmd;
    cout << "This is our command shell." << endl;
    while (true)
    {
    	vec_cmd.clear();
    	con.clear();
	    cout << "$ ";
	    string input;
	    char *argv[64];
	    getline(cin, input);

		char *temp = new char[input.length() + 1];
		strcpy(temp, input.c_str());
	    if (strcmp(temp, "exit") == 0)
	    {
	    	exit(0);
	    }
	  	parse(input, vec_cmd, con);  
	}
      return 0;
}