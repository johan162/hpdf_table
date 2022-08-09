#!/usr/bin/env bash
TOKEN=kallekalle
nodes=("hpdf_table_ub18" "hpdf_table_ub20" "hpdf_table_ub22")
for n in ${nodes[@]}; do
	curl  "http://192.168.64.12:8080/buildByToken/build?job=${n}&token=${TOKEN}"
	echo $n
done
echo "Done."

