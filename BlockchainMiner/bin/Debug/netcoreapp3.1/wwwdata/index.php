<?php
$query = $_GET['query'];

$blockchain = __DIR__ . '\\blockchain\\';
$pendingBlockchain = __DIR__ . '\\pendingblocks\\';

$username = $_GET['username'];
$password = $_GET['password'];

$recipientAddress = $_GET['recipientAddress'];
$givenFromAddr = $_GET['fromAddress'];
$fromAddress = generateWalletAddress();

$completedBlocks = array_slice(scandir($blockchain), 2);
$pendingBlocks = array_slice(scandir($pendingBlockchain), 2);

$amountOfBlocks = count($completedBlocks);
$amountOfPendingBlocks = count($pendingBlocks);

$currentTime = time();

function submitBlock()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if(!isset($_GET['blockNum']) || !isset($_GET['programID']) || intval($_GET['blockNum']) <= $amountOfBlocks || intval($_GET['blockNum']) > $amountOfBlocks + $amountOfPendingBlocks)
    {
        echo "ERR: block number is missing or bad, canceling connection";
        return "ERROR";
    }

        $transactions = ReadVariableFromJSON($pendingBlockchain . "block" . $_GET['blockNum'] . ".dccblock", "transactions");
        $lastHash = ReadVariableFromJSON($blockchain . "block" . strval($amountOfBlocks) . ".dccblock", "hash");

        $data = strval($lastHash);
        for($x = 0; $x < count($transactions); $x++)
        {
                $data .= $transactions[$x];
        }
        $data .= $_GET['nonce'];
        $data = str_replace(" ", "", str_replace("\n", "", $data));
        $hashedData = hash("sha256", $data);

        if($hashedData != $_GET['minedHash'])
        {
                echo "ERR: block mined incorrectly as " . $hashedData . ", canceling connection";
                return "ERROR";
        }
        if(strpos($lastHash, trim(file("minelevel.txt")[0])) && strpos($lastHash, trim(file("minelevel.txt")[0])) != 0)
        {
                echo "ERR: mined block has not used the correct mining level, canceling connection";
                return "ERROR";
        }

        if($currentTime - intval(trim(file("minelevel.txt")[1])) > 30)
        {
                $strng = trim(file("minelevel.txt")[0]);
                if(strlen($strng) > 3)
                {
                        $strng = mb_substr($strng, 0, -1) . "\n";
                        $strng .= $currentTime;
                        file_put_contents("minelevel.txt", $strng);
                }
                else if(strlen($strng) < 6)
                {
                        $strng = trim(file("minelevel.txt")[0]) . "0/n";
                        $strng .= $currentTime;
                        file_put_contents("minelevel.txt", $strng);
                }
        }
        else if(strlen($strng) < 6)
        {
                $strng = trim(file("minelevel.txt")[0]) . "0/n";
                $strng .= $currentTime;
                file_put_contents("minelevel.txt", $strng);
        }

        $blockFile = $blockchain . "block" . $_GET['blockNum'] . ".dccblock";

        // Create json data and write it to file
        $blank_array = [];
    $data = array("Version"=>$_GET['Version'], "lastHash"=>trim($lastHash), "hash"=>trim($_GET['minedHash']), "nonce"=>trim($_GET['nonce']), "time"=>$currentTime, "transactions"=>ReadVariableFromJSON($pendingBlockchain . "block" . $_GET['blockNum'] . ".dccblock", "transactions"), "transactionTimes"=>ReadVariableFromJSON($pendingBlockchain . "block" . $_GET['blockNum'] . ".dccblock", "transactionTimes"));
    file_put_contents($blockFile, json_encode($data), LOCK_EX);

        unlink($pendingBlockchain . "block" . $_GET['blockNum'] . ".dccblock");
        $amountOfBlocks++;
        $amountOfPendingBlocks--;
        CreatePendingBlock();
        CreateTransaction(1, $givenFromAddr, "");
        echo "successfully submitted block";

        $outFiles = array_slice(scandir('.\\programs\\' . $_GET['programID'] . "-out"), 2);
        $uploads_dir = '.\\programs\\' . $_GET['programID'] . "-out";

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
                        move_uploaded_file($tmp_name, $uploads_dir . "\\" . $name);
                        if(file_exists($uploads_dir . "\\" . $name))
                        {
                                echo PHP_EOL . "Program data successfully uploaded as: " . str_replace(".txt", "", $name);

                                $cost = ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "Cost") * ($_GET['time'] / 60.0 / ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "MinutesLeft"));

                                // Create json data and write it to file
                                $data = array("Zip"=>$_GET['programID'] . ".zip", "TotalMinutes"=>ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "TotalMinutes"), "Author"=>ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "Author"), "MinutesLeft"=>ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "MinutesLeft") - floatval($_GET['time']) / 60.0, "ComputationLevel"=>ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "ComputationLevel"), "Cost"=>ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "Cost"), "Built"=>FALSE);
                                file_put_contents('.\\programs\\' . $_GET['programID'] . ".cfg", json_encode($data), LOCK_EX);

                                CreateTransaction($cost, "dcc1f4e25f12ae7460b9f1430a5ed1858384368b70435855570cdacdc0338b708f4", $givenFromAddr);
                        }
                        else
                                echo PHP_EOL . "Program data failed to upload: " . str_replace(".txt", "", $name);
        }
        else
                echo PHP_EOL . "Program data failed to upload: " . str_replace(".txt", "", $name);
}

