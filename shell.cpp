#include<iostream>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<algorithm>
#include <assert.h>
#include <time.h>
using namespace std;
string trim(string input) {
	int i = 0;
	//while (i < input.size() && (input [i] == ' ' | input [i] == '\''))

	while (i < input.size() && (input[i] == ' ')) {
		if (input[i] == '\'') {
			break;
		}
		i++;
	}

	if (i < input.size())
		input = input.substr(i);
	else {
		return "";
	}
	
	i = input.size() - 1;
	while (i >= 0 && input[i] == ' '){
		if (input[i] == '\'') {
			break;
		}
		i--;
	}
    if (i >= 0)
        input = input.substr (0, i+1);
    else
        return "";
    
    return input;
    
}

vector<string> split(string line, string separator = " ") {
	vector<string> result;

	int sing_quote_num = count(line.begin(), line.end(), '\'');
	int doub_quote_num = count(line.begin(), line.end(), '\"');
	int total_quotes = sing_quote_num + doub_quote_num;
	int i = 0;
	vector<size_t> found_quote;
	size_t start = 0;
	char doub_token[] = "\"";
	char sing_token[] = "'";
	char look_for = separator[0];
	//char look_for = ' ';
	char replace_for = look_for + 150;
	if (total_quotes > 0) {
		while (i < doub_quote_num) {
			found_quote.push_back(line.find(doub_token, start));
			start = found_quote[i] + 1;
			i++;
		}
		//int h = i;
		if (found_quote.size() == 0)
		{
			i = 0;
		}
		start = 0;
		while (i < (total_quotes)) {
			found_quote.push_back(line.find(sing_token, start));
			start = found_quote[i] + 1;
			i++;
			//h++;
		}
	}

	int num_replacement = 0;
	if (total_quotes % 2 == 0) {
		for (int i = 0; i < ((total_quotes) / 2); i++) {
			//cout << "loop from " << found_quote[i] << endl;
			//cout << "loop to " << found_quote[i + 1] << endl;
			if (i != 0) {
				i++;
			}
			for (int j = found_quote[i]; j < found_quote[i + 1]; j++) {
				if (line[j] == look_for) { //token instead of ' '
					line[j] = replace_for;
					num_replacement++;
				}

			}
			//i++;
		}
	}
	
	vector<size_t> found_restore;
	while (line.size()) {
		size_t found = line.find(separator);
		if (found == string::npos) {
			string lastpart = trim(line);
			if (lastpart.size() > 0) {
				/*start = 0;
				int w = 0;
				while (w < (num_replacement)) {
					found_restore.push_back(line.find(replace_for, start));
					start = found_restore[w] + 1;
					w++;
				}
				for (int k = 0; k < found_restore.size(); k++)
				{
					lastpart[found_restore[k]] = look_for;
				}*/
				result.push_back(lastpart);
			}
			break;
		}
		string segment = trim(line.substr(0, found));
		//cout << "line: " << line << "found: " << found << endl;
		line = line.substr(found + 1);

		//cout << "[" << segment << "]"<< endl;
		if (segment.size() != 0) {
			result.push_back(segment);
		}

		//cout << line << endl;
	}
	for (int k = 0; k < result.size(); k++)
	{
		start = 0;
		int w = 0;

		size_t found = result[k].find(replace_for, start);
		if (found != string::npos)
		{
			while (w < (num_replacement)) {
				found_restore.push_back(result[k].find(replace_for, start));
				start = found_restore[w] + 1;
				w++;
			}
			for (int e = 0; e < found_restore.size(); e++)
			{
				result[k][found_restore[e]] = look_for;
			}
		}
	}
	return result;
}

char** vec_to_char_array (vector<string> parts){
    char ** result = new char * [parts.size() + 1]; // add 1 for the NULL at the end
    for (int i=0; i<parts.size(); i++){
        // allocate a big enough string
        result [i] = new char [parts [i].size() + 1]; // add 1 for the NULL byte
        strcpy (result [i], parts[i].c_str());
    }
    result [parts.size()] = NULL;
    return result;
}

