FindDup Suite

FindDup Suite contains 3 parts:
- finddup.pl
- dupselect.php
- unlink.pl

finddup.pl is the duplicate file finder which will search duplicate files in 
current directory or specified directory. Progress will print to STDERR and 
duplicate file list will print to STDOUT.

Usage:
 $ perl finddup.pl [Path] > [duplicate-files.txt]


dupselect.php is a PHP web application for helping user to select duplicate 
files. Copy/move [duplicate-files.txt] to same directory as dupselect.php in 
the web server. For example, creating a web server with PHP in localhost, then 
copy dupselect.php and duplicate-files.txt to /var/www/html, and then fire up 
a web browser(such as Firefox) and go to http://localhost/dupselect.php and 
you will have a screen like this:

 duplicate-files.txt DupSelect CalcTotal

The DupSelect link will go to selecting duplicate files using 
duplicate-files.txt duplicate file list.
The CalcTotal link show you a list and calculate Total size of all files in the 
file.

In DupSelect mode, you will see a group seperated by horizontal line like this:

   [generate]
     _________________________________________________________________
   
   [ ] ./20080428/kaberen22204.jpg (f66f689f)
   [X] ./20080427/kaberen22204.jpg (f66f689f)
     _________________________________________________________________

   [ ] ./20080105/T2AW_WP_3/T2AW_10/T2AW_10z.psd (274a3199) (*)
   [X] ./20080910/Al_4575/T2AW_10/T2AW_10z.psd (274a3199) (**)
   [X] ./20071124/T2AW_WP_2/T2AW_10.psd (274a3199)
     _________________________________________________________________

   [ ] ./20080506/moeura26162.png (37645a01)
   [X] ./20081228/moeura46760.png (37645a01)
     _________________________________________________________________

The first file in each group is kept by default. dupselect.php will think 
the file in deeper directory is more important.
There is two marks, (*) and (**). (*) repersents there is a file in deeper 
directory in this group while (**) means there is more than one file in 
deeper directory.
The selected files will generate a duplicate-files-delete.txt and 
duplicate-files-delete.lst file in server after pressing [generate] button.
duplicate-files-delete.txt is for you to view a report using CalcTotal mode.
duplicate-files-delete.lst is for delecting using unlink.pl.

Inside dupselect.php:
There is some variables in dupselect.php for the sorting and selecting 
strategy.

 $excludes_order = array('detail');
 $includes_order = array('waren','kaberen','moeren','kabeura','moeura');
 $deselects = array('this-one-needs-duplicate');
 $normal_depth = 2;

The $excludes_order variable controls which file should place in the back.
The $includes_order variable controls which file should place in the fronter, 
but the file in deeper directory still have higher priority.
The $deselects variable controls which file should not be selected in 
DupSelect mode.
The $normal_depth variable controls which file become more important.


unlink.pl is an utility for deleting files using a .lst list file.
Usage:
 $ perl unlink.pl [duplicate-files-delete.lst]
