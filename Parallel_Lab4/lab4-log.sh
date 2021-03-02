LOG_FILE_NAME="log.txt"

getSignalType() {
    case "$1" in
        0) echo $(gettext EXIT_PROGRAM);;
        2) echo $(gettext INTERFERENCE_PROGRAM);;
        10) echo $(gettext HEALTH_CHECK);;
        12) echo $(gettext SIGNAL_ACCEPTED);;
        15) echo $(gettext TERMINATED);;
        21) echo $(gettext WRITE_DATE);;
        18) echo $(gettext WRITE_RANDOM_VALUE);;
        20) echo $(gettext STOP_PROGRAM);;
    esac
}

if [[ -z "$1" || -z "$2" ]]; then
    gettext NOT_VALID_LOG_PARAMATERS
    exit 1
fi

signal_name_parameter=$1
process_id_parameter=$2
signal_number=$(kill -l $signal_name_parameter)

echo "$(date) - $process_id_parameter - $signal_number - $signal_name_parameter - $(getSignalType $signal_number)" >> $LOG_FILE_NAME