void execute (string command){
    vector<string> argstrings = split (command, " "); // split the command into space-separated parts
	for (int i = 1; i < argstrings.size(); i++)
	{
		//cout << "command in exec[" << i << "]"<< argstrings[i] << endl;
		argstrings[i].erase(remove(argstrings[i].begin(), argstrings[i].end(), '\''), argstrings[i].end());
		argstrings[i].erase(remove(argstrings[i].begin(), argstrings[i].end(), '\"'), argstrings[i].end());
	}
	
    char** args = vec_to_char_array (argstrings);// convert vec<string> into an array of char*
	//cout << "command in exec: " << (string)args << endl;

		execvp(args[0], args);

}

int main (){
	int num_bckgrnd = 0;
	vector<int> bg_vect;
	string previous_directory;
    while (true){ // repeat this loop until the user presses Ctrl + C
		//waitpid
		if (num_bckgrnd) {
			for (int i = 0; i < bg_vect.size(); i++)
			{
				int child = waitpid(bg_vect[i], 0, WNOHANG);
				if (child > 0) {
					bg_vect.erase(remove(bg_vect.begin(), bg_vect.end(), child), bg_vect.end());
				}
			}
			if (bg_vect.size() == 0) {
				num_bckgrnd = 0;
			}
		}
		char cwd[256];
		string current_directory = getcwd(cwd, sizeof(cwd));
		string current_user = getenv("USER");

		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		char s[64];
		assert(strftime(s, sizeof(s), "%c", tm));

		const char* current_user_;
		const char* current_directory_;
		current_user_ = current_user.c_str();
		current_directory_ = current_directory.c_str();
        string commandline = "";/*get from STDIN, e.g., "ls  -la |   grep Jul  | grep . | grep .cpp" */
		printf("%s:%s %s>> ", current_user_, current_directory_, s);
		//cout << current_user<<":"<< current_directory << asctime(timeinfo) << " >> ";
		getline(cin, commandline);
		num_bckgrnd = count(commandline.begin(), commandline.end(), '&');
        vector<string> tparts = split (commandline, "|");
		//vector<int> bg_vect;
		//save input/output
		int tempin = dup(0);
		int tempout = dup(1);
		//int num_bckgrnd = 0;
        // for each pipe, do the following:
        for (int i=0; i<tparts.size(); i++){
            // make pipe
			int fd[2];
			pipe(fd);
			//a.push_back(fork());
			int pid = fork();
			if (num_bckgrnd) {
				bg_vect.push_back(pid);
				tparts[i].erase(remove(tparts[i].begin(), tparts[i].end(), '&'), tparts[i].end());
			}

			/*cout << "num_bckgnd: " << num_bckgrnd << endl;
			cout << "child pid: " << pid << endl;
			cout << "vector size: " << bg_vect.size() << endl;*/

			vector<string> parts = split(tparts[i], " ");
			const char* dir;
			if (parts[0] == "cd") {
				if (parts[1] == "-")
				{
					dir = previous_directory.c_str();
					chdir(dir);
				}
				else
				{
					string current_directory = getcwd(cwd, sizeof(cwd));
					previous_directory = current_directory;
					dir = parts[1].c_str();
					chdir(dir);
					char cwd[256];
					current_directory = getcwd(cwd, sizeof(cwd));
					/*directories[1] = current_directory;
					cout << "from: " << directories[0] << endl;
					cout << "to: " << directories[1] << endl;*/
				}

			}
			else if (parts[0] == "jobs")
			{
				int num_jobs = bg_vect.size();
				//cout << "There are " << num_jobs << "jobs" <<endl;
				cout << "PID" << endl;
				for (int i = 0; i < num_jobs; i++)
				{
					cout << bg_vect[i] << endl;
				}
			}

			if (!pid){
                // redirect output to the next level
                // unless this is the last level
				int num_input = count(tparts[i].begin(), tparts[i].end(), '<');
				int num_output = count(tparts[i].begin(), tparts[i].end(), '>');
				
				/*cout << "num_bckgnd: " << num_bckgrnd << endl;
				cout << "child pid: " << pid << endl;
				cout << "tparts before removing &: " << tparts[i] << endl;
				
				cout << "vector size: " << bg_vect.size() << endl;
				cout << "tparts after removing &: " << tparts[i] <<endl;*/
				const char* out;
				const char* in;
				size_t found_input = tparts[i].find("<");
				size_t found_output = tparts[i].find(">");
				size_t doub_q = tparts[i].find("\"");
				size_t doub_q_last = tparts[i].rfind("\"");
				size_t sing_q = tparts[i].find("\'");
				size_t sing_q_last = tparts[i].rfind("\'");

				string input_file_name;
				string check = tparts[i];
				/*if ((found_input < doub_q_last | found_input > doub_q) | (found_input < sing_q_last | found_input > sing_q)) {
					num_input = 3;
				}
				if ((found_output < doub_q_last | found_output > doub_q) | (found_output < sing_q_last | found_output > sing_q)) {
					num_output = 3;
				}*/
				if (num_input == 1)
				{
					//size_t found_input = tparts[i].find("<");
					if (found_input != string::npos)
					{
						if (num_input+num_output == 2)
						{
							size_t start = found_input + 2;
							size_t end = found_output - start - 1;
							input_file_name = tparts[i].substr(found_input + 2, end) + "\0";
						}
						else
						{
							input_file_name = tparts[i].substr(found_input + 2) + "\0";
						}
						cout << "input file name : " << input_file_name << endl;
						in = input_file_name.c_str();
						tparts[i] = tparts[i].substr(0,found_input);
					}
					//char * cstr = new char[str.length() + 1];
					//vector<string> tparts2 = split(argstrings[i], "<");
					//cout << "Input file name is: " << tparts2[1] << endl;
					int fd = open(in, O_RDONLY); //open file
					dup2(fd, 0);   // make stdin go to file
				}
				if (num_output == 1)
				{
					//size_t found_output = tparts[i].find(">");
					if (found_output != string::npos) //if current condition and num_input !=1
					{
						string out_file_name = check.substr(found_output + 2)+"\0";
						cout << "out file name : " << out_file_name << endl;
						out = out_file_name.c_str();
						if (num_input + num_output == 1)
						{
							tparts[i] = tparts[i].substr(0, found_output);
						}
					}
					//vector<string> tparts2 = split(argstrings[i], ">");
					//cout << "Output file name is: " << tparts2[1] << endl;
					
					int fd = open(out, O_WRONLY | O_TRUNC | O_CREAT); //open file
					dup2(fd, 1);   // make stdout go to file
					close(fd);

				}


                if (i < tparts.size() - 1){
					close(fd[0]);
                    // redirect STDOUT to fd[1], so that it can write to the other side
					dup2(fd[1], 1);
                    close (fd[1]);   // STDOUT already points fd[1], which can be closed
                }
                //execute function that can split the command by spaces to 
                // find out all the arguments, see the definition
				//string temp = tparts[i];


					//cout << tparts[i] << endl;
					execute(tparts[i]); // this is where you execute


            }else{
				//num_bckgrnd = count(commandline.begin(), commandline.end(), '&');
				/*if (num_bckgrnd) {
					bg_vect.push_back(pid);
					//tparts[i].erase(remove(commandline.begin(), commandline.end(), '&'), commandline.end());
				}*/
				//cout << (num_bckgrnd);
				if (num_bckgrnd) {
					/*for (int i = 0; i < bg_vect.size(); i++)
					{
						int child = waitpid(bg_vect[i], 0, WNOHANG);
						//bg_vect.push_back(child);
						if (child > 0) {
							bg_vect.erase(remove(bg_vect.begin(), bg_vect.end(), child), bg_vect.end());
						}
					}*/
				}
				else
				{
					wait(0);            // wait for the child process
					//waitpid(pid, 0, 0);
				}

				// then do other redirects
				dup2(fd[0], 0);
				close(fd[1]);
				close(fd[0]);
				//dup2(tempin, 0); //redirect stdin back to original
				//close(tempin);
            }
			//dup2(fd[0], 0);
			/*dup2(tempin, 0);
			dup2(tempout, 1);
			close(tempin);
			close(tempout);*/
        }
		dup2(tempin, 0);
		dup2(tempout, 1);
    }
}