function CreatePendingBlock()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if($amountOfPendingBlocks < 1)
        {
                // Create json data and write it to file
                $blank_array = [];
                $data = array("Version"=>$_GET['Version'], "lastHash"=>ReadVariableFromJSON($blockchain . "block" . ($amountOfBlocks) . ".dccblock", "hash"), "hash"=>"0000000000000000000000000000000000000000000000000000000000000000", "nonce"=>"0", "time"=>"0", "transactions"=>$blank_array, "transactionTimes"=>$blank_array);
                file_put_contents($pendingBlockchain . "block" . ($amountOfBlocks+1) . ".dccblock", json_encode($data), FILE_APPEND | LOCK_EX);

                echo "created new block: " . ($amountOfBlocks + 1) . "\n";
                $amountOfPendingBlocks++;
        }
}

function CreateTransaction($amount, $from, $to)
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $transactionTimesArray = ReadVariableFromJSON($pendingBlockchain . "block" . ($amountOfBlocks+1) . ".dccblock", "transactionTimes");
        $transactionArray = ReadVariableFromJSON($pendingBlockchain . "block" . ($amountOfBlocks+1) . ".dccblock", "transactions");
        if($to != "")
                array_push($transactionArray, $amount . "->" . $from . "->" . $to);
        else
                array_push($transactionArray, $amount . "->" . $from);

        array_push($transactionTimesArray, $currentTime);

        // Create json data and write it to file
        $data = array("Version"=>$_GET['Version'], "lastHash"=>ReadVariableFromJSON($pendingBlockchain . "block" . ($amountOfBlocks+1) . ".dccblock", "lastHash"), "hash"=>ReadVariableFromJSON($pendingBlockchain . "block" . ($amountOfBlocks+1) . ".dccblock", "hash"), "nonce"=>"0", "time"=>"0", "transactions"=>$transactionArray, "transactionTimes"=>$transactionTimesArray);
        file_put_contents($pendingBlockchain . "block" . ($amountOfBlocks+1) . ".dccblock", json_encode($data), LOCK_EX);
}

function ReadVariableFromJSON($fileLocation, $variableName)
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $readfile = file_get_contents($fileLocation);
        $readfile = stripslashes($readfile);
        $arr = json_decode($readfile, true);
        fclose($readfile);

        return $arr[$variableName];
}

function getPendingBlock()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if(!isset($_GET['blockNum']) || intval($_GET['blockNum']) <= $amountOfBlocks || intval($_GET['blockNum']) > $amountOfBlocks + $amountOfPendingBlocks)
    {
        echo "ERR: pending block number " . $_GET['blockNum'] . " is missing or bad, canceling connection";
        return "ERROR";
    }

        echo file_get_contents($pendingBlockchain . "block" . $_GET['blockNum'] . ".dccblock");
}

