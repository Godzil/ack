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

POSTFIX=".h"
if [ -f ${file}.str ]; then
	POSTFIX="_loc.h"
fi

if [ ! -f ${file}${POSTFIX} ]; then
	echo "No header, building it"
	# Be carefull if a ${file}.str file exist use _loc.h postfix.
	
	HEADERMAGIC=`echo ${file}.h | tr '[.-/]' '_' | tr '[a-z]' '[A-Z]'`
	BASE=`dirname ${file}.c`

	touch ${file}${POSTFIX}
	cat > ${file}${POSTFIX} <<EOF
/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
EOF
	echo "#ifndef ${HEADERMAGIC}" >> ${file}${POSTFIX}
	echo "#define ${HEADERMAGIC}" >> ${file}${POSTFIX}
	echo "" >> ${file}${POSTFIX}
	cproto -U__BLOCKS__ ${INCPATH[*]} ${file}.c >> ${file}${POSTFIX}
	echo "" >> ${file}${POSTFIX}
	echo "#endif /* ${HEADERMAGIC} */" >> ${file}${POSTFIX}
	echo "" >> ${file}${POSTFIX}
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
	rm ${file}.h.new
	#
fi