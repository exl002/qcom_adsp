#!/usr/local/bin/perl
$dump_size_file = $ARGV[0];
if ($#ARGV !=0)
{
print "usage: perl $0 <file_name containing_details_of_dump_size>";
exit;
}
open IN, "< $dump_size_file" or die "cant open the file $!\n";
@dump_size_file_content = <IN>;
close(IN);
open OUT, "> $dump_size_file" or die "cant open the file $!\n";
foreach(@dump_size_file_content)
{
print OUT $1 if ($_ =~m/(\S+)\s+bytes\s+$/);
}
close(OUT);
