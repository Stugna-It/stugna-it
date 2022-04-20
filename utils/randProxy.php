<?php


start(1000);


function saveToFile($prx,$filename='proxy.txt') {
    $fp = fopen($filename, 'a');
    foreach ($prx as $proxy) {
        fwrite($fp, $proxy . PHP_EOL);
    }
    fclose($fp);
    echo count($prx) . " proxies save to {$filename}". PHP_EOL;
}

function start($threads,$timeout = 5) {
    $multi 	= curl_multi_init();
    $checked = 0;
    $found = 0;
    $res = [];
    $ip = [];
    $prx = [];
    $start = time();
    while (true)   {
        for($i=0;$i < $threads; $i++) {
            $ip[$i] = randProxy();
            $curl[$i] = curl_init();
            curl_setopt($curl[$i],CURLOPT_RETURNTRANSFER,1);
            curl_setopt($curl[$i],CURLOPT_URL,"http://213.183.56.193/");
            curl_setopt($curl[$i],CURLOPT_PROXY, $ip[$i]);
            curl_setopt($curl[$i],CURLOPT_TIMEOUT,$timeout);
            curl_multi_add_handle($multi,$curl[$i]);
            $checked++;
        }

        do {
            curl_multi_exec($multi,$active);
            sleep(1);
        }   while   ( $active > 0 );

        foreach($curl as $cid => $cend)         {
            $con[$cid] = curl_multi_getcontent($cend);
            curl_multi_remove_handle($multi,$cend);
            if(strpos($con[$cid],'Hello !') > 0) {
                $res[] = $ip[$cid];
                echo "{$checked} " . $ip[$cid] . PHP_EOL;
                $prx[] = $ip[$cid];
                $found++;
            }
        }
        $elapsed = time() - $start;
        $pps = round($checked / $elapsed);
        echo "checked {$checked} found {$found} pps {$pps}" . PHP_EOL;
        if (($elapsed % 600) == 0) {
            if (count($prx)>0) {
                saveToFile($prx);
                $prx = [];
            }
        }
    }
    return $res;
}


function randProxy($ports = [80, 8080, 3128, 8081, 1080, 999, 9080, 8085, 8888, 9002, 8089]) {
    while (1) {
        $ip = long2ip(rand(0, 4294967295));
        if (filter_var($ip, FILTER_VALIDATE_IP, FILTER_FLAG_NO_PRIV_RANGE | FILTER_FLAG_NO_RES_RANGE)) {
            return 'http://' . $ip . ':' . $ports[rand(0,count($ports)-1)];
        }
    }
}
