# TermTree
Tree file and folder view for windows.

Made in c I suppose. 

## Installation /  How to build 


#### WINDOWS
```
gcc -o ttree tree.c libs/stringex.c libs/argLib.c
```
* can add to path and run with "ttree"


#### LINUX
```
gcc -o ttree ltree.c libs/stringex.c libs/argLib.c
```

## Features
* ### file / folder sizes

![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree01.PNG)


* ### infile search
* #### flag:

```
-ftext_to_find/file_type(leave empty for all file types)
```
![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree04.PNG)


* ### search for files / folders

![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree02.PNG)


* ### specify depth of subfolders

![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree03.PNG)


Tested on:

windows 10, 11

archlinux

