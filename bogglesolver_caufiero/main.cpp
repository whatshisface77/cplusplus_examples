#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <algorithm>

//No std namespace for me, I like the specificity and I think it improves readability


//Trie used to hold all legal words
class boggle_trie{
	public:
		//Every child that spawns off of a node letter is included in this vector for the node
		std::vector<boggle_trie*> children;
		//Used to determine branch length and if there is a leaf present
		int prefix_count, leaf_count;
		
		//Constructor to set counts and prepare base node for all 26 letters
		boggle_trie()
		{
			children.resize(26, NULL);
			prefix_count = 0;
			leaf_count = 0;
		}
		
		//The recursive insert, takes in words one letter at a time
		void insert_util(boggle_trie* node, std::string &word, int i);
		//The insert function, takes in words and sends them to (insert_util) for recursive seperation
		void insert(std::string word);
		//The recursive search, takes in words one letter at a time, travelling down existing node branches to find full words
		bool search_util(boggle_trie* node, std::string &word, int i);
		//The search function, takes in words and sends them to (search_util) to search by letter recursively
		bool search(std::string word);

		
};

void boggle_trie::insert_util(boggle_trie* node, std::string &word, int i)
{

	if(i == word.size())
	{
		node->prefix_count++;
		node->leaf_count++;
		return;
	}
	
	//As long as there is more than one letter left
	//Keep adding to the children vector of that node a new trie for the next letter
	node->prefix_count++;
	
	if(!node->children[word[i] - 'a'])
		node->children[word[i] - 'a'] = new boggle_trie();
		
	insert_util(node->children[word[i] - 'a'], word, i + 1);
	
}

void boggle_trie::insert(std::string word)
{
	
	if(!word.size())
		return;
		
	//Begin recursive insertion if there is an actual word
	insert_util(this, word, 0);
	
}

bool boggle_trie::search_util(boggle_trie* node, std::string &word, int i)
{
	if(!node)
		return false;
	if(i == word.size())
		return node->leaf_count > 0;
		
	//As long as there is more than one letter left
	//Keep diving through node vectors until there is one letter left to find full word	
	return search_util(node->children[word[i] - 'a'], word, i + 1);
}

bool boggle_trie::search(std::string word)
{
	if(!word.size())
		return true;
	
	//Begin recursive search if there is an actual word
	return search_util(this, word, 0);
}



class c_boggle
{
	//Words that are legal
	std::vector<std::string> legal_dictionary;
	//Words that have been found
	std::vector<std::string> legal_found;
	//The size of the board is known throughout the object
	int width, height;
    public:
    //prior to solving a board, configure legal words
    void set_legal_words (
        //the legal words, alphabetically sorted
        const std::vector<std::string> all_words);

    //Collects legal words and board pieces before executing find_words to locate every legal word
    void solve_board(
        //board width
        int board_width,
        //board height
        int board_height,
        //board width*height characters in row major order
        const char *board_letters
    );
    
    //Searching through the trie to find the appropriate legal words on the board
	void find_words(
		//The current node (character) of the Trie
		boggle_trie* TrieChild,
		//Full boggle board
		char **b,
		//The row of the current character
		int r, 
		//The column of the current character
		int c, 
		//The board containing visited/non-visited spaces
		bool **bv,
		//The word being formed char-by-char
		std::string words 
	 );
	 
	 //Print all of the found words
	 void print_results();
	 
	 //Determines if a possible space can be on the board
	 bool in_bounds(
	 	//Possible space's row
	 	int r,
	 	//Possible space's column
	 	int c
	 );
    
    //Used for sorting legal words in alphabetical order before game begins
    std::vector<std::string> alphabetical(
		std::vector<std::string> word_list
	);
    
};

//possibly add reference to all_words later for Oh optimization
void c_boggle::set_legal_words (std::vector<std::string> all_words)
{
	std::cout << "Setting and sorting legal words..." << std::endl;
	
    std::vector<std::string> sorted_words = alphabetical(all_words);
    for(int i = 0; i < sorted_words.size(); i++)
    {
        legal_dictionary.push_back(sorted_words[i]);
    }
    
    std::cout << "Alphabetical Sort Complete!" << std::endl;
    
}

void c_boggle::solve_board(int board_width, int board_height, const char *board_letters)
{
	
	std::cout << "Establishing Boggle Board..." << std::endl;
	
	width = board_width;
	height = board_height;
	
	char board[board_width][board_height];
	int board_order = 0;
	for(int i = 0; i < board_width; i++)
	{
		for(int j = 0; j < board_height; j++)
		{
			board[i][j] = board_letters[board_order];
			board_order++;
		}
	}
	
	//Variable transfer necessary for the search function, b points to the full board
	char *b[board_width];
	for(int i = 0; i < board_width; i++)
	{
		b[i] = board[i];
	}
	
	std::cout << "Board established!" << std::endl;
	
	std::cout << "Inserting legal words into algorithm..." << std::endl;
	
	boggle_trie Trie;
	for(int i = 0; i < legal_dictionary.size(); i++)
	{
		Trie.insert(legal_dictionary[i]);
	}
	
	std::cout << "Insertion successful!" << std::endl;
	
	std::cout << "Searching for legal words..." << std::endl;
	
	//Marking the whole board as false, turn to true as we visit tiles
	bool board_visited[board_width][board_height];
	memset(board_visited, false, sizeof(board_visited));
	
	//bv points to the original boards visited
	bool *bv[board_width];
	for(int i = 0; i < board_width; i++)
	{
		bv[i] = board_visited[i];
	}
	
	

	
	//start searching the board, r is rows, c is columns
	for(int r = 0; r < board_width; r++)
	{
		for (int c = 0; c < board_height; c++)
		{
			std::string words = "";
			if(Trie.children[board[r][c] - 'a'])
			{
				words += board[r][c];
				find_words(Trie.children[board[r][c] - 'a'], b, r, c, bv, words);
			}
		}
	}
	
	std::cout << "Search completed!" << std::endl;
	//Once the board is fully searched, reveal found words
	print_results();
	
}

