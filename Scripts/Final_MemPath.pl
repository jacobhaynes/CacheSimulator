use strict; 
use warnings; 
use Time::HiRes qw(gettimeofday tv_interval);

my $debug = 0;
my $showOutput = 0; 
my $runBase = 1; 
my $runL1_2Way = 1;
my $runL2_2Way = 1;
my $runAll_2Way = 1;
my $run2_4_Way = 1; 
my $runL2_Big = 1; 
my $runAll_FA = 1; 
my $zcat = "zcat";
my $outputPath = "Output";
my $inputPath = "/Users/jacob/Documents/CompOrg/Files/traces-long";

foreach my $argument (@ARGV)
{
	if($argument eq "--debug")
	{
	  $debug = 1;
	}
	elsif ($argument eq "--mac")
	{
	  $zcat = "gzcat"; 
	}
	elsif($argument eq "--linux")
	{
	  $zcat = "zcat"; 
	}
	elsif($argument eq "--show")
	{
	  $showOutput = 1; 
	}
  elsif($argument eq "--1M")
  {
    $outputPath = "Output-1M";
    $inputPath = "/Users/jacob/Documents/CompOrg/Files/traces-1M"
  }
}

my @testNames = ("gcc.gz"); 
my @configNames = ("Base", "L1-2way", "L2-2way", "All-2way",
                      "2-4-way", "L2-Big", "All-FA",
                  "Base_32", "L1-2way_32", "L2-2way_32", "All-2way_32", "2-4-way_32", "L2-Big_32", "All-FA_32",
                  "Base_64", "L1-2way_64", "L2-2way_64", "All-2way_64", "2-4-way_64", "L2-Big_64", "All-FA_64");

my $mainStartTime = [gettimeofday];
for my $testName (@testNames)
{
  for my $configName (@configNames)
  {
    my ($testNameWithoutExt) = $testName =~ /(.*)\.(.*)/; 
    printf("$testNameWithoutExt, $configName\n"); 
    runFinal("$inputPath/$testName", "Config/$configName.txt", "$outputPath/$configName $testNameWithoutExt.txt");
  }
}
my $mainRunTime = tv_interval ( $mainStartTime, [gettimeofday]);
print "mainRunTime ".$mainRunTime."\n"; 
`echo $mainRunTime > TOTAL_RUN_TIME.txt`; 

sub runFinal {
	my ($file, $configFile, $outputFile ) = @_; 
	
	my $startTime = [gettimeofday];
	
	my $simulationString; 
	if($file =~ /.gz$/)
	{
		$simulationString .= "$zcat " ; 
	}
	else 
	{
		$simulationString .= "cat "
	}
	$simulationString .= "$file | ./Simulator $configFile";
	
	my $results = `$simulationString`;
	
	 open (MYFILE, ">$outputFile"); 
   print MYFILE $results."\n";
   my $runTime = tv_interval ( $startTime, [gettimeofday]);
   print MYFILE "Runtime $runTime"; 
   close (MYFILE); 
   
   
	if($showOutput == 1 or $debug == 1)
	{
			print($results."\n"); 
			print("Runtime ".$runTime."\n"); 
	}
}

sub println {
	print $_[0]."\n"; 
}
