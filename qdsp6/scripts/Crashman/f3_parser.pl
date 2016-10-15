#!perl.exe 

#$f3_input = $ARGV[0];
$oppath=$ARGV[0];
$num_of_lines=0;
$opfullpath=$oppath."\/DSPAnalysis.txt";
$f3_input=$oppath."\/f3log.txt";

open(F3IPFILE,"<$f3_input") or die "$f3_input: $!\n";
open (OUTFILE, ">>$opfullpath") or die " cant open output file: $!\n";

$entry=1;
$entry1=1;
$inputline=<F3IPFILE>;
while ($inputline ne "") {
if($inputline =~ /\w+.+/)
{
$num_of_lines=$num_of_lines+1;
}
$inputline=<F3IPFILE>;
}
#$num_of_lines=$.;
#print"\n$num_of_lines\n";
$cnt=0;
close(F3IPFILE);

open(F3IPFILE,"<$f3_input") or die "$f3_input: $!\n";
$inputline=<F3IPFILE>;
while ($inputline ne ""){

	if ($cnt>=($num_of_lines-20))
	{
	#print"\nPramod\n";
	#print"$inputline";
	if($entry ==1)
	{
	$entry=0;
	print OUTFILE ("\n");
	print OUTFILE ("Last few messages in F3 logs:\n");
	print OUTFILE ("----------------------------\n");
	}
	print OUTFILE ("$inputline");
	}
	else
	{
	#(grep (/^$unique$/,@first_list_new))
	if (grep (/fail/i,$inputline) || grep (/error/i,$inputline) || grep (/underrun/i,$inputline) || grep (/overflow/i,$inputline) || grep (/UnSupported/i,$inputline)) {
	if($entry1 ==1)
	{
	$entry1=0;
	print OUTFILE ("\nErrors found in F3 logs:\n");
    print OUTFILE ("------------------------\n");
	}
	#if (($inputline =~ /fail/i) || ($_ =~ /error/i) || ($_ =~ /underrun/i) || ($_ =~ /overflow/i) || ($_ =~ /UnSupported/i)) {
	print OUTFILE ("$inputline");
	#print"$inputline";
	}
	}
	$inputline=<F3IPFILE>;
	$cnt++
	}
	
close(F3IPFILE);
close(OUTFILE);