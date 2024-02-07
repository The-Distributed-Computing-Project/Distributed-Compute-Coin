# Filesharing System

In order to share programs in the distributed system to be run, the system for sharing files needs to be secure and ensure accuracy to prevent malicious software as well as prevent requiring a centralized database.

## Deluge

This is done in a similar fashion to torrenting. Each docker container will have a corresponding `.DELUGE` file (like torrent, get it?). This file will contain a list of SHA-256 hashes corresponding with 32kb chunks of the container file. There can be a maximum of 4096 hashes in this list, corresponding with a maximum of 128MB of data in the container file.
The Deluge will contain all necessary information for returning data back to the original recipient (just their wallet address and IP address) which is also used for charging them. It will also have the programID, which is a SHA-256 hash of the entire container, used to identify it and differentiate between others.
