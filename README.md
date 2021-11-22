# Distributed Computing Blockchain Cryptocurrency
A unique blockchain built in C#, with a client for trading, a host for distribution, and a miner for mining. This crypto can be bought, traded, sold, or used just like any other. What sets it apart though is how it can be used. I am implementing a system into it just like the Folding@home project with distributed computing power. Anybody will be able to submit code to be computed or hosted across the peer-to-peer network, and will pay for that using this crypto.

If you want to learn more, please feel free to go to [the wiki.](https://github.com/sam-astro/DC-Blockchain-Cryptocurrency/wiki)

## How to download and use:
1. First, either clone with `git clone https://github.com/sam-astro/DC-Blockchain-Cryptocurrency` or download it as a ZIP file, and extract it.
2. All of the programs are packaged in this folder. They all follow the same directory map, and the binary is located at `(Program Name) -> bin -> Release -> (Program Name).exe`.
3. **If you wish to remain as secure as possible and only trade, then you should use the Cold Wallet.** It won't allow you to upload programs, so it is ideal for sending and receiving. The first time it starts up, your keys will be generated and stored in your .dccwallet file. This may take a minute or two, and progress will be written to the console.
4. **If creating programs to run is important to you then you should use the ClientCSForm.** Programs are written in a language called Rust. You will need to install that before using ClientCSForm. You can do that by going to the [Rust website](https://www.rust-lang.org/tools/install) and following the instructions there, which should only take a few minutes. Further instructions on uploads are located [here](https://github.com/sam-astro/DC-Blockchain-Cryptocurrency/wiki#upload-program), make sure you have read these and know the rules before uploading.
5. **If you are interested in earning DCC, then you can use the Miner.** The miner hashes blocks for verifying the blockchain network, and also run programs created by clients. Just like the clients, programs are written in a language called Rust and you will need to install Rust before using the Miner. You can do that by going to the [Rust website](https://www.rust-lang.org/tools/install) and following the instructions there, which should only take a few minutes.
