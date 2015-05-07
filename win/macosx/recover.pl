#!/usr/bin/perl

# NetHack 3.6  recover.pl $NHDT-Date$  $NHDT-Branch$:$NHDT-Revision$
# NetHack 3.6  recover.pl $Date: 2011/10/17 01:29:20 $  $Revision: 1.3 $
# Copyright (c) Kenneth Lorber, Kensington, Maryland, 2009
# NetHack may be freely redistributed.  See license for details.

# Wrapper for 3.4.3 recover to be called from Applescript to reset the Qt
# package after it locks up due to a bug in suspend handling.

# find the right place
($playground = $0) =~ s!/recover.pl$!!;
if(! -d $playground){
	print "Cannot find playground $playground.";
	exit 0
}
if(! -f "$playground/castle.lev" && ! -f "$playground/nhdat"){
	print "Failed to find playground $playground.";
	exit 0
}
print "Playground is $playground.\n";
chdir $playground or do {
	print "Can't get to playground.\n";
	exit 0
};
if(-e 'perm_lock'){
	print "Attempting to remove perm_lock.\n";
	$try_perm = 1;
	unlink 'perm_lock';
} else {
	print "Did not find perm_lock (this is OK).\n";
}
if(-e 'perm_lock'){
	print "Failed to remove perm_lock: $!\n";
	exit 0
}
if($try_perm){
	print "Removed perm_lock.\n";
}

# run recover, but only if there is something that looks promising
$recover = "./recover";
$recover = "/usr/games/bin/recover" unless(-e $recover);

$uid = $<;
foreach ( <$uid*.0> ){
	system ("$recover -d . $_");
}

print "Done.\n";

exit 0
