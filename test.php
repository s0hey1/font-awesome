<?php

function utf8_to_unicode($str) {
	$unicode = array();		
	$values = array();
	$lookingFor = 1;
	for ($i = 0; $i < strlen($str); $i++) {
		$thisValue = ord($str[$i]);
		if ($thisValue < 128){
			$unicode[] = $thisValue;
		}else{
			if (count($values) == 0){
			 $lookingFor = ($thisValue < 224) ? 2 : 3;
			}
			$values[] = $thisValue;
			if (count($values) == $lookingFor) {
				$number = ($lookingFor == 3) ?
					(($values[0] % 16) * 4096) + (($values[1] % 64) * 64) + ($values[2] % 64):
					(($values[0] % 32) * 64) + ($values[1] % 64);
				$unicode[] = $number;
				$values = array();
				$lookingFor = 1;
			} // if
		} // if
	} // for
	return $unicode;
}

mb_internal_encoding("UTF-8");
$pipes = array();
$cmd = "./font-awesome -v --text-color f0f000FF --as-codepoints -f testfonts/1.ttf >output.png";
$desc = array(
	array("pipe", "rb"),
	array("pipe", "wb"),
	array("pipe", "w"),
);
$text = trim(file_get_contents("unicode_sample.txt"));
$uctext = utf8_to_unicode($text);
foreach($uctext as $v){
	var_dump($v);
	var_dump(dechex($v));
}
$text = implode("\n", $uctext);
var_dump($text);
$process = proc_open($cmd, $desc, $pipes);
if (is_resource($process)){
	fwrite($pipes[0], $text);
	fclose($pipes[0]);

	$return = stream_get_contents($pipes[1]);
	fclose($pipes[1]);
	$error = stream_get_contents($pipes[2]);
	fclose($pipes[2]);
	proc_close($process);
	echo "------\n";
	var_dump($return);
	var_dump($error);
}
