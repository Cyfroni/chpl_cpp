STARTTIME=$(date +%s%3N)
$@
ENDTIME=$(date +%s%3N)
echo "It takes $(($ENDTIME - $STARTTIME)) miliseconds to complete this task..."
