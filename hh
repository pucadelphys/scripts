#!/bin/sh
# #cluster central
# 'central'
# '/home/alex/.ssh/cluster_pass'
# #cluster enseñanza
# "alumno7@10.0.15.11"
# # cluster keter
# "sefirot"
# home/alex/.ssh/sefi_pass'

usage () {
    cat << EOF
Connect to the INMEGEN servers

Usage: hh [OPTION]

    Options:
        -e
            Connect to server from an external network
        -m
            Mount remote home to ~/mnt/cluster
        -h
            Print usage
        -t
            Connect to teaching server instead
        -u
            Unmount remote home
        -s
            Connect to sefirot
EOF
    exit ${1}
}

TYPE='ssh'

while getopts "tcshmue" ARG; do
    case "${ARG}" in
        t)
            [ -n "${CONNECTION}" ] && echo 'Two servers selected' && exit 1
            CONNECTION="alumno";;
        s)
            [ -n "${CONNECTION}" ] && echo 'Two servers selected' && exit 1
            CONNECTION="central";;
        h)
            usage 0 ;;
        m)
            TYPE='mount';;
        u)
            TYPE='umount';;
        c)
            sshpass -f "/home/alex/.ssh/sefi_pass" scp ~/.cache/wal/colors.sh sefirot:~/.cache/wal
            exit 0 ;;
        e)
            EXTERNAL=true;;
        *)
            usage 1 ;;
    esac
done

shift  $(( $OPTIND -1 ))

[ "${1}" == "--help" ] && usage 0
[ ${#} -ne 0 ] && usage 1

[ -z ${CONNECTION} ] && CONNECTION='sefirot'

PASSFILE=~/.ssh/${CONNECTION}_pass

[ -n "${EXTERNAL}" ] && CONNECTION="r${CONNECTION}"

if [ "${TYPE}" == "ssh" ];then
    sshpass -f ${PASSFILE} ssh ${CONNECTION}
elif [ "${TYPE}" == "mount" ]; then
    sshfs "${CONNECTION}:~/external" "/home/alex/mnt/cluster"
elif [ "${TYPE}" == "umount" ]; then
    [ "$(findmnt -T /home/alex/mnt/cluster | grep -q fuse.sshfs)" ] && (echo 'No mount found'; exit 1)
    umount /home/alex/mnt/cluster
fi
