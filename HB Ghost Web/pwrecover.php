<?php

//This script made by Tamir, All Rights Reserved ©
//Do not copy or take credit for it!
//Please visit www.tfps.tk for more scripts for helbreath.
//For more questions contact me at tfps_@hotmail.com

//edit the line below to your server path. use double slashes
//-----------------------------------------------------------
$dir = 'C:\\HBFun\\Account';
//-----------------------------------------------------------

if(!$_POST)
{
echo '<form action="'.$_SERVER['PHP_SELF'].'" method=post>
        Account: <input type="text" name=acc><br>
		Email Adress: <input type="text" name=mail><br>
		Secret Answer: <input type="password" name=pass><br>
		New Password: <input type="password" name=nwpass><br>
        <input type="submit" value="Password Recover">
        </form>';
}
else{
  $acc = $_POST['acc'];
  $mail = $_POST['mail'];
  $pass = $_POST['pass'];
  $nwpass = $_POST['nwpass'];
  
  if(!@$acc | !@$pass | !@$nwpass| !@$mail)
  die('Please fill in all the fields');
	
  $fl = substr($acc, 0, 1);
  $ascii = ord($fl);
  $filename = $dir . '\\ASCII' . $ascii . '\\' . $acc . '.txt';
  $lines = file($filename);
  $a=0;
   
  foreach($lines as $a){
    $a = trim($a);
    if($a == 'account-Answer   = '.$pass.''){ $passflag=TRUE; }
    elseif($a == 'account-Email    = '.$mail.''){ $mailflag=TRUE; }
    elseif(substr_count($b, 'character-LEVEL') > 0){ $rep = $a; }
  }
  if(!$passflag){ echo '<font color="#FF0000">Incorrect Secret Answer</font>'; }
  if(!$mailflag){ echo '<font color="#FF0000">Incorrect Email address</font>'; }
  else{
    $fl = substr($acc, '0', '1');
    $casc = ord($acc);
    $filenames = $dir . '\\ASCII' . $ascii . '\\' . $acc . '.txt';
    $lines = file($filenames);
    $a=0;
    foreach($lines as $b){
      if(substr_count($b, 'account-password') > 0){ $sline = $a; }
      $a++;
    }
    $lines[$sline] = 'account-password = '.$nwpass.'
';
    $lines = implode('', $lines);
    $fo = fopen("$filename", 'w+');
    fwrite($fo, $lines);
    echo '<font color="#FF0000">Password changed successfuly</font>.<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>';
  }
}
?>