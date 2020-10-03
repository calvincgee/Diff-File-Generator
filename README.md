# Diff File Generator

A diff file is a file that holds information about what has changed between two files. This is useful because instead of sending the original and new files, both of which can be very large, instead send just the original file and the diff file, which is normally very small. Then the receiver can construct the after file from the diff file and time is saved sending a large file. For example, if the original file is 76 kb and the new kb is 78 kb, this program could create a diff file of 1 kb. This way you can send 2 files of a total of 77 kb instead of two files of 154 kb.

## Features

The function createDiff() takes in the original file and the new file and uses hash tables to store large string sequences and recursion to find the largest sequences in the file. It then constructs a diff file using references to the sequences position in the original file.

The function applyDiff() takes in the original file and the diff file and reconstructs the new file based on interpreting the diff field.

## Usage

- To run the program, first clone the git repo.
- The createDiff() program draws from the "Input Text Files" folder so place your original and new files into this folder or choose from some examples provided: an excerpt from War and Peace or some source code from a regex git project made by Fabrice Bellard.
- Next, navigate to diff.cpp in the src folder. Change the file names to reflect the two files you would like to create a diff of.

`string before_file = "fileNameHere1.txt";`

`string after_file = "fileNameHere2.txt";`
  
- Next build the file using a cl.exe command. Finally, run the diff.exe file created. In the "Ouput Text Files" folder is diff.txt which will contain the new diff file, and newText.txt which contains the recreated new file which should be identical to the second file input.

Enjoy!
  
## Author

Calvin Gee

calving314@gmail.com
