use strict; 
use warnings; 

my $debug = 1;
my $showOutput = 0; 
my $testBase = 1; 
my $testL1_2Way = 1;
my $testL2_2Way = 1;
my $testAll_2Way = 1;
my $test2_4_Way = 1; 
my $testL2_Big = 1; 
my $testAll_FA = 1; 
my $zcat = "zcat";

foreach my $argument (@ARGV)
{
	if($argument eq "--debug")
	{
	  print "Yo\n"; 
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
} 

print @ARGV."\n"; 

my $shortT1Result = `cat Files/traces-short/I1 | ./Simulator Base.txt`;
my ($executionTimeT1) = $shortT1Result  =~ /Final Execution Time: (\d*)$/; 

if($testBase == 1)
{
	println("Base"); 
	runTest("traces-short/I1", 676, "Base.txt"); 
	runTest("traces-short/I2", 1180, "Base.txt"); 
	runTest("traces-short/I3", 852, "Base.txt"); 
	runTest("traces-short/I4", 2635, "Base.txt"); 
	runTest("traces-short/I5", 5314, "Base.txt"); 
	runTest("traces-short/I10", 7345, "Base.txt"); 
	runTest("traces-1M/gcc.gz", 5887028, "Base.txt"); 
}

if($testL1_2Way == 1)
{
	println("L1 2Way"); 
	runTest("traces-short/I1", 676, "L1-2way.txt"); 
   runTest("traces-short/I2", 1180, "L1-2way.txt"); 
	runTest("traces-short/I3", 852, "L1-2way.txt"); 
	runTest("traces-short/I4", 2635, "L1-2way.txt"); 
	runTest("traces-short/I5", 5314, "L1-2way.txt"); 
	runTest("traces-short/I10", 6262, "L1-2way.txt"); 
	runTest("traces-1M/gcc.gz", 5284198, "L1-2way.txt"); 
}

if($testL2_2Way == 1)
{
	println("L2 2Way"); 
	runTest("traces-short/I1", 676, "L2-2way.txt"); 
   runTest("traces-short/I2", 1180, "L2-2way.txt"); 
	runTest("traces-short/I3", 852, "L2-2way.txt"); 
	runTest("traces-short/I4", 2635, "L2-2way.txt"); 
	runTest("traces-short/I5", 5314, "L2-2way.txt"); 
	runTest("traces-short/I10", 7345, "L2-2way.txt"); 
	runTest("traces-1M/gcc.gz", 5137222, "L2-2way.txt"); 
}


if($testAll_2Way == 1)
{
	println("All 2Way"); 
	runTest("traces-short/I1", 676, "All-2way.txt"); 
   runTest("traces-short/I2", 1180, "All-2way.txt"); 
	runTest("traces-short/I3", 852, "All-2way.txt"); 
	runTest("traces-short/I4", 2635, "All-2way.txt"); 
	runTest("traces-short/I5", 5314, "All-2way.txt"); 
	runTest("traces-short/I10", 6262, "All-2way.txt");  
	runTest("traces-1M/gcc.gz", 4793250, "All-2way.txt"); 
}


if($test2_4_Way == 1)
{
	println("2-4-Way"); 
	runTest("traces-short/I1", 676, "2-4-way.txt"); 
   runTest("traces-short/I2", 1180, "2-4-way.txt"); 
	runTest("traces-short/I3", 852, "2-4-way.txt"); 
	runTest("traces-short/I4", 2635, "2-4-way.txt"); 
	runTest("traces-short/I5", 5314, "2-4-way.txt"); 
	runTest("traces-short/I10", 6262, "2-4-way.txt"); 
	runTest("traces-1M/gcc.gz", 4147286, "2-4-way.txt"); 
}

if($testL2_Big == 1)
{
	println("L2-Big"); 
	runTest("traces-short/I1", 676, "L2-Big.txt"); 
   runTest("traces-short/I2", 1180, "L2-Big.txt"); 
	runTest("traces-short/I3", 852, "L2-Big.txt"); 
	runTest("traces-short/I4", 2635, "L2-Big.txt"); 
	runTest("traces-short/I5", 5314, "L2-Big.txt"); 
	runTest("traces-short/I10", 6262, "L2-Big.txt"); 
	runTest("traces-1M/gcc.gz", 4718976, "L2-Big.txt"); 
}

if($testAll_FA == 1)
{
	println("All-FA"); 
	runTest("traces-short/I1", 676, "All-FA.txt"); 
   runTest("traces-short/I2", 1180, "All-FA.txt"); 
	runTest("traces-short/I3", 852, "All-FA.txt"); 
	runTest("traces-short/I4", 2635, "All-FA.txt"); 
	runTest("traces-short/I5", 5314, "All-FA.txt"); 
	runTest("traces-short/I10", 6262, "All-FA.txt"); 
	runTest("traces-1M/gcc.gz", 3394701, "All-FA.txt"); 
	
}


sub runTest {
	my ($file, $expectedTime, $configFile ) = @_; 
	
	my $simulationString; 
	if($file =~ /.gz$/)
	{
		$simulationString .= "$zcat " ; 
	}
	else 
	{
		$simulationString .= "cat "
	}
	$simulationString .= "Files/$file | ./Simulator $configFile";
	
	my $results = `$simulationString`;

	my($myTime) = $results  =~ /Final Execution Time: (\d*)/; 
	if(not defined($myTime))
	{
		$myTime = -1; 
		 
	}
	if($showOutput == 1)
	{
			print($results."\n"); 
			if($debug == 1)
      {
			  print($results); 
	    }
	}
	
	if($myTime != $expectedTime)
	{
		println("Failed $file $configFile : Expected $expectedTime Mine $myTime");
	}
	else
	{
		println("Passed $file $configFile : $expectedTime");
	}
}

sub println {
	print $_[0]."\n"; 
}
