#!/usr/bin/perl
use strict;
use warnings;

my $file;
my $out_file='out.data';
my $plot_file='out.plot';
my $out_folder="1D";
my $is_x=0;
my $is_y=0;
my $cut;
my $x;
my $y;
my $k;
my $value;
my $point;
my $line;
my %points;
my @keys;
my $i;
my $j;
my $time;
my $phys_value;
my $label_name;
my $axe;
my $cmd;
my %values;
my %x_points;
my %y_points;
my $y_min;
my $was_e=0;
my $syntax_exc_message = "Syntax must be: [path to the file] [axe]=[axe value], ..\n";
my $title="";
my $label="";
my $name="";
my $data_files_list="";
my $delimeter="";
my $linestyle="";
my $file_number=0;

$cmd=$ARGV[0];
$axe = substr ($cmd,0,index ($cmd,"="));
$cut = substr ($cmd,index ($cmd,"=")+1);

if($axe eq "x")
	{$is_x=1;}
else {if($axe eq "y")
	{$is_y=1}
	else
		{print "Excenption. Keys format: [axe]=[cutted axe value] [path to the file]"; exit(1);};
}
if(@ARGV > 2) {$delimeter = ",";}

for($j=1; $j<@ARGV; $j++){
	$file=$ARGV[$j];
	print $j.": file \"".$file."\"\n";

	$linestyle = "with linespoints pt $j";

	if($j == (@ARGV-1)) {$delimeter="";}

	open(PLOT,"<".$file);
	$i=0;
	while(<PLOT>)
	{
		$_ =~ s/\n//;
		$line = $_;
		$line =~ s/^[ ]*//;
		if($line =~ /^set[ ]*?title/){
			$line =~ s/set[ ]*?title[ ]*?//;
			$line =~ s/\"//;
			$line =~ s/\"//;
			$line =~ s/^[ ]*//;
			$time = substr($line, 0, index($line, ","));
			$time =~ s/\[\w*?\]//;
			$time =~ s/t[ ]*?=[ ]*?//;

			$line = substr ($line,index ($line,",")+1);
			if(($title ne "") and (index ($title,$line) < 0)) {
				print "Incmpatible physical values $title and $line\.\n";
				exit(1);
			}
			$title = $line;
			$out_file = 't='.$time.' [sec]';
		}
		if(($line =~ /^set xlabel/) and $is_x){$label = $line;}
		if(($line =~ /^set xlabel/) and $is_y){$label = $line;}
		if($line =~ /^set output/){
			$line=~s/set output \"res//;
			$line=~s/\w*\///;
			$line=~s/\w*\///;
			$line=~s/\w*\///;
			$line=~s/\"//;

#			$phys_value=$line;
#			$phys_value=~s/[\.E\+\d]*_//;
#			$label_name=$phys_value;
#			$phys_value=~s/_[\w\.]*//;
#			$label_name=~s/\w*_//;
#			$label_name=~s/\.\w*//;
#			$label_name=~s/\.\w*//;

#			if($is_x) {$label_name=substr($label_name,0,1);}
#			if($is_y) {$label_name=substr($label_name,1,1);}

			$phys_value = "phys_value";
			$label_name = "label_name";
			$name = $phys_value."_".$label_name.".gif";
		}

		if($line =~ /^[ ]*e[ ]*$/){$was_e = 0;}

		if($line =~ /^[ ]*[+-]?\d+\.[\dE+-]*[ ]*[+-]?\d+\.[\dE+-]*[ ]*[+-]?\d+\.[\dE+-]*[ ]*$/){
			$x = $line;
			$y = $line;
			$value = $line;

			$x =~ s/[ ]*[\d+-]*\.[\d+-E]*[ ]*$//;
			$y = $x;

			$x =~ s/[ ]*[\d\.+-E]*$//;
			$y =~ s/^[\d\.+-E]*[ ]*//;

			$value =~ s/$x//;
			$value =~ s/$y//;

			$value =~ s/[ ]*//;
			$x =~ s/[ ]*//;
			$y =~ s/[ ]*//;		

			if($was_e == 0) {$i++;$values{$i}=0.0;$x_points{$i}=0.0;$y_points{$i}=0.0;}
			$values{$i} += 0.25*$value;
			$x_points{$i} += 0.25*$x;
			$y_points{$i} += 0.25*$y;

			$was_e = 1;
		}
	}
	close(PLOT);

	$y_min = 1000;
	if($is_y){
		foreach $k (keys %y_points){
			if(abs($cut - $y_points{$k}) < abs($cut - $y_min)) {$y_min = $y_points{$k}};
		}

		foreach $k (keys %y_points){
			if($y_points{$k} == $y_min){
				$points{$x_points{$k}} = $values{$k};
			}
		}
	}
	if($is_x){
		foreach $k (keys %x_points){
			if(abs($cut - $x_points{$k}) < abs($cut - $y_min)) {$y_min = $x_points{$k}};
		}

		foreach $k (keys %x_points){
			if($x_points{$k} == $y_min){
				$points{$y_points{$k}} = $values{$k};
			}
		}
	}

	$i=0;
	foreach $k (keys %points){
		$keys[$i++]=$k;
	}

	$out_file = $out_file;
	if(index ($data_files_list,$out_file) > 0){
		$file_number++;
		$out_file = $out_file." ($file_number)";
	}
	$data_files_list = $data_files_list."\'".$out_file."\' ".$linestyle.$delimeter." ";

	open(OUTF,'>'.$out_file);
	foreach my $k (sort { $b <=> $a } @keys) {
		print OUTF $k."\t".$points{$k}."\n";
	}
	close(OUTF);
}

open(OUTP,'>',$plot_file);
	print OUTP " load \"1D.plot\"\n";
	print OUTP "set title \"".$title." (".$axe."=".$cut.")"."\"\n";
	print OUTP $label."\n";
	print OUTP "set output \"".$out_folder."/".$name."\"\n";
	print OUTP " plot $data_files_list"."\n";
close(OUTP);

`gnuplot '$plot_file'`;
`rm t\\=*`;
