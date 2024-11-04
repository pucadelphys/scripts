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
Connect to the INMEGEN servers, sefirot by default.

Usage: hh [OPTION]

    Options:
        -m
            Mount remote home to ~/mnt/cluster
        -h
            Print usage
        -u
            Unmount remote home
        -s
            Connect to central server
        -d
            Connect to drona server
        -f
            Connect to fenix server
EOF
    exit ${1}
}


# while getopts "tcshmue" ARG; do
while getopts "tfcshmudp:j" ARG; do
    case "${ARG}" in
        f)
            [ -n "${CONNECTION}" ] && echo 'You may only specify one server' && exit 1
            CONNECTION="fenix";;
        d)
            [ -n "${CONNECTION}" ] && echo 'You may only specify one server' && exit 1
            CONNECTION="drona";;
        s)
            [ -n "${CONNECTION}" ] && echo 'You may only specify one server' && exit 1
            CONNECTION="central";;
        h)
            usage ;;
        m)
            [ -n "${TYPE}" ] && echo 'You may only specify one -muc' && usage 1
            TYPE='mount';;
        u)
            [ -n "${TYPE}" ] && echo 'You may only specify one -muc' && usage 1
            TYPE='umount';;
        c)
            [ -n "${TYPE}" ] && echo 'You may only specify one -muc' && usage 1
            TYPE='colors';;
        # e)
            # EXTERNAL=true;;
        p)
            [ -n "${TYPE}" ] && echo 'You may only specify one -muc' && usage 1
            INLOC=${OPTARG##*:}
            OUTLOC=${OPTARG%%:*}
            TYPE='paste';;
        j)
            [ -n "${TYPE}" ] && echo 'The j flag is for standard connections only' && usage 1
            JUPYTER=true;;
        *)
            usage 1 ;;
    esac
done

shift  $(( $OPTIND -1 ))

[ "${1}" == "--help" ] && usage
[ ${#} -ne 0 ] && usage 1

[ -z ${TYPE} ] && TYPE='ssh'
[ -z ${CONNECTION} ] && CONNECTION='sefirot'

PASSFILE=~/.ssh/secrets/${CONNECTION}_pass

DEFAULT_HOST=$(ip route | grep default -m 1 | cut -d' ' -f 3)
echo -e "Default host is: \033[1m${DEFAULT_HOST}\033[0m"
[[ "${DEFAULT_HOST}" =~ 192.168.(60.1|17.254|105.254) ]] || CONNECTION="r${CONNECTION}"
# [ -n "${EXTERNAL}" ] && CONNECTION="r${CONNECTION}"


if [ "${TYPE}" == "ssh" ];then
    [[ "${CONNECTION}" == "rdrona" ]] && echo "Drona can be accessed only from the INMEGEN network" && exit
    sshpass -f ${PASSFILE} ssh ${JUPYTER:+-L 8080:127.0.0.1:8080 }${CONNECTION}
elif [ "${TYPE}" == "mount" ]; then
    [ ! -d ~/mnt/cluster ] && mkdir -p ~/mnt/cluster
    sshpass -f ${PASSFILE} ssh ${CONNECTION} "[ ! -d ~/external ] && mkdir ~/external"
    sshfs -o allow_other,default_permissions,ssh_command="sshpass -f /home/alex/.ssh/sefirot_pass ssh" ${CONNECTION}':external' "/home/alex/mnt/cluster"
elif [ "${TYPE}" == "umount" ]; then
    [ "$(findmnt -T /home/alex/mnt/cluster | grep -q fuse.sshfs)" ] && (echo 'No mount found'; exit 1)
    umount /home/alex/mnt/cluster
elif [ "${TYPE}" == "colors" ]; then
    sshpass -f ${PASSFILE} scp ~/.cache/wal/colors.sh ${CONNECTION}:~/.cache/wal
fi