function getBlock()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    if(!isset($_GET['blockNum']) || intval($_GET['blockNum']) > $amountOfBlocks)
    {
        echo "ERR: completed block number " . $_GET['blockNum'] . " is missing or bad, canceling connection";
        return "ERROR";
    }

        echo file_get_contents($blockchain . "block" . $_GET['blockNum'] . ".dccblock");
}

function getBalance()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $balance = 0.0;
        foreach($completedBlocks as $block)
        {
                $transactions = ReadVariableFromJSON($blockchain . $block, "transactions");

                for($x = 0; $x < count($transactions); $x++)
                {
                        if(explode('&', explode('->', trim($transactions[$x]))[1])[0] == $givenFromAddr && explode('&', explode('->', trim($transactions[$x]))[2])[0] != $givenFromAddr && count(explode('->', trim($transactions[$x]))) >= 3)
                        {
                                $balance -= floatval(explode('->', trim($transactions[$x]))[0]);
                        }
                        else if(explode('&', explode('->', trim($transactions[$x]))[1])[0] == $givenFromAddr && count(explode('->', trim($transactions[$x]))) < 3)
                        {
                                $balance += floatval(explode('->', trim($transactions[$x]))[0]);
                        }
                        else if(explode('&', explode('->', trim($transactions[$x]))[2])[0] == $givenFromAddr && explode('&', explode('->', trim($transactions[$x]))[1])[0] != $givenFromAddr && count(explode('->', trim($transactions[$x]))) >= 3)
                        {
                                $balance += floatval(explode('->', trim($transactions[$x]))[0]);
                        }
                }
        }

        return $balance;
}

function pendingFunds()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $balance = 0.0;
        foreach($pendingBlocks as $block)
        {
                $transactions = ReadVariableFromJSON($pendingBlockchain . $block, "transactions");

                for($x = 0; $x < count($transactions); $x++)
                {
                        if(explode('&', explode('->', trim($transactions[$x]))[1])[0] == $fromAddress && explode('&', explode('->', trim($transactions[$x]))[2])[0] != $givenFromAddr && count(explode('->', trim($transactions[$x]))) >= 3)
                        {
                                $balance -= floatval(explode('->', trim($transactions[$x]))[0]);
                        }
                        else if(explode('&', explode('->', trim($transactions[$x]))[1])[0] == $fromAddress && count(explode('->', trim($transactions[$x]))) < 3)
                        {
                                $balance += floatval(explode('->', trim($transactions[$x]))[0]);
                        }
                        else if(explode('&', explode('->', trim($transactions[$x]))[2])[0] == $fromAddress && explode('&', explode('->', trim($transactions[$x]))[1])[0] != $givenFromAddr && count(explode('->', trim($transactions[$x]))) >= 3)
                        {
                                $balance += floatval(explode('->', trim($transactions[$x]))[0]);
                        }
                }
        }

        return $balance;
}

function generateWalletAddress()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
    $addr = "dcc" . hash("sha256", strval($username . $password));
    return $addr;
}

function initializeNewAddress()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;
        $addr = generateWalletAddress();
        CreatePendingBlock();

        CreateTransaction(0, $addr, "");

        echo "initialized: " . $addr;
        return $addr;
}

