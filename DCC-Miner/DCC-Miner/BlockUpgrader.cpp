

class BlockchainUpgrader
{
public:
	Block Upgrade(Block b, string toVersion)
	{
		Console.ForegroundColor = ConsoleColor.Black;
		Console.BackgroundColor = ConsoleColor.Cyan;
		Console.WriteLine("Upgrading block to version " + toVersion);
		Console.ResetColor();

		if (toVersion == "v0.01alpha-coin")
		{
			b.Version = toVersion;
		}

		return b;
	}
}
