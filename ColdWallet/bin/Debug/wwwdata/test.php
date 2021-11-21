<?php
// Convert $argv into associative array
function parse_argv(array $argv): array
{
    $request = [];
    foreach ($argv as $i => $a) {
        if (!$i) {
            continue;
        }

        if (preg_match('/^-*(.+?)=(.+)$/', $a, $matches)) {
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

$query = $_GET['query'];

echo $query;
return;
?>
