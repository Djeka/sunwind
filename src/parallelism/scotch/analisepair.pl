#!/usr/bin/perl

$size=$ARGV[0];
$parts=$ARGV[1];
open(FD,"srcmesh-$size-$size-$size.xyz");
$xyzformat=<FD>;
$xyzsize=<FD>;
chomp($xyzsize);
while(<FD>) {
   chomp;
   ($id,$x,$y,$z) = split(/\s+/);
   $coords{$id}{'x'} = $x;
   $coords{$id}{'y'} = $y;
   $coords{$id}{'z'} = $z;
};

close FD;

open(FD, "srcmesh-$size-$parts.map");
$mapsize=<FD>;
chomp($mapsize);
while(<FD>) {
   chomp;
   ($id,$p) = split(/\s+/);
   $part{$id}=$p;
   $partsize{$p}+=1;
   $internallinks2{$p}=0;
   $externallinks2{$p}=0;
};
close FD;
@realparts=keys(%partsize);
$realpartno=$#realparts+1;
open(FD,"srcproc-$size-$size-$size.grf");
$type=<FD>;
$line=<FD>;
chomp($line);
($grfsize,$links2) = split(/\s+/,$line);
$i=0;
while(<FD>) {
   chomp;
   @list=split(/\s+/);
   $lsize=$list[0];
   for($j=0;$j<$lsize;$j++) {
     $linked{$i}{$list[1+$j]} = 1;
     $linked{$list[1+$j]}{$i} = 1;
     if($part{$i} == $part{$list[1+$j]}) {
#  same part.
        $internallinks2{$part{$i}} += 2;
     } else {
        $externallinks2{$part{$i}} += 1;
        $externallinks2{$part{$list[1+$j]}} += 1;
     };
   };
   $i++;
};
close FD;
###################################
#
#  Ok, now everything is read.
#
###################################
$minpartsize=$partsize{$p};
$maxpartsize=$partsize{$p};
$mininternallinks2=$internallinks2{$p};
$maxinternallinks2=$internallinks2{$p};
$minexternallinks2=$externallinks2{$p};
$maxexternallinks2=$externallinks2{$p};
foreach $p (keys(%partsize)) {
  if($partsize{$p}>$maxpartsize) {
     $maxpartsize=$partsize{$p};
  };
  if($partsize{$p}<$minpartsize) {
     $minpartsize=$partsize{$p};
  };
  if($internallinks2{$p}>$maxinternallinks2) {
    $maxinternallinks2 = $internallinks2{$p};
  };
  if($internallinks2{$p}<$mininternallinks2) {
    $mininternallinks2 = $internallinks2{$p};
  };
  if($externallinks2{$p}>$maxexternallinks2) {
    $maxexternallinks2 = $externallinks2{$p};
  };
  if($externallinks2{$p}<$minexternallinks2) {
    $minexternallinks2 = $externallinks2{$p};
  };
 # print "$p ".$partsize{$p}." ".$externallinks2{$p}." ".$externallinks2{$p}."\n";
};

print "$size $parts $realpartno $minpartsize $maxpartsize $mininternallinks2 $maxinternallinks2 $minexternallinks2 $maxexternallinks2\n";
