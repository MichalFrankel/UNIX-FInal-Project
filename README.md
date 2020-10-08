# UNIX Final Project

How to run this program?

Through the terminal:
1. Run myFileSystemMonitor -d {dir}  -i {ip}
   e.g: myFileSystemMonitor -d  /etc  -i 127.0.0.1

View information regarding the directory file changes: 

* Throguh the localhost via the browser
* Through the terminal when running the following command: netcat -l -u -p 10000
* Through the terminal connecting via Telnet, using the following command: telnet 127.0.0.1 12345
   - For retreiving information regarding the backtrcke, use the "backtrace" command. Or parse help for getting all the availible commads.
   
* Log file called "events.txt" will be added once the program was ended, and it would contain all of the events occured while the program was running. 
