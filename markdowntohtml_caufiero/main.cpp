#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>

//Not using namespace std, I like being specific and I think it helps with readability


void key_position(const std::string key, std::string &words, std::vector<int> &list, int increment)
{
	//This function goes through the string sent in (words) and uses the (key) to split the string into substrings
	//substring and find get the exact position of the (key), and that exact position is added to the (list) of all positions
	
	std::string new_words = words.substr(words.find(key) + key.length());
	increment += words.find(key) + key.length();
	list.push_back(increment);
	
	//Then once we get the first position, we plug the substring into this function again until there are no more (keys)
	if(new_words.find(key) != std::string::npos)
	{
		key_position(key, new_words, list, increment);
	}
	
}

void split_into_lines(std::vector<std::string> &input, std::string words, std::string key)
{
	//Works very similarly to the above function, but instead of getting positions, it gets whole strings
	//The big string, (words), is seperated into substrings indicated by the (key), and then everything before and after
	//is deleted before being added into the (input) vector
	
	std::string temp = words;
	
	size_t pos = 0;
	while((pos = temp.find(key)) != std::string::npos)
	{
		std::string split = temp.substr(0, pos);
		input.push_back(split);
		temp.erase(0, pos + key.length());
	}
	
	input.push_back(temp);
	
}

void replacement_algorithm(std::string first_val, std::string second_val, std::string &output, bool use_endtags)
{
	std::string temp_string = output;
	int tags_added = 0;

	if(output.find(first_val) != std::string::npos)
	{
		std::vector<int> position_list;
		key_position(first_val, temp_string, position_list, 0);
		
		for(size_t i = 0; i < position_list.size(); i++)
		{
			//The & symbol was very confusing, I bet there is a better way but I couldnt think of one without more recursion
			//This section just makes sure there is a semicolon very close in front of the &, so it's a primitive
			//If so, ignore it, if not, move on and replace
			if(first_val == "&")
			{
				//the lower displacement does not account for ignored values, so I made a new displacement for this section
				int amp_displacement = 6 + (tags_added * second_val.length());
				if(output.find_first_of(";", position_list[i]) < position_list[i] + amp_displacement)
				{
					//The & has been ignored, go to the next &
					continue;
				}
				
			}
			
			std::string tag = second_val;
			int end_tag = 0;
			
			if(tags_added%2 == 1 && use_endtags)
			{
				tag.insert(1, "/");
				end_tag++;
			}
			
			//The amount of chars that will be placed with this tag
			int chars_placed = second_val.length();
			//The amount of chars removed after the tag replaces them
			int chars_removed = first_val.length();
			
			//The total amount of chars once all is added and removed this cycle
			int current_move = chars_placed - chars_removed;
			//Accounts for new chars placed if that tag has a backslash
			int current_endtag = chars_placed + end_tag;
			//Accounts for the total amount of end tags in the displacement
			int total_endtags = 0;
			if(use_endtags)
			{
				total_endtags = tags_added * .5;
			}
			
			//This makes sure tags fall in the correct place
			//keeps the grand total of tags added, characters removed, and endtags appended
			int displacement = (tags_added * (current_move) - chars_removed + (total_endtags));
			output.insert(position_list[i] + displacement, tag);
			output.erase((position_list[i] + displacement) + (current_endtag), chars_removed);
			
			tags_added++;
			
		}
	}
}

