#!/usr/bin/env sh
# get data
wget https://github.com/OSMNames/OSMNames/releases/download/v2.1.1/planet-latest-100k_geonames.tsv.gz 

# extract belgium
zcat planet-latest-100k_geonames.tsv.gz | \
    awk -F $'\t' 'BEGIN {OFS = FS} {if(NR!=1){if($16 =="be"){print}}else{print}}' | \
    gzip - > countryExtract.tsv.gz 

# split double names, add alt names (+ ignore errors in Brussel)
 zcat countryExtract.tsv.gz | \
    awk -F $'\t' 'BEGIN{OFS="\t"} {n=split($1,a,";");for(i=1;i<=n;i++){print($4,a[i],$9,$7,$8)}} $9 > 16 && $2 != "\"\"" && $2 ~ /^[A-Za-z ]*$/ {print($4,$2,$9,$7,$8)}'