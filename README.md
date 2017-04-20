COEN 283 Project 1
==================

Members
-------
Jiaoni Zhou jzhou@scu.edu
Chenjun Ling cling@scu.edu

Results
-------
1.Internal Commands

about
```sh
mysh$ about
Jiaoni Zhou W1189742
Chenjun Ling W1189446
```
cd
```sh
mysh$ cd
/Users/admin/Documents
mysh$ cd ..
mysh$ pwd
/Users/admin
mysh$ cd errordir/
/Users/admin/errordir/: No such file or directory
```

clr
```sh
mysh$ clr
```
dir
```sh
mysh$ dir
README.md shell.c
```
environ
```sh
mysh$ environ
TERM_PROGRAM=iTerm.app
......
PWD=/Users/admin/Documents/
......
```
echo
```sh
mysh$ echo ls
ls
mysh$ echo "hello"
hello
```
help
```sh
mysh$ help
GNU bash, version 3.2.57(1)-release (x86_64-apple-darwin16)
These shell commands are defined internally.  Type `help' to see this list.
Type `help name' to find out more about the function `name'.
Use `info bash' to find out more about the shell in general.
Use `man -k' or `info' to find out more about commands not in this list.

A star (*) next to a name means that the command is disabled.

 JOB_SPEC [&]                       (( expression ))
 . filename [arguments]             :
 [ arg... ]                         [[ expression ]]
 ......
```

2.System Shell Commands

ls
```sh
mysh
$ ls
README.md	shell.c
```
ls -l
```sh
mysh$ ls -l
total 24
-rw-r--r--  1 admin  staff  1185 Apr 19 20:00 README.md
-rw-r--r--  1 admin  staff  8060 Apr 19 19:52 shell.c
```
ls -l > foo
```sh
mysh$ ls
README.md		shell.c
mysh$ ls -l > foo
mysh$ ls
README.md	foo		shell.c
```
sort < testfile
```sh
mysh$ sort < foo
-rw-r--r--  1 admin  staff     0 Apr 19 20:06 foo
-rw-r--r--  1 admin  staff  1374 Apr 19 20:04 README.md
-rw-r--r--  1 admin  staff  8060 Apr 19 19:52 shell.c
total 24
```
pipe: ls -l | more
```sh
mysh$ ls -l | more
total 32
-rw-r--r--  1 admin  staff  1686 Apr 19 20:10 README.md
-rw-r--r--  1 admin  staff   172 Apr 19 20:06 foo
-rw-r--r--  1 admin  staff  8060 Apr 19 19:52 shell.c
```
