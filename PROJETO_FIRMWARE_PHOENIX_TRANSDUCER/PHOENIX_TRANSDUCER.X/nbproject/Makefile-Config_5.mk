#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-Config_5.mk)" "nbproject/Makefile-local-Config_5.mk"
include nbproject/Makefile-local-Config_5.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Config_5
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=main.c io.c ad.c uart.c timer.c app.c oscillator.c crc.c util.c interrupt.c pwm.c i2c.c protocol.c ads1271.c spi.c power.c encoder.c DS18S20.c GeneralCurve.c ClickCurve.c ShutOffCurve.c PeakCurve.c esp8266.c peakfinder.c trq_protocol.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/io.o ${OBJECTDIR}/ad.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/timer.o ${OBJECTDIR}/app.o ${OBJECTDIR}/oscillator.o ${OBJECTDIR}/crc.o ${OBJECTDIR}/util.o ${OBJECTDIR}/interrupt.o ${OBJECTDIR}/pwm.o ${OBJECTDIR}/i2c.o ${OBJECTDIR}/protocol.o ${OBJECTDIR}/ads1271.o ${OBJECTDIR}/spi.o ${OBJECTDIR}/power.o ${OBJECTDIR}/encoder.o ${OBJECTDIR}/DS18S20.o ${OBJECTDIR}/GeneralCurve.o ${OBJECTDIR}/ClickCurve.o ${OBJECTDIR}/ShutOffCurve.o ${OBJECTDIR}/PeakCurve.o ${OBJECTDIR}/esp8266.o ${OBJECTDIR}/peakfinder.o ${OBJECTDIR}/trq_protocol.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/io.o.d ${OBJECTDIR}/ad.o.d ${OBJECTDIR}/uart.o.d ${OBJECTDIR}/timer.o.d ${OBJECTDIR}/app.o.d ${OBJECTDIR}/oscillator.o.d ${OBJECTDIR}/crc.o.d ${OBJECTDIR}/util.o.d ${OBJECTDIR}/interrupt.o.d ${OBJECTDIR}/pwm.o.d ${OBJECTDIR}/i2c.o.d ${OBJECTDIR}/protocol.o.d ${OBJECTDIR}/ads1271.o.d ${OBJECTDIR}/spi.o.d ${OBJECTDIR}/power.o.d ${OBJECTDIR}/encoder.o.d ${OBJECTDIR}/DS18S20.o.d ${OBJECTDIR}/GeneralCurve.o.d ${OBJECTDIR}/ClickCurve.o.d ${OBJECTDIR}/ShutOffCurve.o.d ${OBJECTDIR}/PeakCurve.o.d ${OBJECTDIR}/esp8266.o.d ${OBJECTDIR}/peakfinder.o.d ${OBJECTDIR}/trq_protocol.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/io.o ${OBJECTDIR}/ad.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/timer.o ${OBJECTDIR}/app.o ${OBJECTDIR}/oscillator.o ${OBJECTDIR}/crc.o ${OBJECTDIR}/util.o ${OBJECTDIR}/interrupt.o ${OBJECTDIR}/pwm.o ${OBJECTDIR}/i2c.o ${OBJECTDIR}/protocol.o ${OBJECTDIR}/ads1271.o ${OBJECTDIR}/spi.o ${OBJECTDIR}/power.o ${OBJECTDIR}/encoder.o ${OBJECTDIR}/DS18S20.o ${OBJECTDIR}/GeneralCurve.o ${OBJECTDIR}/ClickCurve.o ${OBJECTDIR}/ShutOffCurve.o ${OBJECTDIR}/PeakCurve.o ${OBJECTDIR}/esp8266.o ${OBJECTDIR}/peakfinder.o ${OBJECTDIR}/trq_protocol.o

