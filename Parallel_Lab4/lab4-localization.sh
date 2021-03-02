#!/bin/bash

function require {
    local utilName=$1
    $(command -v $utilName 1>&2 2>/dev/null)
    return $? 
}

function getFileLocale {
    echo "$(basename $1 | awk -F "[-.]" '{print $2}')"
}

function getTaskName {
    echo "$(basename $1 | awk -F "-" '{print $1}')"
}

function checkDir {
    path=$1
    if ! [[ -d $path ]]; then
        mkdir -p $path 1>&2 2>/dev/null || exit 1;
    fi
}

function setLocalization {
    `require msgfmt` && `require gettext` || exit 1;

    local localizationPath="/usr/share/locale"
    {
        result=$(checkDir "$localizationPath") && [ -w $localizationPath ]
    } && {
        export TEXTDOMAINDIR=$localizationPath
    } || {
        localizationPath="$HOME$localizationPath"
        result=$(checkDir "$localizationPath") && [ -w $localizationPath ] && {
            export TEXTDOMAINDIR=$localizationPath
        } || {
            exit 1
        }
    }

    local localizationFilePattern="[a-zA-Z0-9]\+\-[a-z]\{2\}\_[A-Z]\{2\}\.po"
    find $(pwd)/localization -type f | grep $localizationFilePattern | while read line; do
        locale=$(getFileLocale $line)
        result=$(checkDir "$localizationPath/${locale:0:2}/LC_MESSAGES") &&
        {
            taskName=$(getTaskName $line)
            msgfmt -o "$(dirname $line)/$taskName.mo" $line
            mv "$(dirname $line)/$taskName.mo" "$localizationPath/${locale:0:2}/LC_MESSAGES" 
        } || 
        {
            exit 1;
        }
    done
}

setLocalization