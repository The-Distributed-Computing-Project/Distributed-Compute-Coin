Short: Z
Long: parallel
Help: Perform transfers in parallel
Added: 7.66.0
Category: connection curl
Example: --parallel $URL -o file1 $URL -o file2
---
Makes curl perform its transfers in parallel as compared to the regular serial
manner.

This option is global and does not need to be specified for each use of
--next.
