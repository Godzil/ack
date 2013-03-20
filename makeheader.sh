#!/bin/bash
file=${1%.c}

INCPATH=("-Ih")
INCPATH+=("-Imodules/h")
INCPATH+=("-I/tmp/ack-build/pmcache")
INCPATH+=("-I/tmp/ack-temp/headers" )
for fil in /tmp/ack-temp/pmcache/*; do
	if [ -d ${fil} ]; then
		INCPATH+=("-I${fil}")
	fi
done


if [ ! -f ${file}.h ]; then
	echo "No header, building it"
	touch ${file}.h
	cat > ${file}.h <<EOF
/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
EOF
	HEADERMAGIC=`echo ${file}.h | tr '[.-/]' '_' | tr '[a-z]' '[A-Z]'`
	BASE=`dirname ${file}.c`
	echo "#ifndef ${HEADERMAGIC}" >> ${file}.h
	echo "#define ${HEADERMAGIC}" >> ${file}.h
	echo "" >> ${file}.h
	cproto -U__BLOCKS__ ${INCPATH[*]} ${file}.c >> ${file}.h
	echo "" >> ${file}.h
	echo "#endif /* ${HEADERMAGIC} */" >> ${file}.h
	echo "" >> ${file}.h
else
	echo "Header existing... try to update.. (manual check should be done)"
	cp ${file}.h ${file}.h.old
	HEADERMAGIC=`echo ${file}.h | tr '[.-/]' '_' | tr '[a-z]' '[A-Z]'`
	BASE=`dirname ${file}.c`
	echo "#ifndef ${HEADERMAGIC}" > ${file}.h.new
	echo "#define ${HEADERMAGIC}" >> ${file}.h.new
 	cat ${file}.h.old >> ${file}.h.new
	echo "" >> ${file}.h.new
	cproto -U__BLOCKS__ ${INCPATH[*]} -I${BASE} ${file}.c >> ${file}.h.new
	echo "" >> ${file}.h.new
	echo "#endif /* ${HEADERMAGIC} */" >> ${file}.h.new
	echo "" >> ${file}.h.new
	
	cp ${file}.h.new ${file}.h
	#
fi