# Filesharing System

In order to share programs in the distributed system to be run, the system for sharing files needs to be secure and ensure accuracy to prevent malicious software as well as prevent requiring a centralized database.

## Deluge

This is done in a similar fashion to torrenting. Each docker container will have a corresponding `.DELUGE` file (like torrent, get it?). This file will contain a list of SHA-256 hashes corresponding with 32kb chunks of the container file. There can be a maximum of 4096 hashes in this list, corresponding with a maximum of 128MB of data in the container file.
The Deluge will contain all necessary information for returning data back to the original recipient (just their wallet address and IP address) which is also used for charging them. It will also have the programID, which is a SHA-256 hash of the entire container, used to identify it and differentiate between others.

### Deluge Daemon

The background process that handles all Deluge related activities. These include but are not limited to: asking known peers for deluge files and IDs; replying to any peer asking for a chunk of a deluge file, given by it's ["totalHash"]["chunkHash"] pair; asking peers (from a deluge's "peers" array) for chunks if missing; adding peers to a deluge's "peers" array if they are known to have it's file.

### Process of the lifetime of a Deluge

* Developer creates deluge file, and shares it with as many of it's peers as will accept it.
* Developer allows incoming connections requesting the container data, and should remain open until a sufficient number of peers can seed the data
    * At the same time, each peer that takes a deluge adds it's own IP address to it, begins downloading the container, and shares with the other peers on the deluge list that it can now seed.
* As peers recieve containers, they begin computing it if they are miners.
    * After a peer computes for a given interval, the output data is placed in the folder `wwwdata/tempoutdata` with the temporary name `out.txt`. After the container is ended, the dcc-client will get the SHA-256 hash of the output data, and move the output file to `wwwdata/finaloutdata`, changing the name to the hash, for example: `wwwdata/finaloutdata/fd394f214e71e4aaf995914207d44181ca9e92c2f508afadf06d367f06151f84.txt`.
    * The hash of the output data as well as the IP of the condenser is shared with every peer on the deluge list, as well as other condensers, and if this condenser is the one that solved this block, it will add it to the list within that block.


### Example Deluge File:

```json
{
    "_address": "fd394f214e71e4aaf995914207d44181ca9e92c2f508afadf06d367f06151f84",
    "_chunkSizeB": 32768,
    "_ip": "255.255.255.255",
    "_name": "TestProgram",
    "_totalHash": "8ed9e40f6a2b49d6e3e2b9cea111078f0d66dfba51e3505a0d37b65b296d84ee",
    "_totalSizeB": 27260050,
    "_version": "v0.1.2-alpha-deluge",
    "hashList": [
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "cd372fb85148700fa88095e3492d3f9f5beb43e555e5ff26d95f5a6adc36f8e6",
        "30edb2e712f9c663449d27087decc3513b198f7cc6c61e60fb6542ab2f8a9251",
        "d29a07bd60f2fb2c5b2aa354d32fc0a309d2020818d5cc7db0034fdcebe3e5ef",
        "3ba7803576a645c3f10de04f8289cf712f9e26637c4ce8f988f01a0ffc5c2cd2",
        "9178a2f34f098d5ab419f532c353735c82c41b4bf8572f54466628d48d4cc0a0",
        "989651020bd3ebb4d4130d4a733622969e91c82345130947f7fb81aedca0d9c5",
        .
        .
                               ... 820 more lines ...
        .
        .
        "9cd43451243d9a79d1c9a75a0d31820275bb8af0fd365dadce9f3545a6182d0e",
        "df85353b3015beeda476205d7cf9a6256d40626905442a47407fd62155026eae",
        "dafa07d8531e2e85c4d3ab5ed767aaee99717623430ac26d52c102d2d9ccee7d",
        "0d338ebe8d557fa4bb8db4eaf0f25f6b71ad8bc93cef5f8b31dc28280b8b477a",
        "e9aca86c8be82403921fc3462a9ebe9f7d2b9ee5ba149fda86bd23069c95b70a",
        "87255fd1b1f811c247533cda46d70facd6579c27d9bff8d75cd35059ad227671",
        "9703a55971a89b04a0db7a039a8ef5df6c8250d94854348839018c5637d2c23c"
    ],
    "peers": [
        [
            "255.255.255.255",
            7922
        ]
    ]
}
```
