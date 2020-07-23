<?php

//This script made by Tamir, All Rights Reserved ©
//Do not copy or take credit for it!
//Please visit www.tfps.tk for more scripts for helbreath.
//For more questions contact me at tfps_@hotmail.com

//edit the line below to your server path. use double slashes
//-----------------------------------------------------------
$dir = 'C:\\HBFun';
//-----------------------------------------------------------

if(!$_POST)
{
        echo '<form action="'.$_SERVER['PHP_SELF'].'" method=post>
        Account: <input type="text" name=acc><br>
        Password: <input type="password" name=pass><br>
        Character: <input type="text" name=char><br>
        <input type="submit" value="Unstuck">
        </form>';
}
else
{
        $acc = $_POST['acc'];
        $pass = $_POST['pass'];
        $char = $_POST['char'];
        if(!@$acc | !@$pass | !@$char)
        die('Please fill in all the fields');

        $a_ord = ord($acc);
        $c_ord = ord($char);
        $path = ($dir==''?'':$dir.'\\').'Account\\AscII'.$a_ord.'\\'.$acc.'.txt';
        $char_path = ($dir==''?'':$dir.'\\').'Character\\AscII'.$c_ord.'\\'.$char.'.txt';

        if(!file_exists($path))
        die('Account <b>'.$acc.'</b> is not exists');
        elseif(!file_exists($char_path))
        die('Character <b>'.$char.'</b> is not exists');

        $file = file($path);
        for($a=0; $a<count($file); $a++)
        {
                if(preg_match('/^account-password = '.$pass.'$/', trim($file[$a])))
                $pass_check = true;
                if(preg_match('/^account-character-HBF5  = '.$char.'$/', trim($file[$a])))
                $char_check = true;
        }

        if(!@$pass_check)
        die('The password is not correct.');
        elseif(!@$char_check)
        die('The character doesn\'t exists in this account');

        $file = file($char_path);

        for($i=0; $i<count($file); $i++)
        {
                if(preg_match('/character-loc-map = /', $file[$i]))
                $line = $i;
                elseif(preg_match('/character-location   = /', $file[$i]))
                $loc = $i;
                elseif(preg_match('/character-loc-x   = /', $file[$i]))
                $loc_x = $i;
                elseif(preg_match('/character-loc-y   = /', $file[$i]))
                $loc_y = $i;
        }
        $loc = str_replace('character-location   = ', '', $file[$loc]);
        if(trim($loc) == 'aresden')
        $loc = 'aresden';
        elseif(trim($loc) == 'elvine')
        $loc = 'elvine';
        else
        $loc = 'default';

        $file[$line] = "character-loc-map = ".$loc."\r\n";
        $file[$loc_x] = "character-loc-x   = -1\r\n";
        $file[$loc_y] = "character-loc-y   = -1\r\n";

        $file = join('', $file);

        $fopen = fopen($char_path, 'w');
        fwrite($fopen, $file);
        fclose($fopen);

        echo 'Your character should now work! Have FUN!';
}


?>