# Source Files
SOURCEFILES=main.c io.c ad.c uart.c timer.c app.c oscillator.c crc.c util.c interrupt.c pwm.c i2c.c protocol.c ads1271.c spi.c power.c encoder.c DS18S20.c GeneralCurve.c ClickCurve.c ShutOffCurve.c PeakCurve.c esp8266.c peakfinder.c trq_protocol.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-Config_5.mk ${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24EP512GP806
MP_LINKER_FILE_OPTION=,--script=p24EP512GP806.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  .generated_files/flags/Config_5/67f087ee80f627351fedaeac8648d9c273c57f6a .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/io.o: io.c  .generated_files/flags/Config_5/44359a817e680c8f402de636ad31cef069dd9251 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/io.o.d 
	@${RM} ${OBJECTDIR}/io.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  io.c  -o ${OBJECTDIR}/io.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/io.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ad.o: ad.c  .generated_files/flags/Config_5/dc80e2c3f15cea0c72fa54a69e9d2344c5769ba9 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ad.o.d 
	@${RM} ${OBJECTDIR}/ad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ad.c  -o ${OBJECTDIR}/ad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ad.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/uart.o: uart.c  .generated_files/flags/Config_5/9ebec349a19e585cc87d4a0093506b3bbc75afbb .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart.c  -o ${OBJECTDIR}/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/uart.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/timer.o: timer.c  .generated_files/flags/Config_5/bcab544cbbd871577f8a83da13dc2c1ad8662c25 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  timer.c  -o ${OBJECTDIR}/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/app.o: app.c  .generated_files/flags/Config_5/a6da9d742c9b23f3838668457c4c82cbfa273b53 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app.o.d 
	@${RM} ${OBJECTDIR}/app.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app.c  -o ${OBJECTDIR}/app.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/app.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/oscillator.o: oscillator.c  .generated_files/flags/Config_5/ca25463dbe698f4d5fba53942f7237bd637cb2bd .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/oscillator.o.d 
	@${RM} ${OBJECTDIR}/oscillator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  oscillator.c  -o ${OBJECTDIR}/oscillator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/oscillator.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/crc.o: crc.c  .generated_files/flags/Config_5/9c7cb344f1fe9640897720a9f8ae016a26a4eb79 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc.o.d 
	@${RM} ${OBJECTDIR}/crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc.c  -o ${OBJECTDIR}/crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/crc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/util.o: util.c  .generated_files/flags/Config_5/8607e490b85a4bd0a15bed88dcc51d82cdf47d5 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/util.o.d 
	@${RM} ${OBJECTDIR}/util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  util.c  -o ${OBJECTDIR}/util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/util.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/interrupt.o: interrupt.c  .generated_files/flags/Config_5/15fb42ce4fea0d24813c9d137cba85146c8cd27c .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupt.o.d 
	@${RM} ${OBJECTDIR}/interrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupt.c  -o ${OBJECTDIR}/interrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/interrupt.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/pwm.o: pwm.c  .generated_files/flags/Config_5/76959b8658f6f9d504be8770bc4ed1f4c704a207 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pwm.o.d 
	@${RM} ${OBJECTDIR}/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pwm.c  -o ${OBJECTDIR}/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/pwm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/i2c.o: i2c.c  .generated_files/flags/Config_5/2e3094d75fd88889bfd7e14b30e2ac31775b2994 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c.o.d 
	@${RM} ${OBJECTDIR}/i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c.c  -o ${OBJECTDIR}/i2c.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/i2c.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/protocol.o: protocol.c  .generated_files/flags/Config_5/1d7c03cc0ca5863374dbb69effdff765d833dc29 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/protocol.o.d 
	@${RM} ${OBJECTDIR}/protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  protocol.c  -o ${OBJECTDIR}/protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/protocol.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ads1271.o: ads1271.c  .generated_files/flags/Config_5/19d89a6d70c28ad7d3ae071364846f4bf5a88d83 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ads1271.o.d 
	@${RM} ${OBJECTDIR}/ads1271.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ads1271.c  -o ${OBJECTDIR}/ads1271.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ads1271.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/spi.o: spi.c  .generated_files/flags/Config_5/c62d3ab1761322152af96193a1ff41151d1e8976 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/spi.o.d 
	@${RM} ${OBJECTDIR}/spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  spi.c  -o ${OBJECTDIR}/spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/spi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/power.o: power.c  .generated_files/flags/Config_5/f0742ba489ace80399bce4e0e29f8f8af9bf99fd .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/power.o.d 
	@${RM} ${OBJECTDIR}/power.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  power.c  -o ${OBJECTDIR}/power.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/power.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/encoder.o: encoder.c  .generated_files/flags/Config_5/176ac578ec849ed8a65e46b241487b0f7b73ead3 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/encoder.o.d 
	@${RM} ${OBJECTDIR}/encoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  encoder.c  -o ${OBJECTDIR}/encoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/encoder.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DS18S20.o: DS18S20.c  .generated_files/flags/Config_5/fc6e272c77f98f93cd63957297cf1f9330dbd953 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DS18S20.o.d 
	@${RM} ${OBJECTDIR}/DS18S20.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DS18S20.c  -o ${OBJECTDIR}/DS18S20.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DS18S20.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/GeneralCurve.o: GeneralCurve.c  .generated_files/flags/Config_5/aebbfbce809c8ebdbfd1bff90490564ed8f650a5 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/GeneralCurve.o.d 
	@${RM} ${OBJECTDIR}/GeneralCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  GeneralCurve.c  -o ${OBJECTDIR}/GeneralCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/GeneralCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ClickCurve.o: ClickCurve.c  .generated_files/flags/Config_5/1bfda1e75d89be1a42f9a4d0326266a4c2daa9a2 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ClickCurve.o.d 
	@${RM} ${OBJECTDIR}/ClickCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ClickCurve.c  -o ${OBJECTDIR}/ClickCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ClickCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ShutOffCurve.o: ShutOffCurve.c  .generated_files/flags/Config_5/c244ceb842f5c0ac004d243136b3c9ffcd467be2 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o.d 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ShutOffCurve.c  -o ${OBJECTDIR}/ShutOffCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ShutOffCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/PeakCurve.o: PeakCurve.c  .generated_files/flags/Config_5/1f2fbedaf594a8d5ed54f0de59f643ea3b4c0a02 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PeakCurve.o.d 
	@${RM} ${OBJECTDIR}/PeakCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PeakCurve.c  -o ${OBJECTDIR}/PeakCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PeakCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/esp8266.o: esp8266.c  .generated_files/flags/Config_5/2abf8e5a0754b31f69857f783861fe3e9d84cf21 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/esp8266.o.d 
	@${RM} ${OBJECTDIR}/esp8266.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  esp8266.c  -o ${OBJECTDIR}/esp8266.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/esp8266.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/peakfinder.o: peakfinder.c  .generated_files/flags/Config_5/d93a028911291b75c0af2bfc3650ca1775980d77 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/peakfinder.o.d 
	@${RM} ${OBJECTDIR}/peakfinder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peakfinder.c  -o ${OBJECTDIR}/peakfinder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/peakfinder.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/trq_protocol.o: trq_protocol.c  .generated_files/flags/Config_5/24f30de1c53032a39eb725f192df04af0585ec32 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/trq_protocol.o.d 
	@${RM} ${OBJECTDIR}/trq_protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  trq_protocol.c  -o ${OBJECTDIR}/trq_protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/trq_protocol.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/main.o: main.c  .generated_files/flags/Config_5/ba65dedcb213f53fa76370409cbe79968ab03495 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/io.o: io.c  .generated_files/flags/Config_5/8a530fbb360eb5c0e7cd07abaec03964a4faab43 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/io.o.d 
	@${RM} ${OBJECTDIR}/io.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  io.c  -o ${OBJECTDIR}/io.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/io.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ad.o: ad.c  .generated_files/flags/Config_5/3ba9233d7b31bf83a1118f5033f16e196c1b6767 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ad.o.d 
	@${RM} ${OBJECTDIR}/ad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ad.c  -o ${OBJECTDIR}/ad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ad.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/uart.o: uart.c  .generated_files/flags/Config_5/6188d6142e685cd97c2604c0c546a39dbc6e50ab .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart.c  -o ${OBJECTDIR}/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/uart.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/timer.o: timer.c  .generated_files/flags/Config_5/4b53f95517b9fc26687d77f2925f6ddab8c7a5bb .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  timer.c  -o ${OBJECTDIR}/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/timer.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/app.o: app.c  .generated_files/flags/Config_5/f9c49f862c8535b8c9d2334e155fa88a6a7db652 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app.o.d 
	@${RM} ${OBJECTDIR}/app.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app.c  -o ${OBJECTDIR}/app.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/app.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/oscillator.o: oscillator.c  .generated_files/flags/Config_5/81cecf91f122538b497ad3f5f4f8f2b79cf57a69 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/oscillator.o.d 
	@${RM} ${OBJECTDIR}/oscillator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  oscillator.c  -o ${OBJECTDIR}/oscillator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/oscillator.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/crc.o: crc.c  .generated_files/flags/Config_5/9c0b440473a924eb085a1f7f018ed8bd00c773c6 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc.o.d 
	@${RM} ${OBJECTDIR}/crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc.c  -o ${OBJECTDIR}/crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/crc.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/util.o: util.c  .generated_files/flags/Config_5/b4fcea896093f92d820cf99bca093f9027a8e0b1 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/util.o.d 
	@${RM} ${OBJECTDIR}/util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  util.c  -o ${OBJECTDIR}/util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/util.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/interrupt.o: interrupt.c  .generated_files/flags/Config_5/1c17e8fc0f2d6923ee39f457e9fa0870c244729 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupt.o.d 
	@${RM} ${OBJECTDIR}/interrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupt.c  -o ${OBJECTDIR}/interrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/interrupt.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/pwm.o: pwm.c  .generated_files/flags/Config_5/9afda9b672d254fced3a789028d44e6288172cc3 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pwm.o.d 
	@${RM} ${OBJECTDIR}/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pwm.c  -o ${OBJECTDIR}/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/pwm.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/i2c.o: i2c.c  .generated_files/flags/Config_5/77ea3e855528a6f558d98e620d6d27456ae4b447 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c.o.d 
	@${RM} ${OBJECTDIR}/i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c.c  -o ${OBJECTDIR}/i2c.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/i2c.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/protocol.o: protocol.c  .generated_files/flags/Config_5/91e8fd2febc14ed8f9f033164ce7ceac027fb6fc .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/protocol.o.d 
	@${RM} ${OBJECTDIR}/protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  protocol.c  -o ${OBJECTDIR}/protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/protocol.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ads1271.o: ads1271.c  .generated_files/flags/Config_5/dd47288cb1232ba74715cfec4364a88b06ae6c48 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ads1271.o.d 
	@${RM} ${OBJECTDIR}/ads1271.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ads1271.c  -o ${OBJECTDIR}/ads1271.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ads1271.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/spi.o: spi.c  .generated_files/flags/Config_5/3cbc8915064abbf8c35393c6b216c62e5f20ad5 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/spi.o.d 
	@${RM} ${OBJECTDIR}/spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  spi.c  -o ${OBJECTDIR}/spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/spi.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/power.o: power.c  .generated_files/flags/Config_5/2e47aae9e0c9f869cc605316363d8165a8cfd548 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/power.o.d 
	@${RM} ${OBJECTDIR}/power.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  power.c  -o ${OBJECTDIR}/power.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/power.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/encoder.o: encoder.c  .generated_files/flags/Config_5/eb7df6e570179e1ff3f98c29e4bf5f06a4c0f95b .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/encoder.o.d 
	@${RM} ${OBJECTDIR}/encoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  encoder.c  -o ${OBJECTDIR}/encoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/encoder.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DS18S20.o: DS18S20.c  .generated_files/flags/Config_5/b39ad99d966041f779f65c35e311b5b1e6603f28 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DS18S20.o.d 
	@${RM} ${OBJECTDIR}/DS18S20.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DS18S20.c  -o ${OBJECTDIR}/DS18S20.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DS18S20.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/GeneralCurve.o: GeneralCurve.c  .generated_files/flags/Config_5/12fa8afd2f67dd79912883f2166c63d0362d7fb4 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/GeneralCurve.o.d 
	@${RM} ${OBJECTDIR}/GeneralCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  GeneralCurve.c  -o ${OBJECTDIR}/GeneralCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/GeneralCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ClickCurve.o: ClickCurve.c  .generated_files/flags/Config_5/c172f4472ca5730046eec1fbf063f2110ecd14d7 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ClickCurve.o.d 
	@${RM} ${OBJECTDIR}/ClickCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ClickCurve.c  -o ${OBJECTDIR}/ClickCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ClickCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ShutOffCurve.o: ShutOffCurve.c  .generated_files/flags/Config_5/a65beb024718ccc4d9089d878cc492e588ccf794 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o.d 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ShutOffCurve.c  -o ${OBJECTDIR}/ShutOffCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ShutOffCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/PeakCurve.o: PeakCurve.c  .generated_files/flags/Config_5/212b930e6f9794fd9fa984df477fc0038f3b3a71 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PeakCurve.o.d 
	@${RM} ${OBJECTDIR}/PeakCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PeakCurve.c  -o ${OBJECTDIR}/PeakCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PeakCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/esp8266.o: esp8266.c  .generated_files/flags/Config_5/e9150128a6db67450aecbb40952c8389eda2a4a4 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/esp8266.o.d 
	@${RM} ${OBJECTDIR}/esp8266.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  esp8266.c  -o ${OBJECTDIR}/esp8266.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/esp8266.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/peakfinder.o: peakfinder.c  .generated_files/flags/Config_5/9742b5625529da2c678e1663c8310cef2f7d6d28 .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/peakfinder.o.d 
	@${RM} ${OBJECTDIR}/peakfinder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peakfinder.c  -o ${OBJECTDIR}/peakfinder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/peakfinder.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/trq_protocol.o: trq_protocol.c  .generated_files/flags/Config_5/348c511f70f415b8681b093b08c4f47b29c7336f .generated_files/flags/Config_5/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/trq_protocol.o.d 
	@${RM} ${OBJECTDIR}/trq_protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  trq_protocol.c  -o ${OBJECTDIR}/trq_protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/trq_protocol.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--stack=300,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="OUTPUT.MAP",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_Config_5=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=300,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="OUTPUT.MAP",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex ${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
