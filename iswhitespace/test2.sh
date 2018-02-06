unset i;
for i in {1..257}
  do
    echo $i
    wc "text$i.txt";
  done
