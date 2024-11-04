 #!/bin/sh

 read -r idle1 total1 <<<$(awk 'NR==1 {for(i=2; i<=NF;i++)j+=$i; print $5, j}' /proc/stat)

 sleep 2

 while true
 do

     read -a stat <<<$(head -1 /proc/stat)

     idle2=${stat[4]}
     total2=0

     for i in ${stat[@]/cpu}; do
     let total2+=$i
     done

     time=$(bc -l <<<"(1 - ($idle2 - $idle1) / ($total2 - $total1))*100")

     [ ${time%.*} -ge 50  ] || break

     idle1=$idle2
     total1=$total2

     sleep 300

 done

 curl -d 'Sefirot is available' $(</home/abfemat/.ntfy)
