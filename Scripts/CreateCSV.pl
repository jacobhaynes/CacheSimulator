#!/usr/bin/env perl

my $filepath = shift;
my @files =<$filepath/*.txt>; 

open OUTPUT_CSV, ">Results.csv";  

my $wroteHeader = 0;
foreach my $file (@files)
{
	my $header;
	my $lineCSV; 
	#print $file."\n"; 
	my ($testName) = $file =~ /.*\/(.*)\.txt/; 
	print "$testName\n"; 
	$header = "";
	$lineCSV = $testName; 
	open THISFILE, "<$file";
	
	while (<THISFILE>)
	{
		chomp;
		my $line = $_;
		#print $line."\n"; 
		my ($name, $value) = $line =~ /(.*):? ([\d\.]{1,20})/;
		if($name !~ /filename/ and 
			defined($name) and 
			defined($value) and 
			$line !~ /\|/)
		{
			print "name $name, value $value\n";
			$lineCSV .= ",$value"; 
			$header .= ",$name"; 
		} 
	} 
	if(!$wroteHeader)
	{
		print OUTPUT_CSV "$header\r\n"; 
		$wroteHeader = 1; 
	}
	print OUTPUT_CSV "$lineCSV\r\n";
	close THISFILE; 
	print "\n"; 
}

close OUTPUT_CSV;