# Pixmicat! Template-Embedded Library v070606 (Perl port)
# Copyright(C) 2005-2007 Pixmicat! Development Team
# PHP version by: scribe & RT
# Perl Port by RT
#
# Pixmicat! Template-Embedded Library (PTE) is released under The Clarified 
# Artistic License.
# A more detailed definition of the terms please refer to the attached 
# "LICENSE" file. If you do not receive the program with The Artistic License 
# copy, please visit http://pixmicat.openfoundry.org/license/ to obtain a copy.

# class PTE
package PTE;

# exported functions
@EXPORT_OK = qw(ParseBlock EvalIF EvalFOREACH EvalInclude);

# constructor - read template file
# parameter 1 - template file name
sub new {
	my $invocant = shift; # Class name
	my $class   = ref($invocant) || $invocant; # Object or class name
	my $tplfile = shift; # template name
	
	open(FH, $tplfile) || die("Error: $!");
	my @tpl_data = <FH>;
	close(FH);
	
	my $this = {
		tpl => join("",@tpl_data),
		tpl_block  => (),
	};
	return bless $this, $class;
}

# readBlock - get Block from template data
# parameter 1 - block name
sub readBlock {
	my ( $this, $blockName ) = @_;
	if(!defined $this->{tpl_block}{$blockName}){ # found before?
		if($this->{tpl} =~ /<\!--&($blockName)-->(.*?)<!--\/&\1-->/sm) {
			$this->{tpl_block}{$blockName} = $2; # place it in hash
		} else {
			$this->{tpl_block}{$blockName} = undef; # not found
		}
	}
	return $this->{tpl_block}{$blockName};
}

# ParseBlock - Parse the block and replace statements and variables, output parsed string
# parameter 1 - block name
# parameter 2 - reference hash of arguments
sub ParseBlock {
	my ( $this, $blockName, $ary_val ) = @_;
	my $tmp_block = $this->readBlock($blockName);
	if(!$tmp_block) { return ""; } # not found
	$tmp_block = $this->EvalFOREACH($tmp_block, $ary_val); # eval FOREACH statements
	$tmp_block = $this->EvalIF($tmp_block, $ary_val); # eval IF statements
	$tmp_block = $this->EvalInclude($tmp_block, $ary_val); # eval Includes
	return $this->str_array_replace([keys %$ary_val], [values %$ary_val], $tmp_block); # replace variables
}

# EvalIF - evaluate IF statements, output parsed string
# parameter 1 - block
# parameter 2 - reference hash of arguments
sub EvalIF {
	my ( $this, $tpl, $ary ) = @_;
	my $tmp_tpl = $tpl;
	my @vari = my @statement = my @iftrue = my @iffalse = ();
	my $ifres = '';
	while($tmp_tpl =~ /<\!--&IF\((\$.*?),'(.*?)','(.*?)'\)-->/smg){ # separating search and replace part because it will cause pointer reset
		push @statement, $&; push @vari, $1; push @iftrue, $2; push @iffalse, $3;
	}
	for(my $i=0;$i<=$#statement;$i++) {
		$tmp_tpl = $this->str_replace($statement[$i], $$ary{'{'.$vari[$i].'}'}?$this->EvalInclude($iftrue[$i],$ary):$this->EvalInclude($iffalse[$i],$ary), $tmp_tpl);
	}
	return $tmp_tpl;
}
# EvalFOREACH - evaluate FOREACH statements, output parsed string
# parameter 1 - block
# parameter 2 - reference hash of arguments
sub EvalFOREACH {
	my ( $this, $tpl, $ary ) = @_;
	my $tmp_tpl = $tpl;
	my @vari = my @statement = my @block = ();
	my $eachvar;
	while($tmp_tpl =~ /<\!--&FOREACH\((\$.*?),\'(.*?)\'\)-->/smg){
		push @statement, $&; push @vari, $1; push @block, $2;
	}
	my $foreach_tmp = '';
	for(my $i=0;$i<=$#vari;$i++) {
		if(defined($$ary{'{'.$vari[$i].'}'}) && ref($$ary{'{'.$vari[$i].'}'}) eq 'ARRAY') {
			foreach $eachvar (@{$$ary{'{'.$vari[$i].'}'}}) {
				$foreach_tmp .= $this->ParseBlock($block[$i], $eachvar);
			}
		}
		$tmp_tpl = $this->str_replace($statement[$i], $foreach_tmp, $tmp_tpl);
	}
	return $tmp_tpl;
}
# EvalInclude - evaluate Includes, output parsed string
# parameter 1 - block
# parameter 2 - reference hash of arguments
sub EvalInclude {
	my ( $this, $tpl, $ary ) = @_;
	my $tmp_tpl = $tpl;
	my @statement = my @block = ();
	while($tmp_tpl =~ /<\!--&([^\(\)']*)\/-->/smg) {
		push @statement, $&; push @block, $1;
	}
	for(my $i=0;$i<=$#block;$i++) {
		$tmp_tpl = $this->str_replace($statement[$i], $this->ParseBlock($block[$i], $ary), $tmp_tpl);
	}
	return $tmp_tpl;
}

# str_array_replace - Perl equivalent of PHP's str_replace(array,array,string)
# parameter 1 - reference array of search strings
# parameter 2 - reference array of replace strings
# parameter 3 - target string
sub str_array_replace {
	my ($this, $search, $replace, $string) = @_;

	for(my $i=0;$i<=$#$search;$i++) {
		$string = $this->str_replace($$search[$i], $$replace[$i], $string);
	}

	return $string;
}

# str_array_replace - Perl equivalent of PHP's str_replace(string,string,string)
# parameter 1 - search string
# parameter 2 - replace string
# parameter 3 - target string
sub str_replace {
	my ($this, $search, $replace, $string) = @_;

	$string =~ s/\Q$search/$replace/g; # \Q = take away regular expression's power of $search

	return $string;
}
