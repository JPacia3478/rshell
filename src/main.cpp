#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

void parse(string letter, vector<char**> &vec_cmd, vector<char> &con);
void execute(vector<char**>cmd, vector<char>con);

void parse(string letter, vector<char**> &vec_cmd, vector<char> &con)
{
	vector<string> cmd;
	bool end_flag = false;
	bool letter_flag = false;
	int i;

	while (letter_flag == false)
	{
		string temp;

		for (i = 0; i < letter.length(); i++)
		{
			end_flag = false;                                   // If the end of the command has been reached

			if (letter[i] == '#')                                // If start of a comment disregard rest of line
			{
				break;
			}
			if (letter[i] == ';')                                // If the char is ';' push into the connector stack
			{
				end_flag = true;
				con.push_back(letter[i]);
			}
			else if (letter[i] == '&' || letter[i] == '|')       // If the char is '&' or '|' push into the connector stack 
			{
				end_flag = true;
				con.push_back(letter[i]);
				i++;                                            // Skip the duplicate '&' or '|'
			}
			if (end_flag == true)                                // If the current letter is a connector 
			{
				if (temp[0] == ' ')
				{
					temp.erase(0, 1);
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
		cmd.push_back(temp);
		letter_flag = true;
	}

	// Parse the Parse ( Vector<string> cmd into Char * Array into Vector of the Char * Array )
	bool cmd_flag = false;                      // While loop flag
	
	for (int k = 0; k < cmd.size(); k++)
	{
		// TEST
		// cout << cmd.at(k) << endl;
		string hold = cmd.at(k);                // Temp holder for the first command

		vector<char*> temp;                     // Vector for the single command line
		
		// LOOP - Parses the spaces in the single command line.
		while (cmd_flag == false)
		{
		    char *temp_hold = new char[hold.length()];
			int index = 0;                                          // Index for temp_hold
			
			for (int z = 0; z < hold.length(); z++)
			{
				if (hold[z] != ' ')
				{
					
					temp_hold[index] = hold[z];
					index++;
				}
				else
				{
					temp_hold[index] = '\0';                        // Add Null Character
					temp.push_back(temp_hold);                      // Push the array onto the vector
					temp_hold = new char[hold.length()];            // Reset the array
					index = 0;
				}
			}
			
			temp_hold[index]= '\0';                             // Add Null Character on last string
			temp.push_back(temp_hold);                          // Add last string
			temp.push_back('\0');                               // Have the last index be '\0'
			temp_hold = new char[hold.length()];
			index = 0;

			cmd_flag = true;                                    // Exit while loop
		}
  		
		char** argv;
		argv = new char *[temp.size()];
		char *copy; 
		int count;
		// TEST
		// cout << endl << temp.size() << endl;
		
 		for (int m = 0; m < (temp.size() - 1); m++)
 		{
 		    // ------------------- ERROR HERE -------------------------------------
 		    // Segmentation Fault when the lines below are added
 		    
 		    // cout << i << " ";
 		    copy = temp.at(m);
 		    count = strlen(copy);
 			argv[m] = new char [count];
 			strcpy ( argv[m], copy );
 		}
 		
 		cout << endl;
 		
 		// char * null = '\0';														// Add Null to the last index of Char**
 		// strcpy ( argv[temp.size()], temp.at(temp.size()) );
  		vec_cmd.push_back(argv);
  		// TEST
  		// cout << endl << "Pushed into vec_cmd" << endl;
  		cmd_flag = false;
 	}
	    execute(vec_cmd,con);  	
 }


 void execute(vector<char**>cmd, vector<char>con)
 {
	int cmd_index = 0;
	int con_index = 0;

	for (cmd_index = 0; cmd_index < cmd.size(); cmd_index++)
	{
			char **argv = new char *[cmd.size()];
		argv = cmd[cmd_index];
		pid_t pid;
        int status;
        if ((pid = fork()) < 0)
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
            else
            {
                exit(1);
            }
        }
        else if (pid == 0)
        {
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
                else
                {
                    exit(1);
                }
            }
        }
        else
        {
            while(wait(&status) != pid);
            if (con[con_index] == '|')
            {
            	break;
            }
        }
        skip:
		delete [] argv;
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