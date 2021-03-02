#!/bin/bash

LOCALIZATION_SCRIPT="./lab4-localization.sh"
CHILD_SCRIPT="./lab4-child_process.sh"
LOG_SCRIPT="./lab4-log.sh"

export CHILD_PROCESS_FILE="child.id"
export PARENT_PROCESS_FILE="parent.id"

source $($LOCALIZATION_SCRIPT)
source gettext

help()
{
    echo $(gettext "GENERAL_INFO")
    echo $(gettext "SEE_DETAILS")
    exit 0
}

helpWihtDetails()
{
    echo $(gettext "GENERAL_INFO")
    echo ""
    echo $(gettext "OPTIONS")
    echo $(gettext "PARAMETER_HELP")
    echo $(gettext "PARAMETER_HELP_DETAILED")
    exit 0
}

runChildProcess() 
{
    childProcess=$(ps -p "$(<$CHILD_PROCESS_FILE)" | grep "$(<$CHILD_PROCESS_FILE)")
    
    if [[ ! -z "$childProcess" ]] ; then
        echo $(gettext CHILD_PROCESS_ALREADY_RUNNING)
    else
        nohup $CHILD_SCRIPT > log 2>&1 & 
        echo $! > $CHILD_PROCESS_FILE
        echo $(gettext CHILD_PROCESS_STARTED)
    fi
}

sendSignal() 
{
    if [ -z "$1" ]; then
        echo $(getMessage NO_SIGNAL_ERROR) 1>&2
        exit 1
    fi

    signalId=$1
    childProcess=$(ps -p "$(<$CHILD_PROCESS_FILE)" | grep "$(<$CHILD_PROCESS_FILE)")

    if [[ ! -f "$CHILD_PROCESS_FILE" || -z "$childProcess" ]] ; then
        runChildProcess
    fi

    kill -s $signalId "$(<$CHILD_PROCESS_FILE)"
}

ping()
{
    while true
    do
        if [[ -z "$(ps -p "$(<$PARENT_ID_FILE)" | grep "$(<$PARENT_ID_FILE)")" ]]; then
            return
        fi
    	
        childProcess=$(ps -p "$(<$CHILD_ID_FILE)" | grep "$(<$CHILD_ID_FILE)")
        if [[ -f "$CHILD_ID_FILE" && ! -z "$childProcess" ]] ; then
            kill -s SIGUSR1 "$(<$CHILD_ID_FILE)"
        fi

        sleep 5
    done
}

killChildProcess() 
{
    childProcess=$(ps -p "$(<$CHILD_PROCESS_FILE)" | grep "$(<$CHILD_PROCESS_FILE)")

    if [[ -f "$CHILD_PROCESS_FILE" && ! -z "$childProcess" ]] ; then
        kill -15 "$(<$CHILD_PROCESS_FILE)"
        echo $(gettext CHILD_PROCESS_TERMINATED)
    else
        echo $(gettext CHILD_PROCESS_DOWN)
    fi
}

handleSignal() 
{
    if [ -z "$1" ]; then
        echo $(gettext NO_SIGNAL_TO_PROCESS_ERROR) 1>&2
        exit 1
    fi

    processId=$$
    signalId="$1"

    $LOG_SCRIPT $signalId $processId

    case "$signalId" in
        EXIT) echo $(gettext EXIT_PARENT_PROCESS) | logger;;
        INT)
            echo $(gettext INTERRUPTED_PARENT_PROCESS) | logger
            exit 0;;
        TERM)
            echo $(gettext TERMINATED_PARENT_PROCESS) | logger
            exit 0;;
        TSTP)
            echo $(gettext STOPPED_PARENT_PROCESS) | logger
            kill -19 $$;;
    esac
}

printMenu()
{
    printf "$(gettext SELECT_COMMAND)\n"
    printf "$(gettext DATE_COMMAND)\n"
    printf "$(gettext RANDON_NUMBER_COMMAND)\n"
    printf "$(gettext KILL_COMMAND)\n"
    printf "$(gettext START_COMMAND)\n"
    printf "$(gettext EXIT_COMMAND)\n"
    echo ""
}

processCommand()
{
    command=$1

    case $command in
        "date") sendSignal TTIN;;
        "random") sendSignal CONT;;
        "kill") killChildProcess;;
        "start") startChildProcess;;
        "exit") exit 0;;
        *) echo $(gettext UNKNOWN_COMMAND) 1>&2;;
    esac
}

args=()
argsNumber=0

for arg in $@; do
    ((argsNumber++))
    args+=( $arg )
done

for (( i=0; i < $argsNumber; i++ )); do
    case ${args[$i]} in
        "-h" ) help;;
        "--help" ) helpWihtDetails;;
        * ) echo $(gettext NOT_VALID_OPTION) ${args[$index]}. $(gettext HELP) 1>&2; exit 1;;
    esac
done

echo $(gettext FATHER_PROCESS_RUNNING) | logger

processId=$$
signals=(INT TERM EXIT USR1 USR2 TSTP)

for signal in "${signals[@]}"; do
    trap "handleSignal $signal" "$signal"
done

echo $processId > $PARENT_PROCESS_FILE

runChildProcess
ping &

while true
do
    printMenu
    read command
    processCommand $command
done