# Project Karman Avionics System
The avionics system for the Northeastern University AIAA Project Karman rocket.

Check out the [wiki](https://github.com/AIAANortheastern/karman-avionics/wiki)!

### Getting Started

Download Cygwin, Git Bash, or some other command line interface (Bash for Windows 10, etc.)

Create a folder on your computer. For example,
~~~
Documents/Karman
~~~

Navigate to this directory in the comand line interface. If you are using Cygwin or Git Bash,
this means typing 
~~~
cd ~/Documents/Karman
~~~
Where ~ means "My home directory" (C:\Users\<username>"), and cd means "change directory".

Enter the command:
~~~~
git clone https://github.com/AIAANortheastern/karman-avionics.git
~~~~

This will "clone" the git repository to your computer. If you are using Git Bash, you will see a nice
blue parenthetical next to your current directory that says "master".

TODO
Create a branch: Tyler pls fill this in kthx.

###Using Atmel Studio

This project uses the free software Atmel Studio, created by the Atmel Corporation for use with AVR microcontrollers. This software is based off of Microsoft Visual Studio. To get it, go to 
[Atmel's website](http://www.atmel.com/tools/atmelstudio.aspx) And click the disk next to the web installer entry at the bottom of the page. This will install atmel studio. Be prepared to wait a while!

Make sure you install AVR for 8/16 bit processors, and the Atmel Software Framework (ASF). We will be using both of these extensions.

Open up the karman-avionics.atsln file in the directory karman-avionics in Atmel studio, and you're good to go!

To add a file:
	Right click on the folder you want the file to exist in. For example, if you're making a driver, in the solution explorer on the right side of Atmel Studio, right click the drivers directory. If you don't see it, be sure to expand the src directory by clicking the plus next to src.
	
Select "Add new item" and then choose either a .c file or a .h file. Don't add any C++ files! C++ is not fully supported by avr-gcc. Make sure that your .c and .h files have the same name, and that the names are descriptive of what the file will be. For example, "TylersGreatNewFile.c" is not a good name. If you are making the driver for the Silocon labs Si7021-A20 Temperature/humidity sensor, "Si7021_a20.c" is a very good file name.
	
### Good practices

If you commit something that doesn't compile, I will find you, and I will make you fix it until it does.

- Rule #1: Don't merge if it doesn't compile.
- Corrolary to Rule #1: Avoid merging if you have compiler warnings.
- Rule #2: Don't merge if it doesn't compile.


Naming conventions:

To keep everything consistent and readable, some naming conventions should be used. We can discuss these if you think they're terrible, but for some basic guidelines:
	
- filenames should start with a capital letter, and try to have everything else be lowercase. The only caracters that should be used are A-Z, 0-9, and underscore. Avod dash or em-dash, or any other special characters.
	
- every .c file MUST have an accompanying header file (.h). In the header you will declare every function that's in your .c file. 
	
Note the difference between declaration and definition. A declaration is of the form
```c
void foo(uint8_t param1, float32_t param2);
```
While a definition is of the form
```c
void foo(uint8_t param1, float32_t param2)
{
	...
	<function body goes here>
}
```
	
- Make sure everything in the body of your header files is within the "include guards". These ensure that the body of the header file is only included once. This makes the compiler much happier.
	
- functions should use the snake_case convention
	
- variables should use camelCase. That way we get to learn both! (don't hurt me)
	
