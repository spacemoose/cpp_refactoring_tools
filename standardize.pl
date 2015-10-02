# Long long ago, in a company far far away, I worked on a codebase
# where developers had scattered "using namespace std;" statements
# through their header files like rice at a wedding.
#
# At some point people realized what a bad idea this was, but by that
# time there hundreds of these statements in thousands of files.  So I
# created a little perl script that does the following:
#
# - if a header file has a "using namespace std" remove it.
# - in the header file, look for places that might need a std::, and insert them.
# - if a cpp file has stuff from the std without a std::, AND it has
#   no using namespace std statement, insert such a statement after the last include.
#
#! /usr/bin/env
#perl
use warnings;
use strict;
use feature 'switch';
use File::ReadBackwards;
use File::Find::Rule;
use Cwd;


my @possible_offenders = (
	"min",
	"max",
	"cout",
	"cerr",
	"string",
	"_Rb_tree",
	"initializer_list",
	"pair",
	"ostream",
	"equal_to",
	"ostringstream",
	"endl",
	"_Identity",
	"less",
	"function",
	"ifstream",
	"make_pair",
	"make_tuple",
	"tuple",
	"vector",
	"binary_function",
	"unary_function",
	"greater",
	"istream",
	"ofstream",
	"_Select1st",
	"_Rb_tree_const_iterator"
	);

# This takes one of the possible offenders, and turns it into a safe
# matching string...  make sure it's not part of larger vname ...
sub make_safe_matching_string
{
	return /(?<!std::)\b$_[0]\b/;
}# does the passed file already have a using namespace std?
sub already_has_using_directive
{
	open(FILE, $_[0]) or die "$!";
	while (<FILE>){
		if (/^using namespace std;/){
			return 1;
		}
	}# iter ver file.
	return 0;
}

# only needed for cpp files.  Checks if the file has an offending call
# to std namespace without preface or
sub needs_using_directive
{
	if (already_has_using_directive($_[0])){
		return 0;
	}
	open(FILE, $_[0]) or die "$!";
	while (<FILE>){
		if (!/^#include/){
			my $curline = $_;
			foreach(@possible_offenders){
				if($curline =~ /[^:]$_/ ){
					return 1;
				}
			}
		}
	}# iterate over file
	return 0;
}

# Return the index of the last include statement.
# arg1:  array (lines of file).
sub find_last_include
{
	for (my $i = scalar @_; --$i>0;){
		my $curline = $_[$i];
		if ($curline =~ /^\s*#include/){
			return $i;
		}
	}
}

# arg1 filename.
sub read_file{
	my $fname = $_[0];
	if($fname  =~ /h$/) {
		die "tried to insert a using directive in a header file!!!";
	}
	open (FILE, $_[0]) or die "failed to open ", $_;
	return  <FILE>;
}

# insert a using directive.  Must only be called on def file.
# arg: filename
sub insert_using_directive{
	my $fname = $_[0];
	print "insert into ", $fname, "\n";
	my @lines = read_file($fname);
	my $last_include_pos = find_last_include(@lines);
	splice (@lines, $last_include_pos+1, 0, "using namespace std;\n");
	if(!$last_include_pos){
		print "Warning, in file ", $fname, "didn't find any includes, inserted at bof\n";
	}

	#write the new contents.
	open (FILE, ">".$fname) or die "couln't write to ".$fname;
	for(@lines){
		print FILE
	}
	close FILE

}

# a cpp file needs a "using namespace std" *after* all include
# directives, if it has any
sub standardize_cpp
{
	if(!needs_using_directive($_[0])){
		print "NOT: $_[0]\n";
		return 0;
	}
	else {
		print "NEEDED: $_[0]\n " ;
	}
	insert_using_directive($_[0]);

}

# a header file needs a std:: before anything the standard namespace,
# and needs to have any "using namespace std" declaration deleted.
sub standardize_h
{
	print "Standarding: $_[0]\n";
	use File::Slurp 'read_file';
	use File::Slurp 'write_file';
	my @lines = read_file($_[0]);

	my $count =0;
	my $commentLength = 0;

	for (my $i=0; $i < scalar @lines;++$i)
	{
		my $curline=$lines[$i];
		if($lines[$i] =~ /using namespace std;/){
#			print "WAS:", $lines[$i];
			$lines[$i] = ""; # get rid of the offending line.
#			print "IS:", $lines[$i];
		} elsif  ($lines[$i] =~ /^\s*#include/){
			# ignore include.
#			print "skipping:  ", $lines[$i];
		} elsif ($lines[$i] =~ /\/\//){
#		 	print "skipping // comment" , $lines[$i];
		} elsif ($lines[$i] =~ /^\s*\/\*/){
			$commentLength = 1;
#			print "skipping ";
			until ($lines[$i] =~ /\*\//){
				++$i;
				++$commentLength;
			}
#			print $commentLength , "line c-style comment";
		}#skip comments
		else {
			foreach(@possible_offenders){
				if($lines[$i] =~ s/(?<=[\s,(<])$_(?=([^"]*"[^"]*")*[^"]*$)(?![[:alnum:]_])/std::$_/g){
					++$count;
#					print $lines[$i];
				}
			}
		}#default behavior
	}# iterate over file content

	write_file ($_[0],@lines);
	return $count;
}


# find the cpp files which need standardization.
sub find_files
{
	return  find(
		file =>
		name => [ "*.cpp", "*.boc", "*.boh", "*.h" ],
		in => getcwd()
		);
}

my @files_to_check = find_files;
my @modified_files;
my @untouched_files;
my $modified = 0;
foreach (@files_to_check){

	my $curname = $_;
 	if(/h$/){
		$modified = standardize_h($curname);
	} else{
		$modified = standardize_cpp($curname);
	}
	if ($modified){
		push(@modified_files, $curname);
	} else {
		push(@untouched_files, $curname);
	}
}
