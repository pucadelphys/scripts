#!/bin/sh

usage () {
    cat << EOF
Connect to the INMEGEN central cerver

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
}

CONNECTION='abarcenas@cluster.inmegen.gob.mx'
PASSFILE='/home/alex/.ssh/cluster_pass'

while getopts "tsehmu" ARG; do
    case "${ARG}" in
        t)
            CONNECTION="alumno7@10.0.15.11" ;;
        s)
            [[ ${CONNECTION} == "alumno7@10.0.15.11" ]] && (echo 'Two servers selected' && exit 1)
            CONNECTION="abfemat@sefirot.inmegen.gob.mx"
            PASSFILE='/home/alex/.ssh/sefi_pass' ;;
        e)
            PORT="-p 5263 " ;;
        h)
            usage
            exit ;;
        m)
            MNT=$(findmnt -T /home/alex/mnt/cluster | grep inmegen)
            [[ -z $MNT ]] && sshpass -f /home/alex/.ssh/cluster_pass sshfs abarcenas@cluster.inmegen.gob.mx:/home/abarcenas /home/alex/mnt/cluster ;;
        u)
            MNT=$(findmnt -T /home/alex/mnt/cluster | grep inmegen)
            [[ -n $MNT ]] && umount /home/alex/mnt/cluster || echo 'Mount was not found'
            exit ;;
        *)
            usage
            exit 1 ;;
    esac
done

sshpass -f ${PASSFILE} ssh ${PORT}${CONNECTION}
