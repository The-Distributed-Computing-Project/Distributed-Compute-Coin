<?php
// Convert argv into associative array
function parse_argv(array $argv): array
{
    $request = [];
    foreach ($argv as $i => $a) {
        if (!$i) {
            continue;
        }

        if (preg_match("/^-*(.+?)=(.+)$/", $a, $matches)) {
            $request[$matches[1]] = $matches[2];
        } else {
            $request[$a] = true;
        }
    }

    return $request;
}

if (!empty($argv[1])) {
    $_GET = parse_argv($argv);
}

if(isset($_GET["query"]))
	$query = $_GET["query"];

$blockchain = __DIR__ . "/blockchain/";
$pendingBlockchain = __DIR__ . "/pendingblockchain/";

if(isset($_GET["username"]))
	$username = $_GET["username"];
if(isset($_GET["password"]))
	$password = $_GET["password"];

if(isset($_GET["recipientAddress"]))
	$recipientAddress = $_GET["recipientAddress"];
if(isset($_GET["fromAddress"]))
	$givenFromAddr = $_GET["fromAddress"];
$fromAddress = generateWalletAddress();

$completedBlocks = array_slice(scandir($blockchain), 2);
$pendingBlocks = array_slice(scandir($pendingBlockchain), 2);

$amountOfBlocks = count($completedBlocks);
$amountOfPendingBlocks = count($pendingBlocks);

function submitBlock()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if(!isset($_GET["blockNum"]) || !isset($_GET["programID"]) || intval($_GET["blockNum"]) <= $amountOfBlocks || intval($_GET["blockNum"]) > $amountOfBlocks + $amountOfPendingBlocks)
    {
        echo "ERR: block number is missing or bad, canceling connection";
        return "ERROR";
    }
	
	$ourLines = file($pendingBlockchain . "block" . $_GET["blockNum"] . ".txt");
	$lastHash = trim(explode("\n", file_get_contents($blockchain . "block" . strval($amountOfBlocks) . ".txt"))[1]);
	
	$data = strval($lastHash);
	for($x = 3; $x < count($ourLines); $x++)
	{
		$data .= $ourLines[$x];
	}
	$data .= $_GET["nonce"];
	$data = str_replace(" ", "", str_replace("\n", "", $data));
	echo "data calculated: " . $data . "\n";
	$hashedData = hash("sha256", $data);
	
	if($hashedData != $_GET["minedHash"])
	{
		echo "ERR: block mined incorrectly as " . $hashedData . ", canceling connection";
		return "ERROR";
	}
	if(strpos($lastHash, trim(file("minelevel.txt")[0])) && strpos($lastHash, trim(file("minelevel.txt")[0])) != 0)
	{
		echo "ERR: mined block has not used the correct mining level, canceling connection";
		return "ERROR";
	}
	
	if(time() - intval(trim(file("minelevel.txt")[1])) > 31)
	{
		$strng = trim(file("minelevel.txt")[0]);
		if(strlen($strng) > 2)
		{
			$strng = mb_substr($strng, 0, -1) . "\n";
			$strng .= time();
			file_put_contents("minelevel.txt", $strng, LOCK_EX);
		}
	}
	else if(time() - intval(trim(file("minelevel.txt")[1])) < 29)
	{
		$strng = trim(file("minelevel.txt")[0]);
		$strng = $strng . "0\n";
		$strng .= time();
		file_put_contents("minelevel.txt", $strng, LOCK_EX);
	}
	
	$blockFile = $blockchain . "block" . $_GET["blockNum"] . ".txt";
	$strng = "";
	$strng .= trim($lastHash) . "\n";
	$strng .= trim($_GET["minedHash"]) . "\n";
	$strng .= trim($_GET["nonce"]) . "\n";
	for($x = 3; $x < count($ourLines); $x++)
	{
		$strng .= trim($ourLines[$x]) . "\n";
	}
	file_put_contents($blockFile, $strng, FILE_APPEND | LOCK_EX);
	
	unlink($pendingBlockchain . "block" . $_GET["blockNum"] . ".txt");
	$amountOfBlocks++;
	$amountOfPendingBlocks--;
	createPendingBlock(trim("1->" . $givenFromAddr) . "\n");
	echo "successfully submitted block";
	
	$outFiles = array_slice(scandir(__DIR__ . "/programs/" . $_GET["programID"] . "-out"), 2);
	$uploads_dir = __DIR__ . "/programs/" . $_GET["programID"] . "-out";
	
	$highestNumber = 0;
	foreach($outFiles as $file)
	{
		if(intval(explode(".", explode("out", $file)[1])[0]) > $highestNumber)
		{
			$highestNumber = intval(explode(".", explode("out", $file)[1])[0]);
		}
	}
	
	if ($_FILES["file"]["error"] == UPLOAD_ERR_OK)
	{
			$tmp_name = $_FILES["file"]["tmp_name"];
			$name = "out" . strval($highestNumber + 1) . ".txt";
			move_uploaded_file($tmp_name, $uploads_dir . "/" . $name);
			if(file_exists($uploads_dir . "/" . $name))
			{
				echo "Program data successfully uploaded as: " . str_replace(".txt", "", $name);
				
				$cost = floatval(trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[4])) * ($_GET["time"] / 60.0 / floatval(trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[3])));
				
				$contents = trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[0]) . PHP_EOL . trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[1]) . PHP_EOL . trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[2]) . PHP_EOL . floatval(trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[3])) - $_GET["time"] / 60.0 . PHP_EOL .trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[4]) . PHP_EOL .trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[5]); 
				file_put_contents(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg", $contents, LOCK_EX);
				
				$blockFile = $pendingBlockchain . "block" . ($amountOfBlocks + $amountOfPendingBlocks) . ".txt";
				file_put_contents($blockFile, $cost . "->" . $fromAddress . "->dcc1f4e25f12ae7460b9f1430a5ed1858384368b70435855570cdacdc0338b708f4".PHP_EOL , FILE_APPEND | LOCK_EX);
			}
			else
				echo "Program data failed to upload: " . str_replace(".txt", "", $name);
	}
	else
		echo "Program data failed to upload: " . str_replace(".txt", "", $name);
}

