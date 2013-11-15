MyDu
=================

This program is a disk analyzer. It traverses a filesystem, reading data about each file. It also keeps statistics on the filesystem ranging from the largest file to the most recently changed file. It is also able to make some simple graphs showing the data visually.

Goal
-----
This program is suppose to traverse a Linux file system, gathering data. Should be able to do much of what 'du' can do.

Steps
* Make a terminal program that can traverse through the Linux filesystem. 
* Try to gather data from each file's inode
* Output statistics

Extra Credit Ideas
* Grab more information
* Make a graph or visual representation of the data
* Make a GUI (GTK+, Qt...) for the program

Things To Improve
* Check for memory leaks
* Make more graphs

Dependencies
-------------
* gcc
* gnuPlot

Usage
-------------
To Compile:
gcc -Wall -g myDu.c

To Run:
./a.out [file path]

This should run the executable a.out, where it will gather data from file path. File path can be any location from a user's home directory to root.
If this command is run properly, it will output data on the terminal. For easier data mining, it will also out put 'directories.dat' and 'display_info.dat' (both text files that can be viewed from your editor of choice). It also generates graph_files.ps and graph_size.ps, bar graphs that show the data visually.

Git Notes
---------
1. Make directory
2. cd into directory
3. git init
4. Make/modify files as needed
5. git add 'filenamehere'
6. git commit -m 'messagehere'
7. git push origin master