void c_boggle::find_words(boggle_trie* TrieChild, char **b, int r, int c, bool **bv, std::string words)
{
	
	//Word is fully found and meets all Boggle rules
	if(TrieChild->leaf_count > 0 
		&& !std::count(legal_found.begin(), legal_found.end(), words)
		&& words.size() > 2)
		{
			legal_found.push_back(words);
		}
	
	
	//Check if the space is both in-bounds and if it has been visited before 
	//if not, then mark it visited and start looking for the next letter
	if(in_bounds(r, c) && !bv[r][c])
	{
		bv[r][c] = true;
			
		//Will loop through for every child that the Trie root has
		for(int i = 0; i < TrieChild->children.size(); i++)
		{
			if(TrieChild->children[i] != NULL)
			{
				char next_char = (char)i + (char)'a';
				
				//And here, the search of adjacent tiles begins
				//If the tile is in bounds, and is unvisited, AND it contains
				//the next character in the Trie of legal words, recurse
				if(in_bounds(r - 1, c - 1) && !bv[r - 1][c - 1] && b[r - 1][c - 1] == next_char)
				{
					find_words(TrieChild->children[i], b, r - 1, c - 1, bv, words + next_char);
				}
				
				if(in_bounds(r - 1, c) && !bv[r - 1][c] && b[r - 1][c] == next_char)
				{
					
					find_words(TrieChild->children[i], b, r - 1, c, bv, words + next_char);
				}
				
				if(in_bounds(r - 1, c + 1) && !bv[r - 1][c + 1] && b[r - 1][c + 1] == next_char)
				{
					find_words(TrieChild->children[i], b, r - 1, c + 1, bv, words + next_char);
				}
				
				if(in_bounds(r, c - 1) && !bv[r][c - 1] && b[r][c - 1] == next_char)
				{
					find_words(TrieChild->children[i], b, r, c - 1, bv, words + next_char);
				}
				
				if(in_bounds(r, c + 1) && !bv[r][c + 1] && b[r][c + 1] == next_char)
				{
					find_words(TrieChild->children[i], b, r, c + 1, bv, words + next_char);
				}
				
				if(in_bounds(r + 1, c - 1) && !bv[r + 1][c - 1] && b[r + 1][c - 1] == next_char)
				{
					find_words(TrieChild->children[i], b, r + 1, c - 1, bv, words + next_char);
				}
				
				if(in_bounds(r + 1, c) && !bv[r + 1][c] && b[r + 1][c] == next_char)
				{
					find_words(TrieChild->children[i], b, r + 1, c, bv, words + next_char);
				}
				
				if(in_bounds(r + 1, c + 1) && !bv[r + 1][c + 1] && b[r + 1][c + 1] == next_char)
				{
					find_words(TrieChild->children[i], b, r + 1, c + 1, bv, words + next_char);
				}
			}			
		}
	}
	
	//And then once we return, mark this tile unvisited
	//So it can be revisited if other words can be reached from this branch
	bv[r][c] = false;
			
}

void c_boggle::print_results()
{
	std::cout << std::endl;
	std::cout << "FINISHED! The legal words you can form with this Boggle Board are:" << std::endl;
	
	//List everything in the vector containing found words
	for(int i = 0; i < legal_found.size(); i++)
	{
		
		std::cout << i + 1 << ". " << legal_found[i] << std::endl;
	}
	
	std::cout << std::endl;
	
}

//Checks whether possible boggle pieces are in bounds
bool c_boggle::in_bounds(int r, int c)
{
	return (r >= 0 && r < width && c >= 0 && c < height);
}

//Function for comparing two strings to sort in alphabetical order
bool compare_function(std::string a, std::string b)
{
    return a < b;
}

//Take in list of words and sort them in alphabetical order
std::vector<std::string> c_boggle::alphabetical(std::vector<std::string> word_list)
{
    int list_size = word_list.size();
    
    //Runtime of this is O(N*log(N))
    std::sort(word_list.begin(), word_list.end(), compare_function);
    return word_list;
}

int main()
{
	
	c_boggle BoggleBoard;
	bool run_program = true;
	
	std::string legal[] = {"abed", "abo", "aby", "aero", "aery", "bad", "bade", "bead", "bed", "robbed", "be", "beef"};
	int n = sizeof(legal) / sizeof(legal[0]);
    std::vector<std::string> legal_words(legal, legal + n);

    while(run_program)
    {
	    std::cout << "Hello, welcome to the Boggle Simulator" << std::endl
	    	<< "Enter 1 to solve a board" << std::endl
	    	<< "Enter 2 to quit" << std::endl;
	    	
	    int user_input;
	    std::cin >> user_input;
	    
	    switch(user_input)
	    {
	    	case 1:
		    	BoggleBoard.set_legal_words(legal_words);
		    	BoggleBoard.solve_board(3, 3, "yoxrbaved");
		    	break;
		    case 2:
				run_program = false;
		    	break;
		}
	}
    


    return 0;
}



