#!/usr/bin/perl -w

my $filename;
my $infile;
my $version;
my $msversion;
my $tmpdir = "/tmp";
my $tmpfile = "$tmpdir/setrc$$.tmp";

$filename = shift or die "No resource file specified. Exiting.\n";
$infile = $filename.".in";
$version = shift or die "No version number specified. Exiting.\n";
$msversion = shift or die "No MS version number specified. Exiting.\n";

print "Setting version $version ($msversion) in resource file $filename.\n";

open ( fhTMPL, "<$infile" ) or die "Could not open resource template file $filename for reading. Exiting.\n";
open ( fhFILE, ">$filename" ) or die "Could not open resource file for writing. Exiting.\n";
while ( <fhTMPL> ) {
  s/==FVERSION==/$msversion/;
  s/==PVERSION==/$msversion/;
  s/==VERSION==/$version/;
  print fhFILE $_;
}
close ( fhFILE );
close ( fhTMPL );

print "Done.\n";