function createPendingBlock($addOnEnd)
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if($amountOfPendingBlocks < 1)
	{
		$blockFile = $pendingBlockchain . "block" . ($amountOfBlocks + 1) . ".txt";
		$strng = "";
		$strng .= trim(file($blockchain . "block" . ($amountOfBlocks) . ".txt")[1]) . "\n";
		$strng .= "0000000000000000000000000000000000000000000000000000000000000000\n";
		$strng .= "0\n";
		$strng .= $addOnEnd;
		file_put_contents($blockFile, $strng, FILE_APPEND | LOCK_EX);
		echo "created new block: " . ($amountOfBlocks + 1) . "\n";
		$amountOfPendingBlocks++;
	}
}

function getPendingBlock()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if(!isset($_GET["blockNum"]) || intval($_GET["blockNum"]) <= $amountOfBlocks || intval($_GET["blockNum"]) > $amountOfBlocks + $amountOfPendingBlocks)
    {
        echo "ERR: pending block number " . $_GET["blockNum"] . " is missing or bad, canceling connection";
        return "ERROR";
    }
	
	echo file_get_contents($pendingBlockchain . "block" . $_GET["blockNum"] . ".txt");
}

function getBlock()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if(!isset($_GET["blockNum"]) || intval($_GET["blockNum"]) > $amountOfBlocks)
    {
        echo "ERR: completed block number " . $_GET["blockNum"] . " is missing or bad, canceling connection";
        return "ERROR";
    }
	
	echo file_get_contents($blockchain . "block" . $_GET["blockNum"] . ".txt");
}

function getBalance()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if($givenFromAddr != $fromAddress)
    {
        echo "ERR: incorrect wallet address, canceling connection";
        return "ERROR";
    }
	$balance = 0.0;
	foreach($completedBlocks as $block)
	{
		$lines = file($blockchain . $block);
		
		for($x = 3; $x < count($lines); $x++)
		{
			if(explode("->", $lines[$x])[1] == $fromAddress && count(explode("->", $lines[$x])) >= 3)
			{
				$balance -= floatval(explode("->", $lines[$x])[0]);
			}
			else if(explode("->", $lines[$x])[1] == $fromAddress && count(explode("->", $lines[$x])) < 3)
			{
				$balance += floatval(explode("->", $lines[$x])[0]);
			}
			else if(explode("->", $lines[$x])[2] == $fromAddress && count(explode("->", $lines[$x])) >= 3)
			{
				$balance += floatval(explode("->", $lines[$x])[0]);
			}
		}
	}
	
	return $balance;
}

