# add gitConfigMultiBranchTools.txt contents to your .gitconfig file

# at git gui:

1st - at tools menu select the 1st option to create the .sbkp file (it is a simple copy, you should now backup everything, I use Unison)

1.1 - revert all the changed files so you can prepare a "relative patch" to the current mixed branch, based on the new copy, the .sbkp one

2 - create the "relative patches" using git gui tools menu 2nd option

2.1 - change to the branch you want to apply the patch on it (you have to chage to other branches and do the same later)

3 - apply the "relative patch" and resolve minor conflicts using git gui tools menu sbkp option 3.

# Obs: there are some command line tools .sh scripts to let it work on many files at the same time, as thru git gui we can only work in one file per time.

# after using it you can prepare the work for many branches in a single merged branch, and split all the new "relative code" to other branches.

# Obs.: it is all for **linux** (and probably Mac OSX too I guess), and the shell script compatibility is `bash`. But I think they are not complicated to be ported to windows as probably only a few commands will change if at all.

#######################################

# The scripts .sh are to process multiple files at once.
# Link them to some executable path like $HOME/bin or add current folder to $PATH.

# use like this:

gitFindUncomittedFiles.sh # to confirm it is all ok
gitFindUncomittedFiles.sh --sbkp # to create the backukps

# now do your safety backup (for ex sync with Unison)
# and at git gui select all (yes all will work) and revert changes (will only for the changed ones)
gitCreatePatchesForSBKPs.sh

# change the branch and
gitApplyPatches.sh # ignore the ones unrelated to current branch

gitTrashTmpFiles.sh # after you applied at all branches run this
