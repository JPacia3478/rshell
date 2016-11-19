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

void parse(string letter, vector<char**> &vec_cmd, vector<char**> &para_cmd, vector<char> &p_con, vector<char> &con);
bool execute(char **argv/*vector<char**>cmd, vector<char>con*/);
bool test(const char* flag, char* path);
void evaluate(vector<char**>reg_cmd, vector<char**>para_cmd, vector<char>p_con, vector<char>con);

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
				// con.push_back(letter[i]);
				if(para_flag == false || para_end == true)
					con.push_back(letter[i]);
				else
					p_con.push_back(letter[i]);
				i++;
			}
			else if(letter[i] == ']')
			{
				bracket_flag == true;
				// con.push_back(letter[i]);
				i++;
			}
			else if (letter[i] == '&' || letter[i] == '|')      				// If the char is '&' or '|' push into the connector stack 
			{
				end_flag = true;
				// con.push_back(letter[i]);
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
						// if (temp[0] == '[')
						// {
						// 	temp.erase(0, 1);
						// }
						// else if(temp[0] == ' ')
						// {
						// 	temp.erase(0,1);
						// }
					}
					else if(temp[temp.size() - 1] == '|' || temp[temp.size() - 1] == '&' || temp[temp.size() - 1] == ' ')
					{
						temp.erase(temp.size() - 1, temp.size());
					}
					// else if(temp[0] == '[')
					// {
					// 	temp.erase(0,1);
					// 	if (temp[0] == ' ')
					// 	{
					// 		temp.erase(0, 1);
					// 	}
					// }
					// else if(temp[(temp.size() - 1)] == ' ')
					// {
					// 	temp.erase((temp.size() - 1), (temp.size()));
					// 	if(temp[temp.size() - 1] == ']')
					// 	{
					// 		temp.erase((temp.size() - 1), (temp.size()));
					// 	}
					// }
					// else if(temp[(temp.size() - 1)] == ']')
					// {
					// 	temp.erase((temp.size() - 1), (temp.size()));
					// 	if(temp[temp.size() - 1] == ' ')
					// 	{
					// 		temp.erase((temp.size() - 1), (temp.size()));
					// 	}
					// }
					else
						clean = true;
				}
				if((para_flag == true && end_flag == true) || (para_flag == true && bracket_flag == true) || (para_flag == false && para_end == true))
				{
					// cout << "PUSHED PARA VEC" << endl;
					p_cmd.push_back(temp);
					para_end = false;
				}
				else if (para_flag == false && para_end == false)
				{
					// cout << "PUSHED REG VEC" << endl;
					cmd.push_back(temp);
				}
				temp = "";
			}
			else
			{
				temp += letter[i];
			}
			
			end_flag = false;
			// bracket_flag = false;
			// para_end = false;
			
		}
		bool clean = false;
		while (clean == false)
		{
			clean = false;
			if (temp[0] == ' ')                                      // Push back the last string that does not have a connector
			{
				temp.erase(0, 1);
			}
			else if(temp[temp.size() - 1] == '|' || temp[temp.size() - 1] == '&' || temp[temp.size() - 1] == ' ')
			{
				temp.erase(temp.size() - 1, temp.size());
			}
			else
			{
				clean = true;
			}
		}
		
		if((temp != "" || temp[0] != ' ') && para_end == false)
			cmd.push_back(temp);
		else if((temp != "" || temp[0] != ' ') && para_end == true)
			p_cmd.push_back(temp);
		letter_flag = true;
	
	}

// -------------------------- TEST ---------------------------------------------
	//  // Print the vectors ---------------------------------------------------
	
	// cout << endl << "Reg Commands: " << endl;
	// for(int j = 0; j < cmd.size(); j++)
	// {
	//     cout << j << " " << cmd.at(j) << endl;
	// }
	// cout << endl << "Paranthesis Commands: " << endl;
	// for(int u = 0; u < p_cmd.size(); u++)
	// {
		
	// 	cout << u << " " << p_cmd.at(u) << endl;
	// }
	// cout << endl << "Reg Connectors: " << endl;
	// for(int w = 0; w < con.size(); w++)
	// {
	// 	cout << w << " " << con.at(w) << endl;
	// }
	
	// cout << endl << "Paranthesis Connectors: " << endl;
	// for(int p = 0; p < p_con.size(); p++)
	// {
	// 	cout << p << " " << p_con.at(p) << endl;
	// }