function pendingFunds()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if($givenFromAddr != $fromAddress)
    {
        echo "ERR: incorrect wallet address, canceling connection";
        return "ERROR";
    }
	$balance = 0.0;
	foreach($pendingBlocks as $block)
	{
		$lines = file($pendingBlockchain . $block);
		
		for($x = 3; $x < count($lines); $x++)
		{
			if(explode("->", $lines[$x])[1] == $fromAddress && count(explode("->", $lines[$x])) >= 3)
			{
				$balance -= floatval(explode("->", $lines[$x])[0]);
			}
			else if(explode("->", $lines[$x])[1] == $fromAddress && count(explode("->", $lines[$x])) < 3)
			{
				$balance += floatval(explode("->", $lines[$x])[0]);
			}
			else if(explode("->", $lines[$x])[2] == $fromAddress && count(explode("->", $lines[$x])) >= 3)
			{
				$balance += floatval(explode("->", $lines[$x])[0]);
			}
		}
	}
	
	return $balance;
}
 
function generateWalletAddress()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    $addr = "dcc" . hash("sha256", strval($username . $password));
    return $addr;
}

function initializeNewAddress()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
	$addr = generateWalletAddress();
	createPendingBlock();
	
	$blockFile = $pendingBlockchain . "block" . ($amountOfBlocks + $amountOfPendingBlocks) . ".txt";
	file_put_contents($blockFile, "0->" . $addr.PHP_EOL , FILE_APPEND | LOCK_EX);
	
	echo "initialized: " . $addr;
	return $addr;
}

function uploadProgram()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
	
	if($givenFromAddr != $fromAddress)
    {
        echo "ERR: incorrect wallet address, canceling connection";
        return "ERROR";
    }
	
	$cost = (floatval(trim(file("costPerMinute.txt")[0])) + (floatval($_GET["computationLevel"]) / 10) * floatval(trim(file("costPerMinute.txt")[0]))) * floatval($_GET["minutes"]);
	
	if(pendingFunds() > 0)
	{
		if($cost > (getBalance()))
		{
			echo "ERR: cost is larger than current balance, canceling connection";
			return "ERROR";
		}
	}
	else
	{
		if($cost > (getBalance() + pendingFunds()))
		{
			echo "ERR: cost is larger than current balance, canceling connection";
			return "ERROR";
		}
	}
	
	$uploads_dir = __DIR__ . "/programs";
	if ($_FILES["file"]["error"] == UPLOAD_ERR_OK)
	{
		if(strpos($_FILES["file"]["name"], ".zip"))
		{
			$tmp_name = $_FILES["file"]["tmp_name"];
			$randNum = (time()-1636850000) . round(floatval(explode(" ", microtime())[0]) * 1000);
			$name = $randNum . ".zip";
			move_uploaded_file($tmp_name, $uploads_dir . "/" . $name);
			if(file_exists($uploads_dir . "/" . $name))
			{
				echo "Program successfully uploaded as: " . str_replace(".zip", "", $name);
				file_put_contents($uploads_dir . "/" . $randNum . ".cfg", $name . "\n" . $_GET["minutes"] . "\n" . $_GET["computationLevel"] . "\n" . $_GET["minutes"] . "\n" . strval($cost), LOCK_EX);
				
				mkdir(__DIR__ . "/programs/" . $randNum . "-out");
				
				$blockFile = $pendingBlockchain . "block" . ($amountOfBlocks + $amountOfPendingBlocks) . ".txt";
				file_put_contents($blockFile, $cost . "->" . $fromAddress . "->dcc1f4e25f12ae7460b9f1430a5ed1858384368b70435855570cdacdc0338b708f4".PHP_EOL , FILE_APPEND | LOCK_EX);
			}
			else
				echo "Program failed to upload: " . str_replace(".zip", "", $name);
		}
		else
			echo "Program must be packaged in a .zip";
	}
	else
		echo "Program failed to upload: " . str_replace(".zip", "", $name);
}

