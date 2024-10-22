This repo's goal is to capture all of the historical releases of NetHack in one
repo. It's mostly for archival purposes.

# Branches

The [grafted branch](../../tree/grafted), which is the default, is a graft of
the pre-version-control releases onto the official NetHack repo. Using this you
can do fun things like identify chunks of code from 30+ years ago still present
in modern NetHack.

The [releases branch](../../tree/releases) is the base of the grafted branch,
and contains the releases up to version 3.3.1.

# Methodology

For all versions prior to NetHack 3.2.0, the source was fetched from the UTZOO
Usenet archives. Each release's shell-archives were unpacked and applied (with
a legacy version of `patch`, where appropriate) to get an authentic
representation of what the source was upon its original release.

Versions from 3.2.0 and on were fetched from archives of FTP servers.

The patches were not always perfect even in their original form, and I
intervened to fix failed hunks (similar to how original users would have had
to). Most of these hunk fixes were trivial. The most complex but not
particularly interesting example is the patch for 3.0.1, which attempted to
apply contextual changes to a Mac binary sound file that did not exist in 3.0.0
(`src/mac/mrecover.hqx`). Thankfully the patch hunk contained the entirety of
the new file, so I was able to restore it fully.

The commit dates I've taken from the Usenet post authorship information. I've
also included the message from the announcement post in the commit message.