function uploadProgram()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        if($givenFromAddr != $fromAddress)
    {
        echo "ERR: incorrect wallet address, canceling connection";
        return "ERROR";
    }

        $cost = (floatval(trim(file("costPerMinute.txt")[0])) + (floatval($_GET['computationLevel']) / 10) * floatval(trim(file("costPerMinute.txt")[0]))) * floatval($_GET['minutes']);

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

        $uploads_dir = '.\\programs';
        if ($_FILES["file"]["error"] == UPLOAD_ERR_OK)
        {
                if(strpos($_FILES["file"]["name"], ".zip"))
                {
                        $tmp_name = $_FILES["file"]["tmp_name"];
                        $randNum = ($currentTime-1636850000) . round(floatval(explode(" ", microtime())[0]) * 1000);
                        $name = $randNum . ".zip";
                        move_uploaded_file($tmp_name, $uploads_dir . "\\" . $name);
                        if(file_exists($uploads_dir . "\\" . $name))
                        {
                                echo "Program successfully uploaded as: " . str_replace(".zip", "", $name);

                                // Create json data and write it to file
                                $data = array("Zip"=>$name, "TotalMinutes"=>$_GET['minutes'], "Author"=>$fromAddress, "MinutesLeft"=>$_GET['minutes'], "ComputationLevel"=>$_GET['computationLevel'], "Cost"=>$cost, "Built"=>FALSE);
                                file_put_contents($uploads_dir . "\\" . $randNum . ".cfg", json_encode($data), LOCK_EX);

                                mkdir(".\\programs\\" . $randNum . "-out");

                                CreateTransaction($cost, $fromAddress, "dcc1f4e25f12ae7460b9f1430a5ed1858384368b70435855570cdacdc0338b708f4");
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
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        if($amountOfPendingBlocks ==0)
                CreatePendingBlock();

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
                $amount = floatval($_GET['sendAmount']);
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

        CreateTransaction($amount, $fromAddress, $recipientAddress);

        echo "successfully sent ￠" . $amount . " from " .  $fromAddress . " to " . $recipientAddress;
        echo "\n";
        echo "￠" . strval(getBalance() - $amount) . " remaining, " . "￠" . strval(pendingFunds()) . " pending";
}

function isAddressValid($addr)
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

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
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        try
        {
                $timeLeft = ReadVariableFromJSON('.\\programs\\' . $_GET['programID'] . ".cfg", "MinutesLeft");
                echo $timeLeft;
                return $timeLeft;
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
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $highestLifeNum = 0;
        $highestLifeID = "";

        $programs = array_slice(scandir(".\\programs"), 2);
        for ($x = 0; $x < count($programs); $x++)
        {
                if(strpos($programs[$x], ".cfg"))
                {
                        $life = floatval(ReadVariableFromJSON(".\\programs\\" . $programs[$x], "MinutesLeft"));

                        if($life <= 0)
                        {
                                unlink(".\\programs\\" . $programs[$x]);
                                unlink(".\\programs\\" . explode(".", $programs[$x])[0] . ".zip");
                                unlink(".\\programs\\" . explode(".", $programs[$x])[0] . "-out");
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

function getAllPrograms()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $outStr = "";

        $programs = array_slice(scandir(".\\programs"), 2);
        for ($x = 0; $x < count($programs); $x++)
        {
                if(strpos($programs[$x], ".cfg"))
                {
                        $outStr .= explode(".", $programs[$x])[0] . "\n";
                }
        }

        echo $outStr;

        return true;
}

function getProgramOutputDataLength()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $dir = '.\\programs\\' . $_GET['programID'] . "-out\\";

        $programData = array_slice(scandir($dir), 2);
        echo count($programData);

        return true;
}

function hashProgram()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        $hash = hash_file("sha256", ".\\programs\\" . $_GET['programID'] . ".zip");

        echo $hash;

        return true;
}

function getInfo()
{
        global $currentTime, $blockchain, $pendingBlockchain, $username, $password, $recipientAddress, $givenFromAddr, $fromAddress, $completedBlocks, $pendingBlocks, $amountOfBlocks, $amountOfPendingBlocks;

        // Create json data and print it
        $data = array("Address"=>$givenFromAddr, "Balance"=>0, "PendingBalance"=>pendingFunds(), "BlockchainLength"=>$amountOfBlocks, "PendingLength"=>$amountOfPendingBlocks, "MineDifficulty"=>trim(file("minelevel.txt")[0]), "CostPerMinute"=>trim(file("costPerMinute.txt")[0]));

        echo json_encode($data);
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
else if($query == "getInfo")
{
    getInfo();
}
else if($query == "getAllPrograms")
{
    getAllPrograms();
}
else if($query == "getProgramOutputDataLength")
{
    getProgramOutputDataLength();
}
?>