function sendToAddress()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
	
	if($amountOfPendingBlocks ==0)
		createPendingBlock();
	
	if($givenFromAddr != $fromAddress)
    {
        echo "ERR: incorrect wallet address, canceling connection";
        return "ERROR";
    }
	if(!isset($recipientAddress) || !isAddressValid($recipientAddress))
    {
        echo "ERR: recipient address is missing or invalid, canceling connection";
        return "ERROR";
    }
	
	$amount = 0.0;
	try
	{
		$amount = floatval($_GET["sendAmount"]);
	}
	catch (Exception $e)
	{
        echo "ERR: sendAmount was not set or is bad, canceling connection";
        return "ERROR";
	}
	if(pendingFunds() > 0)
	{
		if($amount > (getBalance()))
		{
			echo "ERR: sendAmount is larger than current balance, canceling connection";
			return "ERROR";
		}
	}
	else
	{
		if($amount > (getBalance() + pendingFunds()))
		{
			echo "ERR: sendAmount is larger than current balance, canceling connection";
			return "ERROR";
		}
	}
	
	$blockFile = $pendingBlockchain . "block" . ($amountOfBlocks + $amountOfPendingBlocks) . ".txt";
	file_put_contents($blockFile, $amount . "->" . $fromAddress . "->" . $recipientAddress.PHP_EOL , FILE_APPEND | LOCK_EX);
	
	echo "successfully sent ￠" . $amount . " from " .  $fromAddress . " to " . $recipientAddress;
	echo "\n";
	echo "￠" . strval(getBalance() - $amount) . " remaining, " . "￠" . strval(pendingFunds()) . " pending";
}

function isAddressValid($addr)
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
	
	if(strpos($addr, "dcc") != 0)
	{
		return false;
	}
	if(strlen($addr) != 67)
	{
		return false;
	}
	
	return true;
}

function getProgramLifeLeft()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
	
	try
	{
		$timeLeft = trim(file(__DIR__ . "/programs/" . $_GET["programID"] . ".cfg")[3]);
		echo $timeLeft;
	}
	catch (Exception $e)
	{
        echo "ERR: program not found";
        return "ERROR";
	}
	
	return true;
}

function assignProgram()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
	
	$highestLifeNum = 0;
	$highestLifeID = "";
	
	$programs = array_slice(scandir(__DIR__ . "/programs"), 2);
	for ($x = 0; $x < count($programs); $x++)
	{
		if(strpos($programs[$x], ".cfg"))
		{
		$life = floatval(trim(file(__DIR__ . "/programs/" . $programs[$x])[3]));
		
		if($life <= 0)
		{
			unlink(__DIR__ . "/programs/" . $programs[$x]);
			unlink(__DIR__ . "/programs/" . explode(".", $programs[$x])[0] . ".zip");
			unlink(__DIR__ . "/programs/" . explode(".", $programs[$x])[0] . "-out");
			continue;
		}
		
		if($life > $highestLifeNum)
		{
			$highestLifeNum = $life;
			$highestLifeID = explode(".", $programs[$x])[0];
		}
		}
	}
	
	echo $highestLifeID;
	
	return true;
}

function hashProgram()
{
	global $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
	
	$hash = hash_file("sha256", __DIR__ . "/programs/" . $_GET["programID"] . ".zip");
	
	echo $hash;
	
	return true;
}

if($query == "generateWalletAddress")
{
    generateWalletAddress();
}
else if($query == "initializeNewAddress")
{
    initializeNewAddress();
}
else if($query == "getBalance")
{
    echo strval(getBalance());
}
else if($query == "pendingFunds")
{
    echo strval(pendingFunds());
}
else if($query == "getBlock")
{
    getBlock();
}
else if($query == "getPendingBlock")
{
    getPendingBlock();
}
else if($query == "submitBlock")
{
    submitBlock();
}
else if($query == "sendToAddress")
{
    sendToAddress();
}
else if($query == "amountOfCompletedBlocks")
{
    echo $amountOfBlocks;
}
else if($query == "amountOfPendingBlocks")
{
    echo $amountOfPendingBlocks;
}
else if($query == "getDifficulty")
{
    echo trim(file("minelevel.txt")[0]);
}
else if($query == "getCostPerMinute")
{
    echo trim(file("costPerMinute.txt")[0]);
}
else if($query == "uploadProgram")
{
    uploadProgram();
}
else if($query == "getProgramLifeLeft")
{
    getProgramLifeLeft();
}
else if($query == "assignProgram")
{
    assignProgram();
}
else if($query == "hashProgram")
{
    hashProgram();
}
?>