// -----------------------------------------------------------------------------	
	
	// Parse the Parse ( Vector<string> cmd into Char * Array into Vector of the Char * Array )
	// bool cmd_flag = false;                      // While loop flag
	
	 
	// // BREAK -----------------------------------------------------------------------------------
	for (int k = 0; k < cmd.size(); k++)
	{
		cout << cmd.at(k) << endl;
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
			
			if(temp_hold[index] != '\0' && temp_hold[0] != ' ')
			{
				temp_hold[index]= '\0';                             			// Add Null Character on last string
			}
			
			if(temp_hold[0] != ' ')
				temp.push_back(temp_hold);                         				// Add last string
			// cout << "PUSHED OUTSIDE" << endl;
			temp.push_back('\0');                               				// Have the last index be '\0'
			// cout << "PUSHED NULL" << endl;
			
			
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
  		// cout << endl << "Pushed into vec_cmd" << endl;
  		// cmd_flag = false;
 	}
 	
 	// ADD NULL CHARACTER TO END OF MAIN VECTOR COMMAND
 	if(!vec_cmd.empty())
 		vec_cmd.push_back('\0');
 	
 	// PARANTHESIS -------------------------------------------------------------

	bool newfx = false;
 	for (int q = 0; q < p_cmd.size(); q++)
	{
		cout << p_cmd.at(q) << endl;
		string p_hold = p_cmd.at(q);                // Temp holder for the first command

		vector<char*> para_temp;                     // Vector for the single command line
		
		// LOOP - Parses the spaces in the single command line.

		    char *p_temp_hold = new char[p_hold.length()];
			int p_index = 0;                                          			// Index for temp_hold
			bool p_quote_flag = false;
			int p_quote_count = 0;
			// newfx = false;
			
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
					cout << p_hold[b] << endl;
					p_index++;
					newfx = false;
				}
				else
				{
					if(p_temp_hold[p_index] != '\0')
					{
						p_temp_hold[p_index] = '\0';                        		// Add Null Character
					}
					
					if(p_temp_hold[0] != ' ' && p_temp_hold[1] == '\0')
					{
						para_temp.push_back(p_temp_hold);                      		// Push the array onto the vector
						if(b != (p_hold.length() - 1))
						{
							p_temp_hold = new char[p_hold.length()];
							newfx = true;
						}
					}
					else if(p_temp_hold[0] != ' ')
					{
						para_temp.push_back(p_temp_hold);
						if(b != (p_hold.length() - 1))
						{
							p_temp_hold = new char[p_hold.length()];
							newfx = true;
						}
					}
					// cout << "PUSHED" << endl;
					
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
			// else if(p_temp_hold[1] != '\0' && p_temp_hold[0] != ' ')
			// {
			// 	para_temp.push_back(p_temp_hold);
			// }
			// cout << "PUSHED OUTSIDE" << endl;
			para_temp.push_back('\0');                               				// Have the last index be '\0'
			// cout << "PUSHED NULL" << endl;
			
			
		char** p_argv;
		p_argv = new char *[para_temp.size()];
		char *p_copy; 
		int p_count;
		
		// cout << endl << para_temp.size() << endl;
		
		// for ( int k = 0; k < temp.size(); k++)
		// {
		// 	cout << k << endl;
		// 	cout << temp.at(k) << endl;
		// }
		
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
 		
 		cout << endl;
 		
  		para_cmd.push_back(p_argv);
  		// cout << endl << "Pushed into para_cmd" << endl;
  		// cmd_flag = false;
 	}
 	
 	// ADD NULL CHARACTER TO END OF PARANTHESIS VECTOR COMMAND
 	if(!para_cmd.empty())
 		para_cmd.push_back('\0');


	// // TEST PRINTS
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
	
	// TEST MAIN COMMAND VECTOR PRINT -------------------------------------------
	
	// char ** test_t;
	
	// if (!vec_cmd.empty())
	// {
	// cout << endl << "Main Command Vector: " << endl;
	// cout << vec_cmd.size() << endl;
	
	// for(int g = 0; vec_cmd.at(g) != '\0'; g++)
	// {
	// 	test_t = vec_cmd.at(g);
		
	// 	cout << "Vec_cmd at " << g << endl;
	// 	for (int s = 0; test_t[s] != '\0'; s++)
	// 	{
	// 		cout << s << endl;
	// 		cout << test_t[s] << endl;
	// 		if(test_t[s+1] == '\0')
	// 		{
	// 			break;
	// 		}
	// 	}
	// 	if(vec_cmd.at(g + 1) == '\0')
	// 	{
	// 		break;
	// 	}
	// }
	// }
	// else
	// 	cout << "\nMain Command Vector IS EMPTY\n";
	// -------------------------------------------------------------------------
	
	// TEST PARANTHESIS VECTOR PRINT -------------------------------------------
	// if(!para_cmd.empty())
	// {
	// cout << endl << "Paranthesis Vector: " << endl;
	// // char ** test_t;
	
	// cout << para_cmd.size() << endl;
	
	// for(int g = 0; para_cmd.at(g) != '\0'; g++)
	// {
	// 	test_t = para_cmd.at(g);
		
	// 	cout << "Para_cmd at " << g << endl;
	// 	for (int s = 0; test_t[s] != '\0'; s++)
	// 	{
	// 		cout << s << endl;
	// 		cout << test_t[s] << endl;
	// 		if(test_t[s+1] == '\0')
	// 		{
	// 			break;
	// 		}
	// 	}
	// 	if(para_cmd.at(g + 1) == '\0')
	// 	{
	// 		break;
	// 	}
	// }
	// }
	// else
	// 	cout << "\nParanthesis Vector is EMPTY\n";
	
	// -------------------------------------------------------------------------
	
	// TEST SPECIFIC
	// cout << 2 << endl;
	// cout << test_t[2] << endl;
	
	cout << "\nEnd";
	
}


 bool execute(char **argv/*, vector<char>con*/)
 {
	//int cmd_index = 0;
	//int con_index = 0;
	pid_t pid;
    int status;
    bool executed;
    
//	for (cmd_index = 0; cmd_index < cmd.size(); cmd_index++)	// Scan user input
//	{
		executed = false;
		//char **argv = new char *[cmd.size()];
		//argv = cmd[cmd_index];
		//con_index++;
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
		string t = "test";
		char* ct = new char[4];
		for (int i = 0; i < 4; i++)
		{
			ct[i] = t[i];
		}
		//if(!con.empty())
		//{
			//if (con[con_index] == '[')
			//{
				if (strcmp(argv[0],cd) == 0 || strcmp(argv[0],ce) == 0 || strcmp(argv[0],cf) == 0)
				{
					//bool tested = test(argv[0], argv[1]);//<--function for test command
					if (test(argv[0], argv[1]))
					{
						executed = true;
					}
					// if (!tested)
					// {
					// 	if (con[con_index] == '|')
	    //         		{
	    //         			con_index = con_index + 2;
	    //         			goto skip;
	    //         		}
	    //         		else if (con[con_index] == ';')
	    //         		{
	    //         			con_index++;
	    //         			goto skip;
	    //     			}
	    //     			else if (con[con_index] == '&')
	    //     			{
	    //     				con_index = con_index + 2;
	    //     				break;
	    //     			}
	    //     			else
	    //     			{
	    //     				goto skip;
	    //     			}
					// }
					// else
					// {
					// 	executed = true;
					// 	if (con[con_index] == '|')
					// 	{
					// 		con_index = con_index + 2;
					// 		goto ret;
					// 	}
					// 	else if (con[con_index] == ';')
					// 	{
					// 		con_index++;
					// 		goto skip;
					// 	}
					// 	else
					// 	{
					// 		con_index = con_index + 2;
					// 		goto skip;
					// 	}
					// }
				}
			//}
			else if (strcmp(argv[0], ct) == 0)
			{
				if (strcmp(argv[1],cd) == 0 || strcmp(argv[1],ce) == 0 || strcmp(argv[1],cf) == 0)
				{
					//bool tested = test(argv[0], argv[1]);//<--function for test command
					if (test(argv[1], argv[2]))
					{
						executed = true;
					}
					// if (!tested)
					// {
					// 	if (con[con_index] == '|')
	    //         		{
	    //         			con_index = con_index + 2;
	    //         			goto skip;
	    //         		}
	    //         		else if (con[con_index] == ';')
	    //         		{
	    //         			con_index++;
	    //         			goto skip;
	    //     			}
	    //     			else if (con[con_index] == '&')
	    //     			{
	    //     				con_index = con_index + 2;
	    //     				break;
	    //     			}
	    //     			else
	    //     			{
	    //     				goto skip;
	    //     			}
					// }
					// else
					// {
					// 	executed = true;
					// 	if (con[con_index] == '|')
					// 	{
					// 		con_index = con_index + 2;
					// 		goto ret;
					// 	}
					// 	else if (con[con_index] == ';')
					// 	{
					// 		con_index++;
					// 		goto skip;
					// 	}
					// 	else
					// 	{
					// 		con_index = con_index + 2;
					// 		goto skip;
					// 	}
					// }
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

					bool tested = test(flag, argv[0]);
					if (test(flag, argv[0]))
					{
						executed = true;
					}
					// if (!tested)	//<--function for test command
					// {
					// 	if(!con.empty())
					// 	{
					// 		if (con[con_index] == '|')
	    //         			{
	    //         				con_index = con_index + 2;
	    //         				goto skip;
	    //         			}
	    //         			else if (con[con_index] == ';')
	    //         			{
	    //         				con_index++;
	    //         				goto skip;
	    //     				}
	    //     				else if (con[con_index] == '&')
	    //     				{
	    //     					con_index = con_index + 2;
	    //     					break;
	    //     				}
					// 	}
     //   				else
     //   				{
     //   					goto skip;
     //   				}
					// }
					// else
					// {
					// 	executed = true;
					// 	if(!con.empty())
					// 	{
					// 		if (con[con_index] == '|')
					// 		{
					// 			con_index = con_index + 2;
					// 			goto ret;
					// 		}
					// 		else if (con[con_index] == ';')
					// 		{
					// 			con_index++;
					// 			goto skip;
					// 		}
					// 		else
					// 		{
					// 			con_index = con_index + 2;
					// 			goto skip;
					// 		}
					// 	}
					// 	else
					// 	{
					// 		goto skip;
					// 	}
					// }
				}
			}
		//}
        if ((pid = fork()) < 0)
        {
            perror("ERROR");
            // if (!con.empty())
            // {
	           // if(con[con_index] == '|')
	           // {
	           //   con_index = con_index + 2;
	           //   goto skip;
	           // }
	           // else if (con[con_index] == ';')
	           // {
	           // 	con_index++;
	           // 	goto skip;
	           // }
	           // else if (con[con_index] == '&')
	           // {
	           // 	con_index = con_index + 2;
	           // 	break;
	           // }
            // }
            // else
            // {
            //     goto skip;
            // }
        }
        else if (pid == 0)
        {
            if (execvp(argv[0], argv) < 0)
            {
                perror("ERROR");
              //  if (!con.empty())
              //  {
	             //   if(con[con_index] == '|')
	             //   {
	             //   	con_index + 2;
	             //       goto skip;
	             //   }
	             //   else if (con[con_index] == ';')
	             //   {
	             //   	con_index++;
	             //   	goto skip;
	             //   }
	             //   else if (con[con_index] == '&')
	            	// {
	            	// 	con_index = con_index + 2;
	            	// 	break;
	            	// }
              //  }
              //  else
              //  {
              //      goto skip;
              //  }
            }
        }
        else
        {
            while(wait(&status) != pid)
            {
            	// if (!con.empty())
            	// {
		           // if (con[con_index] == '|')
		           // {
		           // 	con_index = con_index + 2;
		           // 	goto ret;
		           // }
		           // else if (con[con_index] == ';')
		           // {
		           // 	con_index++;
		           // 	goto skip;
		           // }
		           // else
		           // {
		           // 	con_index = con_index + 2;
		           // 	goto skip;
		           // }
            	// }
            	// else
            	// {
            	// 	goto skip;
            	//     executed = true;	
            	// }
            }
			executed = true;
        }
        
        //skip:
		delete [] argv;		// reset argument values
//}
	//ret:
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
	//cout << "reg_cmd" << endl;
	//for (int i = 0; i < reg_cmd.size(); i++)
	//{
	//	cout << reg_cmd.at(i) << endl;
	//}
	// 	cout << "para_cmd" << endl;
	// for (int i = 0; i < para_cmd.size(); i++)
	// {
	// 	cout << para_cmd.at(i) << endl;
	// }
	// 	cout << "p_con" << endl;
	// for (int i = 0; i < p_con.size(); i++)
	// {
	// 	cout << p_con.at(i) << endl;
	// }
	// 	cout << "con" << endl;
	// for (int i = 0; i < con.size(); i++)
	// {
	// 	cout << con.at(i) << endl;
	// }
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
			if (p_con.at(pcon_index) == ')')
			{
				pcon_index++;
			}
			
			while (p_con.at(pcon_index) != ')')
			{
				if (p_con.at(pcon_index) == '(')
				{
					pcon_index++;
				}
				
				bool p_ex = execute(para_cmd.at(pcmd_index));
				
				if ((p_ex == false && p_con.at(pcon_index) == '&') || (p_ex == true && p_con.at(pcon_index) == '|'))
				{
					break;
				}
				if (pcmd_index < para_cmd.size() - 1)
				{
					pcmd_index++;
				}
				
				pcon_index++;
				
				p_outcomes.push_back(p_ex);
			}
			if (i < con.size())
			{
				if (con.at(i) == ';' || con.at(i) == '&' || con.at(i) == '|')
				{
					//bool tmp;
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
				//cout << reg_cmd.at(i) << endl;
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
			//cout << reg_cmd.at(i) << endl;
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