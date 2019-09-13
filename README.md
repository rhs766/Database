# Database
Multithreaded Database Management System  

Simulation of requests from multiple users to access a database.  

Main thread creates the database and request threads according to the input.  
Request threads (represented by each line of the input file) simulate the access  
	of a data record in the database.  
Main thread then prints a summary of the requests.  

Program reads an input file from stdin using redirection.  

Example input file (without comments, group can be 1 or 2, position can be 1-10):  
1 // Starting group (1 or 2)  
1 3 0 5 // User 1 from Group 1 requesting position 3 at time 0 for 5 sec.  
2 3 2 5 // User 2 from Group 2 requesting position 3 at time 2 for 5 sec.  
1 3 1 5 // User 3 from Group 1 requesting position 3 at time 3 for 5 sec.  
2 1 3 1 // User 4 from Group 2 requesting position 1 at time 6 for 1 sec.  

Example output:  
User 1 from Group 1 arrives to the DBMS  
User 1 is accessing the position 3 of the database for 5 second(s)  
User 2 from Group 2 arrives to the DBMS  
User 2 is waiting due to its group  
User 3 from Group 1 arrives to the DBMS  
User 3 is waiting: position 3 of the database is being used by user 1  
User 1 finished its execution  
User 3 is accessing the position 3 of the database for 5 second(s)  
User 4 from Group 2 arrives to the DBMS  
User 4 is waiting due to its group  
User 3 finished its execution  

All users from Group 1 finished their execution  
The users from Group 2 start their execution  

User 2 is accessing the position 3 of the database for 5 second(s)  
User 4 is accessing the position 1 of the database for 1 second(s)  
User 4 finished its execution  
User 2 finished its execution  

Total Requests:  
	Group 1: 2  
	Group 2: 2  
Requests that waited:  
	Due to its group: 2  
	Due to a locked position: 1  
  
Compile: gcc -std=c11 -pthread database.c -o database  
Run: ./database < input.txt  
