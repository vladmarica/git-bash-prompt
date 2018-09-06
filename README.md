## Git Bash Prompt
 A fast and simple Git prompt for Bash, inspired by [posh-git](https://github.com/dahlbyk/posh-git).

Here's an example of what it could look like:

![](https://i.imgur.com/IdDueCK.png)

I make no guarentees as to how well it performs or if it will accurately display the current Git status. It has no indication of detached HEADs, merge conflicts, or any of that fancy stuff.

Tested on Cygwin (MinTTY) on Windows 7 and on Debian 9.

### Requirements
- GCC 6 or highter _(this can be changed to Clang if you modify the Makefile)_
- GNU Make
- A terminal emulator that supports Unicode

### Building
- Clone the repo `git clone https://github.com/Quintinity/git-bash-prompt.git`
- `cd` into the repo folder and run `make`

### Installation

After building the `git-bash-prompt` executable, copy it to anywhere on your `$PATH`.

Then, just include `\$(git-bash-prompt)` to your PS1 wherever you want it. Please not that the Git status part includes a trailing space.

The prompt in the image at the top of the README is produced by the following PS1, for example.
```Bash
export PS1="\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\] \$(git-bash-prompt)\$ "
```
