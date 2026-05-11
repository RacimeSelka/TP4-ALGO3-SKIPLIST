#!/bin/bash

ko='\e[00;31m';
wipe='\e[00m';
ok='\e[01;32m';
warn='\e[01;33m';
warnuntil=3;


COMMAND="$1"
BASE=.
TESTFILES=../Test/test_files
RET=
DIFF_LINES=
SHOULDBE_LINES=

function display_errors {
	echo "$RET errors : "
	for i in $(seq 0 1 $(expr $RET - 1))
	do
		printf "  Got      --> %s\n" "${DIFF_LINES[$i]}"
		printf "  Expected --> %s\n" "${SHOULDBE_LINES[$i]}"
	done
}
function compute_diff {
	# get the differing lines into an array
	readarray -t DIFF_LINES < <(diff "$1" "$2" | grep '<')
	readarray -t SHOULDBE_LINES < <(diff "$1" "$2" | grep '>')
	GOT=${#DIFF_LINES[*]}
	EXPECTED=${#SHOULDBE_LINES[*]}

	if [[ "$GOT" == $EXPECTED ]]; then
	RET=${EXPECTED}
	else
	RET=$(expr $warnuntil + 1)
	fi
}

function test_construction {
    if [ -x $BASE/$COMMAND ]
    then
    rm -f $TESTFILES/result_construct_$1.txt
#    echo "Running " $BASE/$COMMAND -c $1
	$BASE/$COMMAND -c $1 > $TESTFILES/result_construct_$1.txt
	compute_diff "$TESTFILES/result_construct_$1.txt" "$TESTFILES/references/result_construct_$1.txt"
	rm -f $TESTFILES/result_construct_$1.txt
    else
	echo "Command $BASE/$COMMAND not found"
	RET=$warnuntil
    fi
}

function test_search {
    if [ -x $BASE/$COMMAND ]
    then
    rm -f $TESTFILES/result_search_$1.txt
#    echo "Running " $BASE/$COMMAND -c $1
	$BASE/$COMMAND -s $1 > $TESTFILES/result_search_$1.txt 2>/dev/null
	compute_diff "$TESTFILES/result_search_$1.txt" "$TESTFILES/references/result_search_$1.txt"
	rm -f $TESTFILES/result_search_$1.txt
    else
	echo "Command $BASE/$COMMAND not found"
	RET=$warnuntil
    fi
}

function test_iterator {
    if [ -x $BASE/$COMMAND ]
    then
    rm -f $TESTFILES/result_iterate_$1.txt
#    echo "Running " $BASE/$COMMAND -c $1
	$BASE/$COMMAND -i $1 > $TESTFILES/result_iterator_$1.txt  2>/dev/null
	compute_diff "$TESTFILES/result_iterator_$1.txt" "$TESTFILES/references/result_iterator_$1.txt"
	rm -f $TESTFILES/result_iterator_$1.txt
    else
	echo "Command $BASE/$COMMAND not found"
	RET=$warnuntil
    fi
}

function test_remove {
    if [ -x $BASE/$COMMAND ]
    then
    rm -f $TESTFILES/result_remove_$1.txt
#    echo "Running " $BASE/$COMMAND -c $1
	$BASE/$COMMAND -r $1 > $TESTFILES/result_remove_$1.txt  2>/dev/null
	compute_diff "$TESTFILES/result_remove_$1.txt" "$TESTFILES/references/result_remove_$1.txt"
	rm -f $TESTFILES/result_remove_$1.txt
    else
	echo "Command $BASE/$COMMAND not found"
	RET=$warnuntil
    fi
}


function runtest {
 for i in $(seq 1 1 $2)
 do
	test_$1 $i
	[ $RET -eq 0 ] && printf "%-12s [${ok}OK${wipe}]\n" "$1 ($i)" 
	[ $RET -gt 0 -a $RET -le $warnuntil ] && printf "%-12s [${warn}OK${wipe}]\n" "$1 ($i)" && display_errors
	[ $RET -gt $warnuntil ] && printf "%-12s [${ko}KO${wipe}]\n" "$1 ($i)" 
 done
}

echo Executing $COMMAND in directory $PWD

runtest construction 4;
runtest search 4;
runtest iterator 4;
runtest remove 4;
exit 0
