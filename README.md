# Project Karman Avionics System
The avionics system for the Northeastern University AIAA Project Karman rocket.

Check out the [wiki](https://github.com/AIAANortheastern/karman-avionics/wiki)!

### Getting Started

Download Cygwin, Git Bash, or some other command line interface (Bash for Windows 10, etc.)

Create a folder on your computer. For example,
~~~
mkdir ~/git/Karman
~~~

Navigate to this directory in the comand line interface. If you are using Cygwin or Bash,
this means typing
~~~
cd ~/git/Karman
~~~
Where ~ means "My home directory" (C:\Users\<username>"), and cd means "change directory".


Clone the repository by using the command:
~~~
git clone https://github.com/AIAANortheastern/karman-avionics.git
~~~
This creates a folder called "karman-avionics" in the current directory. You can change the name of this if you would like.

Go into the karman-avionics folder (using cd) and type 
~~~
git branch
~~~
You should see that there is one branch called "master".

Create and switch to a new branch by typing 
~~~
git checkout -b my_fancy_branch_name
~~~
But call it something descriptive for the task you are working on.

Now if you want to commit your changes, you can do the following:
~~~
git add .
git commit -m "add xyz, change foo to do bar"
~~~

Pushing your changes to your brand new branch is as easy as typing
~~~
git push -u origin my_fancy_branch_name
~~~
If you are working on a branch with other people, always do a git pull before a git push. After the first push all you need to do is type "git push" to push to github.

Once you've decided that your code is rock solid, and you've tested it extensively, you can issue a pull request,
where you'll be asking the other developers to reveiw your code, and if it's up to par they'll merge it either into a larger
development branch or master.

###Using Atmel Studio

This project uses the free software Atmel Studio, created by the Atmel Corporation for use with AVR microcontrollers. This software is based off of Microsoft Visual Studio. To get it, go to
[Atmel's website](http://www.atmel.com/tools/atmelstudio.aspx) And click the disk next to the web installer entry at the bottom of the page. This will install atmel studio. Be prepared to wait a while!

Make sure you install AVR for 8/16 bit processors, and the Atmel Software Framework (ASF). We will be using both of these extensions.

Open up the karman-avionics.atsln file in the directory karman-avionics in Atmel studio, and you're good to go!

To add a file:
	Right click on the folder you want the file to exist in. For example, if you're making a driver, in the solution explorer on the right side of Atmel Studio, right click the drivers directory. If you don't see it, be sure to expand the src directory by clicking the plus next to src.

Select "Add new item" and then choose either a .c file or a .h file. Don't add any C++ files! C++ is not fully supported by avr-gcc. Make sure that your .c and .h files have the same name, and that the names are descriptive of what the file will be. For example, "TylersGreatNewFile.c" is not a good name. If you are making the driver for the Silocon labs Si7021-A20 Temperature/humidity sensor, "Si7021_a20.c" is a very good file name.


### Linux Development

Download the AVR toolchain http://www.atmel.com/tools/atmelavrtoolchainforlinux.aspx
and unzip it in your favorite directory.
~~~
tar -xzvf avr8-gnutoolchain-X.X.X.XXXX-linux.any.x86_64.tar.gz
~~~

Add the bin directory to your path. For example:
~~~
export PATH=$PATH:~/avr8-gnu-toolchain-linux_x86_64/bin
~~~
Be sure that the path you chose is the full path.
If you append this to your .bashrc file, you won't have to do it every time you open a new terminal.

Make sure that the branch you are tracking is based off of the branch "linux-dev" as this has the makefile in it.

To build, go to the directory karman-avionics/karman-avionics/Debug, and run make.

If you add any files, or any directories, make sure to modify config.mk.

For new C FILES:   add the filename to the variable CSRCS
For new DIRS:      add the directory name to the variable INC_PATH

### Mac Development

Make sure you have the xcode command utils with the following command in your terminal:
`xcode-select --install`

Install homebrew (a package manager for macOS) with the following command:
`/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`.
If you don't want to pipe the script directly into the ruby interpreter (for security) you can download it
first and inspect it before running.

Install the avr-gcc compiler with:
```
brew tap osx-cross/avr
brew install avr-libc
```

Make sure that the branch you are tracking is based off of the branch "linux-dev" as this has the makefile in it.

To build, go to the directory karman-avionics/karman-avionics/Debug, and run make.

If you add any files, or any directories, make sure to modify config.mk.

For new C FILES:   add the filename to the variable CSRCS
For new DIRS:      add the directory name to the variable INC_PATH

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

