#!/bin/bash

SYSTEM_SIGNAL_LOG_SCRIPT="./lab4-log.sh"
PARENT_PROCESS_FILE="parent.id"
OUTPUT_FILE="output.txt"

onSignalHandled()
{
    if [ -z "$1" ]; then
        return 1
    fi

    signalId=$1

    case "$signalId" in
        EXIT) echo $(gettext EXIT_CHILD_PROCESS) | logger;;
        TTIN) echo $(date) >> $OUTPUT_FILE;;
        CONT) echo $RANDOM >> $OUTPUT_FILE;;
        INT) echo $(gettext INTERRUPTED_CHILD_PROCESS) | logger;;
        TERM) 
            echo $(gettext TERMINATED_CHILD_PROCESS) | logger
            exit 0;;
        TSTP) 
            echo $(gettext STOPPED_CHILD_PROCESS) | logger
            kill -19 $processId;;
    esac
}

sendSignal() 
{
    if [ -z "$1" ]; then
        echo $(gettext NO_SIGNAL_ERROR) 1>&2;;
        exit 1
    fi

    parentProcess=$(ps -p "$(<$PARENT_PROCESS_FILE)" | grep "$(<$PARENT_PROCESS_FILE)")
    if [[ -f "$PARENT_PROCESS_FILE" && ! -z "$parentProcess" ]] ; then
        kill -$1 "$(<$PARENT_PROCESS_FILE)"
    fi
}

handleSignal() 
{
    if [ -z "$1" ]; then
        echo $(gettext UNKNOWN_ERROR) 1>&2;;
        exit 1
    fi

    processId=$$
    signalId=$1

    $SYSTEM_SIGNAL_LOG_SCRIPT $signalId $processId
    sendSignal USR2
    onSignalHandled $signalId
}

echo $(gettext CHILD_PROCESS_RUNNING) | logger

signals=(INT TERM EXIT TTIN CONT USR1 USR2 TSTP)
for signal in "${signals[@]}"; do
    trap "handleSignal $signal" "$signal"
done

while true
do
    echo "$(date): $(gettext CHILD_PROCESS_ALIVE)" > log
    sleep 1
done