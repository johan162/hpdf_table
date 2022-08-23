#!/usr/bin/env bash
# Notes: Assume the existence of JENKINS_HOST and JENKINS_TOKEN environment variables

if [[ -z ${JENKINS_HOST} ]]; then
	echo "Error. JENKINS_HOST not set."
	exit 1
fi

if [[ -z ${JENKINS_TOKEN} ]]; then
        echo "Error. JENKINS_TOKEN not set."
        exit 1
fi

nodes=("hpdf_table_ub18" "hpdf_table_ub20" "hpdf_table_ub22")
for n in ${nodes[@]}; do
	curl  "http://${JENKINS_HOST}:8080/buildByToken/build?job=${n}&token=${JENKINS_TOKEN}"
	echo $n
done
echo "Done."