void block_algorithm(std::string &line, std::string &full_output)
{
		//This function takes in blocks of code and determines whether it is <h>, <ul>, <ol>, or <p>
		std::string start_key;
		std::string end_key;
		std::string section = line;
		
		//It's a header
		if(section[0] == '#')
		{
		
			//This if is used for when a header is IMMEDIATELY followed by like a paragraph or list, without two enter presses
			if(section.find('\n') != std::string::npos)
			{
				std::size_t new_position = section.find('\n');
				
				//This is everything under the header
				std::string past_header = section.substr(new_position);
				past_header.erase(0, 1);
				
				//And this is the header itself
				std::string header = section.substr(0, section.length() - past_header.length());
				header.erase(header.find('\n'), 1);
				//Using erase to trim some of the fat off of the above strings
				
				//Do what we've been doing with the whole message to just this small string, then add the two sections
				std::string new_section;
				block_algorithm(header, new_section);
				block_algorithm(past_header, new_section);
				
				//Once both sections have been individually parsed, append them to the whole output
				full_output.append(new_section);
				
			}
			//If there is nothing immediately after the header within one enter press, do this instead
			else
			{		
				int header_length = section.find_first_not_of('#', 0);
					
				start_key = "<h" + std::to_string(header_length) + ">";
				end_key = "</h" + std::to_string(header_length) + ">\n\n";
						
				section.erase(0, header_length);
				//This just accounts for if the header also has markdown on the back as well
				if(section.find("#") != std::string::npos)
				{
					section.erase(section.length() - header_length, header_length);
				}
					
				section.insert(0, start_key);
				section.insert(0 + section.length(), end_key);
				full_output.append(section);
			
			}
			
		}
		//It's an un-ordered list
		else if((section[0] == '-' || section[0] == '+' || section[0] == '*') && section[1] == ' ')
		{
			start_key = "<u1>\n";
			end_key = "</u1>\n\n";
			
			std::vector<std::string> list_elements;
			split_into_lines(list_elements, section, "\n");
			
			std::string list_output;
			
			//Loop to split list into individual elements, adding begin/end tags to each element
			for(size_t j = 0; j < list_elements.size(); j++)
			{
				std::string list_start_key = "<li>";
				std::string list_end_key = "</li>\n";
				std::string list_section = list_elements[j];
				
				int num_spaces = list_section.find_first_not_of(" ", 1);
				
				//This is if someone presses enter without creating a new list marker
				//It becomes a new list element, I know it should be included in the last but im running out of time
				//If they are splitting their element with enter then they get what they deserve
				if((list_section[0] == '-' || list_section[0] == '+' || list_section[0] == '*') && section[1] == ' ')
					list_section.erase(0, num_spaces);
				
				list_section.insert(0, list_start_key);
				list_section.insert(0 + list_section.length(), list_end_key);
				
				list_output.append(list_section);
			}
			
			section = list_output;
		
			section.insert(0, start_key);
			section.insert(0 + section.length(), end_key);
			
			full_output.append(section);
		}
		//It's an ordered list
		else if(isdigit(section[0])  && section[section.find_first_not_of("0123456789")] == '.')
		{
			
			start_key = "<o1>\n";
			end_key = "</o1>\n\n";
			
			std::vector<std::string> list_elements;
			split_into_lines(list_elements, section, "\n");
			
			std::string list_output;
			
			//Loop to split list into individual elements, adding begin/end tags to each element
			for(size_t j = 0; j < list_elements.size(); j++)
			{
				std::string list_start_key = "<li>";
				std::string list_end_key = "</li>\n";
				std::string list_section = list_elements[j];
				
				int num_spaces = list_section.find_first_not_of("1234567890", 1) + 2;
				
				//This is if someone presses enter without creating a new list marker
				//It becomes a new list element
				if(isdigit(list_section[0])  && list_section[list_section.find_first_not_of("0123456789")] == '.')
					list_section.erase(0, num_spaces);
				
				list_section.insert(0, list_start_key);
				list_section.insert(0 + list_section.length(), list_end_key);
				
				list_output.append(list_section);
			}
			
			section = list_output;
			
			section.insert(0, start_key);
			section.insert(0 + section.length(), end_key);
			full_output.append(section);
		}
		//It's a paragraph
		else
		{
			start_key = "<p>";
			end_key = "</p>\n\n";
			
			section.insert(0, start_key);
			section.insert(0 + section.length(), end_key);
			full_output.append(section);
		
		}
}

void markdown_to_html(std::string &input_markdown)
{
	//This function is split into THREE INTEGRAL SECTIONS
	//SECTION 1: SPECIAL-CHARS SECTION, Insertion and replacement of special characters into primitives
	//SECTION 2: BLOCK SECTION, determing what each seperate block of text is
	//SECTION 3: IN-LINE SECTION, going into the full text again and adding text-editing tags
	
	
	//SECTION 1: SPECIAL-CHARS SECTION, STARTS HERE
	std::map<std::string, std::string> special_chars_dictionary;
	
	special_chars_dictionary["&"] = std::string("&amp;");
	special_chars_dictionary["<"] = std::string("&lt;");
	special_chars_dictionary[">"] = std::string("&gt;");
	
	std::map<std::string, std::string>::iterator it;
	for(it = special_chars_dictionary.begin(); it != special_chars_dictionary.end(); ++it)
	{
		
		replacement_algorithm(it->first, it->second, input_markdown, false);
		
	}
	//SECTION 1: SPECIAL-CHARS SECTION, ENDS HERE
	
	
	
	//SECTION 2: BLOCK SECTION, STARTS HERE	
	std::vector<std::string> seperate_lines;
	std::string full_output;
	split_into_lines(seperate_lines, input_markdown, "\n\n");
	
	//Figure out if lines are <p>, <h>, <ul>, or <ol>
	for(size_t i = 0; i < seperate_lines.size(); i++)
	{
		
		block_algorithm(seperate_lines[i], full_output);
		
	}
	
	input_markdown = full_output;
	//SECTION 2: BLOCK SECTION, ENDS HERE
	
	
	//SECTION 3: IN-LINE SECTION, STARTS HERE
	std::map<std::string, std::string> markdown_dictionary;
	
	markdown_dictionary["*"] = std::string("<em>");
	markdown_dictionary["_"] = std::string("<em>");
	markdown_dictionary["**"] = std::string("<strong>");
	markdown_dictionary["__"] = std::string("<strong>");
	
	std::map<std::string, std::string>::reverse_iterator reit;
	for(reit = markdown_dictionary.rbegin(); reit != markdown_dictionary.rend(); ++reit)
	{
		
		replacement_algorithm(reit->first, reit->second, input_markdown, true);
		
	}
	
	//SECTION 3: IN-LINE SECTION, ENDS HERE
	
	
	//And now we print what we've finished to the console and return to main
	std::cout << input_markdown << std::endl;
	
}
	
	
	
	
int main() {
	

	//Take in the input.txt text
	std::string input;
	
	std::string line;
	std::ifstream input_file;
	input_file.open("input.txt", std::ios::in);
	
	if(input_file.is_open())
	{
		while(getline (input_file, line))
		{
			input.append(line);
			input.append("\n");
		}
		input_file.close();
		
		//Input file has been taken in

		//Now we turn that input from markdown to html
		markdown_to_html(input);
	
		//Output string to output.txt
		std::ofstream output_file;
		output_file.open("output.txt", std::ios::out);
		
		if(output_file.is_open())
		{
			output_file << input << std::endl;
		}
		else std::cout << "Unable to open output file" << std::endl;
		//String has been output to the file
	
	}
	else std::cout << "Unable to open input file" << std::endl;
	
	
	return 0;
}



