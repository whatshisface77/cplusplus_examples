# cplusplus_examples
 These are example programs documenting major projects I have completed with C++.

--- Example 1: Boggle Solver ---
This program consists of two classes and the main section, I wrote all of the code on the same file for easy submission and hopefully easier review by your team however splitting it up into multiple files would be a non-issue for me.

In order to analyze a board, simply scroll down to main and type in your board exactly as you asked in the question, as one long string, on line 381 where it calls the (solve_board) function on the Boggle object. You can also change the width and length of the board in this function as well, and as long as your string matches the appropriate length of your board's dimensions, you'll get correct results! (You'll still get correct results if you under/over shoot the string but the function will still run some processes a couple more times on your string just in case)

You can also adjust which legal words to look for on line 364 with the (legal) vector. I added a list of words that I think perfectly display the object's capabilities, however feel free to add as you like, if it is included in this list, then it will be searched for in the board!


-- Example 2: Markdown to HTML ---
This program is simpler to use. All you have to do is type down Markdown text in your (input.txt) file, and it comes out as html tags in your (output.txt) file.
If for any reason these files aren't present or can't be opened, the program will close harmlessly.
