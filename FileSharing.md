# Filesharing System

In order to share programs in the distributed system to be run, the system for sharing files needs to be secure and ensure accuracy to prevent malicious software as well as prevent requiring a centralized database.

This is done in a similar fashion to torrenting. Each docker container will have a corresponding `.DELUGE` file (like torrent, get it?). This file will contain a list of SHA-1 hashes coresponding with 32kb chunks of the container file. There can be a maximum of 2000 hashes in this list, corresponding with a maximum of 64MB of data in the container file.
The Delugewill contain all necessary information for returning data back to the original recipient (just their wallet address and IP address) which is also used for charging them. It will also have the programID, which is a SHA-256 hash of the entire container, used to ideentify it and differentiate between others.
