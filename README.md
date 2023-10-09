# TermTree
Tree file and folder view for windows and linux.

Made in c I suppose. 

## Installation /  How to build 

### If you are using `arch linux`
- Available on `AUR`
- run with `ttree`
```
yay -S termtree
```

### Build using `make`
* build files will be located in `./build` directory
    - make sure to have internet connection when using make, otherwise the build will fail (it has to get it's dependencies from github)
```
make
```
### Installation (linux only)
```
make install
```
- After installation the program should be callable by `ttree`
- To see manual use `man ttree`
### Uninstallation (linux only)
```
make uninstall
```


## Features
* ### file / folder sizes

![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree01.PNG)


* ### infile search

```
ttree --find=<text>/<specific_file_type / empty_for_all>
```
<!-- ![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree04.PNG) -->


* ### search for files / folders

```
ttree --search=<file/folder name>
```
<!-- ![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree02.PNG) -->


* ### specify depth of subfolders

```
ttree --max-index=<number>
```
<!-- ![Alt text](https://github.com/bendikMichal/termtree/blob/images/ttree03.PNG) -->
