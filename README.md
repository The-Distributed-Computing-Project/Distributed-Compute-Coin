## Working on:
* P2P Networking, allowing people to use clients independently of a server. **The server is often down**, due to development, and since P2P is still in progress, the clients basically don't work.
* Moving from C# to C++. It is currently in a windows-only state, so my goal is to have it be more cross-platform, for linux and mac, and also be lower-level for performance.

# Distributed Compute Coin
A unique P2P blockchain built in C++, with a trading client and a miner. This crypto can be bought, traded, sold, or used just like any other. What sets it apart though is how it can be used. It is based on sharing mining hardware with developers to run their programs better and compute a lot of data, quickly, using Distributed Computing. Anybody can submit code using the client to be run across the peer-to-peer network, and will pay for that using this crypto.

If you want to learn more, please feel free to go to [the wiki.](https://github.com/sam-astro/DC-Blockchain-Cryptocurrency/wiki)

## Installation / Use:
1. First, either clone with `git clone https://github.com/sam-astro/DC-Blockchain-Cryptocurrency` or download it as a ZIP file, and extract it.
2. All of the builds are packaged in this folder. They all follow the same directory map, and the binary is located at `(Program Name) -> bin -> Debug -> (Program Name).exe`.
3. **If you wish to remain as secure as possible and only trade, then you should use the Cold-Wallet.** It won't allow you to upload programs, so it is ideal for sending and receiving. The first time it starts up, your keys will be generated and stored in your .dccwallet file. This may take a minute or two, and progress will be written to the console.
4. **Are you a developer? Then you should use the Standard-Wallet.** Programs are written in a language called Rust. You will need to install that before using ClientCSForm. You can do that by going to the [Rust website](https://www.rust-lang.org/tools/install) and following the instructions there, which is easy and should only take a few minutes. Further instructions on uploads are located at the DCC wiki [here](https://github.com/sam-astro/DC-Blockchain-Cryptocurrency/wiki#upload-program), make sure you have read these and know the rules before uploading your first program.
5. **If you are interested in earning DCC, then you can use the Miner.** The miner verifies the blockchain network, and also run programs created by clients. Just like the clients, programs are written in a language called Rust and you will need to install Rust before using the Miner. You can do that by going to the [Rust website](https://www.rust-lang.org/tools/install) and following the instructions there, which should only take a few minutes. The miner uses a console interface instead of the GUIs of the clients, but it is very simple to use. You will first be prompted to enter your wallet address. This is the address you will receive payment for each block you mine. If you don't have a wallet address yet, then open the Staandard-Wallet or Cold-Wallet and sign in. Your wallet will be generated and shown at the top of the window. Next, you will be asked if you want to stay signed in. This will only store your wallet address so you don't need to sign in the next time you open this program. After that, there are a few options for you:
```
Usage: miner [options]
		 OR (while in interactive mode)
	   DCC >  [options]
Options:
  -h, --help						  Display this help menu
  -s, --sync						  Manually re-sync blockchain
  -m, --mine <amount>			      Mine <amount> number of blocks, defaults to 1 if not specified
  -ma, --mineany <block num> <dif>	  (Debug) Mines the block specified by <block num> at the given difficulty <dif>
```
Inputting one of these commands and pressing the Enter key will execute the command. To begin mining, simply type "mine" (and a block amount, if you want to mine more than one).
