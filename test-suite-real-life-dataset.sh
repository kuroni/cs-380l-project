#/bin/bash

COMMAND=("\"cp -r\"" "./cpr" "./cpr-new-buffer" "./cpr-no-falloc" "./cpr-single-file")
LOGCOMMAND=("cp-r" "cpr" "cpr-new-buffer" "cpr-no-falloc" "cpr-single-file")

SRC=("~/Documents/Repositories/linux" "~/osu" "~/Videos/Movies")
DEST=("~/linux-1" "~/osu-1" "~/movies-1")
LOGFILE=("linux" "osu" "movies")

for i in $(seq 0 1 4); do
    for j in $(seq 0 1 2); do
        OUTFILE="../${LOGCOMMAND[$i]}-${LOGFILE[$j]}.txt"
        EXECUTE="./test.sh ${COMMAND[$i]} ${SRC[$j]} ${DEST[$j]}"
        echo $LOGCOMMAND $LOGFILE
        bash -c "$EXECUTE" > $OUTFILE 2>&1
    done
done
