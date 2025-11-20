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
ifeq "$(wildcard nbproject/Makefile-local-Config_3.mk)" "nbproject/Makefile-local-Config_3.mk"
include nbproject/Makefile-local-Config_3.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Config_3
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
	${MAKE}  -f nbproject/Makefile-Config_3.mk ${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24EP512GP806
MP_LINKER_FILE_OPTION=,--script=p24EP512GP806.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  .generated_files/flags/Config_3/55478cfc40d7734f024c24b52d6685862a881c6e .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/io.o: io.c  .generated_files/flags/Config_3/89fb18d68a17a6a84fe2c9a2a796b5d73dd4b98 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/io.o.d 
	@${RM} ${OBJECTDIR}/io.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  io.c  -o ${OBJECTDIR}/io.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/io.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ad.o: ad.c  .generated_files/flags/Config_3/24a3c7f2a33d73c5cf255a5c4c9fde612edc3455 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ad.o.d 
	@${RM} ${OBJECTDIR}/ad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ad.c  -o ${OBJECTDIR}/ad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ad.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/uart.o: uart.c  .generated_files/flags/Config_3/4f61c77ca3c2468892dbc9de8e776d6b308b0c2e .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart.c  -o ${OBJECTDIR}/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/uart.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/timer.o: timer.c  .generated_files/flags/Config_3/7ccb61dde7c706873518b3618f0711839cc50243 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  timer.c  -o ${OBJECTDIR}/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/app.o: app.c  .generated_files/flags/Config_3/25a056b7e585f9c5943830ecc5146851e9837c65 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app.o.d 
	@${RM} ${OBJECTDIR}/app.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app.c  -o ${OBJECTDIR}/app.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/app.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/oscillator.o: oscillator.c  .generated_files/flags/Config_3/58dceeaebb733a668820446375d834734a498f37 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/oscillator.o.d 
	@${RM} ${OBJECTDIR}/oscillator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  oscillator.c  -o ${OBJECTDIR}/oscillator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/oscillator.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/crc.o: crc.c  .generated_files/flags/Config_3/3df04bdab3449ee3651fdd7bb2119e3887b8e526 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc.o.d 
	@${RM} ${OBJECTDIR}/crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc.c  -o ${OBJECTDIR}/crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/crc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/util.o: util.c  .generated_files/flags/Config_3/80ccd4e3f243398c98a3c029a9a847589a6c77ac .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/util.o.d 
	@${RM} ${OBJECTDIR}/util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  util.c  -o ${OBJECTDIR}/util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/util.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/interrupt.o: interrupt.c  .generated_files/flags/Config_3/3a1ff53b966a4aee5f30e14f218b9098977c9700 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupt.o.d 
	@${RM} ${OBJECTDIR}/interrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupt.c  -o ${OBJECTDIR}/interrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/interrupt.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/pwm.o: pwm.c  .generated_files/flags/Config_3/73cab88e3457445a6faed32fcf661814f3d06f96 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pwm.o.d 
	@${RM} ${OBJECTDIR}/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pwm.c  -o ${OBJECTDIR}/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/pwm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/i2c.o: i2c.c  .generated_files/flags/Config_3/f08ac5face231e11fb6583e24a58b52218d32daa .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c.o.d 
	@${RM} ${OBJECTDIR}/i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c.c  -o ${OBJECTDIR}/i2c.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/i2c.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/protocol.o: protocol.c  .generated_files/flags/Config_3/a3afe20b8636c706f3380769d77a8663766dce6d .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/protocol.o.d 
	@${RM} ${OBJECTDIR}/protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  protocol.c  -o ${OBJECTDIR}/protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/protocol.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ads1271.o: ads1271.c  .generated_files/flags/Config_3/e312e2cda40ded527f10a0807e97218b8cea0c84 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ads1271.o.d 
	@${RM} ${OBJECTDIR}/ads1271.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ads1271.c  -o ${OBJECTDIR}/ads1271.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ads1271.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/spi.o: spi.c  .generated_files/flags/Config_3/54a4dc18414c790645d11cadd7b81c21abf0366b .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/spi.o.d 
	@${RM} ${OBJECTDIR}/spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  spi.c  -o ${OBJECTDIR}/spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/spi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/power.o: power.c  .generated_files/flags/Config_3/3621d16960a020615f1d34cc6fd327ec3d7a5f .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/power.o.d 
	@${RM} ${OBJECTDIR}/power.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  power.c  -o ${OBJECTDIR}/power.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/power.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/encoder.o: encoder.c  .generated_files/flags/Config_3/e1cc7ba3775b2945aa3472219102170aba780c9d .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/encoder.o.d 
	@${RM} ${OBJECTDIR}/encoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  encoder.c  -o ${OBJECTDIR}/encoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/encoder.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DS18S20.o: DS18S20.c  .generated_files/flags/Config_3/422c3820ba15d896adac4b7f8c8d2b0219589ee0 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DS18S20.o.d 
	@${RM} ${OBJECTDIR}/DS18S20.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DS18S20.c  -o ${OBJECTDIR}/DS18S20.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DS18S20.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/GeneralCurve.o: GeneralCurve.c  .generated_files/flags/Config_3/716be83ec18867f6a6115c1be0d7482dcb250c0c .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/GeneralCurve.o.d 
	@${RM} ${OBJECTDIR}/GeneralCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  GeneralCurve.c  -o ${OBJECTDIR}/GeneralCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/GeneralCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ClickCurve.o: ClickCurve.c  .generated_files/flags/Config_3/aa069a424d0b63678f0f16036fcd4cee0e144f86 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ClickCurve.o.d 
	@${RM} ${OBJECTDIR}/ClickCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ClickCurve.c  -o ${OBJECTDIR}/ClickCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ClickCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ShutOffCurve.o: ShutOffCurve.c  .generated_files/flags/Config_3/67ade4bf9a385b0b938ddda64b3d09692d972ff6 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o.d 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ShutOffCurve.c  -o ${OBJECTDIR}/ShutOffCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ShutOffCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/PeakCurve.o: PeakCurve.c  .generated_files/flags/Config_3/6abfa445af028004da892c1cc0971f2ba36fc9e4 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PeakCurve.o.d 
	@${RM} ${OBJECTDIR}/PeakCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PeakCurve.c  -o ${OBJECTDIR}/PeakCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PeakCurve.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/esp8266.o: esp8266.c  .generated_files/flags/Config_3/7bb8310c896bb9edb9d8fbf848499dd69de2f9f5 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/esp8266.o.d 
	@${RM} ${OBJECTDIR}/esp8266.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  esp8266.c  -o ${OBJECTDIR}/esp8266.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/esp8266.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/peakfinder.o: peakfinder.c  .generated_files/flags/Config_3/c485c63df3b2aa37fd847ff123c59216821ce0dd .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/peakfinder.o.d 
	@${RM} ${OBJECTDIR}/peakfinder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peakfinder.c  -o ${OBJECTDIR}/peakfinder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/peakfinder.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/trq_protocol.o: trq_protocol.c  .generated_files/flags/Config_3/ce5d7dd36b9660cffdfd17d597e35e5b7770fa33 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/trq_protocol.o.d 
	@${RM} ${OBJECTDIR}/trq_protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  trq_protocol.c  -o ${OBJECTDIR}/trq_protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/trq_protocol.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/main.o: main.c  .generated_files/flags/Config_3/18f052c1e65331a2d2e258fe6ff9ff97063f70d .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/io.o: io.c  .generated_files/flags/Config_3/77b835c9729f37ab9e06143ab05d188e2285790 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/io.o.d 
	@${RM} ${OBJECTDIR}/io.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  io.c  -o ${OBJECTDIR}/io.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/io.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ad.o: ad.c  .generated_files/flags/Config_3/498a28b37295eae0cb90b497626c944f141d143a .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ad.o.d 
	@${RM} ${OBJECTDIR}/ad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ad.c  -o ${OBJECTDIR}/ad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ad.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/uart.o: uart.c  .generated_files/flags/Config_3/3408ad784f69c42c8ab2ea614c798aa4dfa49a91 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart.c  -o ${OBJECTDIR}/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/uart.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/timer.o: timer.c  .generated_files/flags/Config_3/b28c77bf6a946041b3ae9d11224210e6c261d61b .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  timer.c  -o ${OBJECTDIR}/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/timer.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/app.o: app.c  .generated_files/flags/Config_3/5259363ae06d7081cfade976f3060dce5e4fd4d9 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app.o.d 
	@${RM} ${OBJECTDIR}/app.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app.c  -o ${OBJECTDIR}/app.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/app.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/oscillator.o: oscillator.c  .generated_files/flags/Config_3/22feca5f55a5b4b87fae784179282187788e33e0 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/oscillator.o.d 
	@${RM} ${OBJECTDIR}/oscillator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  oscillator.c  -o ${OBJECTDIR}/oscillator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/oscillator.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/crc.o: crc.c  .generated_files/flags/Config_3/2a1198fed0f0ab2600a8c343c6b4c565d8ec2ea .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc.o.d 
	@${RM} ${OBJECTDIR}/crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc.c  -o ${OBJECTDIR}/crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/crc.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/util.o: util.c  .generated_files/flags/Config_3/687f63c3672a6384f10847c00e967eec818cd69c .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/util.o.d 
	@${RM} ${OBJECTDIR}/util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  util.c  -o ${OBJECTDIR}/util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/util.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/interrupt.o: interrupt.c  .generated_files/flags/Config_3/71c360c7ceebe1dfc4a8ab7a9ebed23af932f0e2 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupt.o.d 
	@${RM} ${OBJECTDIR}/interrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupt.c  -o ${OBJECTDIR}/interrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/interrupt.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/pwm.o: pwm.c  .generated_files/flags/Config_3/5612557492ffe65445908523170c70e8facff749 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pwm.o.d 
	@${RM} ${OBJECTDIR}/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pwm.c  -o ${OBJECTDIR}/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/pwm.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/i2c.o: i2c.c  .generated_files/flags/Config_3/eb7c23eeef30f08c3cdafa51881ed4df8001f627 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c.o.d 
	@${RM} ${OBJECTDIR}/i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c.c  -o ${OBJECTDIR}/i2c.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/i2c.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/protocol.o: protocol.c  .generated_files/flags/Config_3/eebf995b70cee5b3256ea0a96642b80030f5e605 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/protocol.o.d 
	@${RM} ${OBJECTDIR}/protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  protocol.c  -o ${OBJECTDIR}/protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/protocol.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ads1271.o: ads1271.c  .generated_files/flags/Config_3/f3db7e00fdbb8707e4268d3221d59895a3dfb3cf .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ads1271.o.d 
	@${RM} ${OBJECTDIR}/ads1271.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ads1271.c  -o ${OBJECTDIR}/ads1271.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ads1271.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/spi.o: spi.c  .generated_files/flags/Config_3/61b6802f3c1431d0185d23bfa39bccabcea35559 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/spi.o.d 
	@${RM} ${OBJECTDIR}/spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  spi.c  -o ${OBJECTDIR}/spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/spi.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/power.o: power.c  .generated_files/flags/Config_3/4938bdbad2de49a503f93d64fa95aed879d28839 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/power.o.d 
	@${RM} ${OBJECTDIR}/power.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  power.c  -o ${OBJECTDIR}/power.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/power.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/encoder.o: encoder.c  .generated_files/flags/Config_3/28f66f96ba895aecd561590b7ae7289855e1a3a .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/encoder.o.d 
	@${RM} ${OBJECTDIR}/encoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  encoder.c  -o ${OBJECTDIR}/encoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/encoder.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DS18S20.o: DS18S20.c  .generated_files/flags/Config_3/acff1f0a0c8a6a9250283c4d2a2498fc1775c85d .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DS18S20.o.d 
	@${RM} ${OBJECTDIR}/DS18S20.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DS18S20.c  -o ${OBJECTDIR}/DS18S20.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DS18S20.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/GeneralCurve.o: GeneralCurve.c  .generated_files/flags/Config_3/1f417de048e6be9abb75e9d6a3109067a502ea25 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/GeneralCurve.o.d 
	@${RM} ${OBJECTDIR}/GeneralCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  GeneralCurve.c  -o ${OBJECTDIR}/GeneralCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/GeneralCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ClickCurve.o: ClickCurve.c  .generated_files/flags/Config_3/87cc5e0ccfa41046fdac9a5893406482ff68a762 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ClickCurve.o.d 
	@${RM} ${OBJECTDIR}/ClickCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ClickCurve.c  -o ${OBJECTDIR}/ClickCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ClickCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ShutOffCurve.o: ShutOffCurve.c  .generated_files/flags/Config_3/f4e77e81b4a753f748fc85a42d61b7adcabb4fc9 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o.d 
	@${RM} ${OBJECTDIR}/ShutOffCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ShutOffCurve.c  -o ${OBJECTDIR}/ShutOffCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ShutOffCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/PeakCurve.o: PeakCurve.c  .generated_files/flags/Config_3/f3f8cabfb06e8449c1660a3460750f43f66b167b .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PeakCurve.o.d 
	@${RM} ${OBJECTDIR}/PeakCurve.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PeakCurve.c  -o ${OBJECTDIR}/PeakCurve.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PeakCurve.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/esp8266.o: esp8266.c  .generated_files/flags/Config_3/5178c0b92645add9c1d0b2dbb16c4cf404464b03 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/esp8266.o.d 
	@${RM} ${OBJECTDIR}/esp8266.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  esp8266.c  -o ${OBJECTDIR}/esp8266.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/esp8266.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/peakfinder.o: peakfinder.c  .generated_files/flags/Config_3/a7aa35baaaf645fc9a46968d3fc7e7562d0ec4a9 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/peakfinder.o.d 
	@${RM} ${OBJECTDIR}/peakfinder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peakfinder.c  -o ${OBJECTDIR}/peakfinder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/peakfinder.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/trq_protocol.o: trq_protocol.c  .generated_files/flags/Config_3/926cb4bb3acf79c91e061327bed9982a11dfc1e7 .generated_files/flags/Config_3/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/trq_protocol.o.d 
	@${RM} ${OBJECTDIR}/trq_protocol.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  trq_protocol.c  -o ${OBJECTDIR}/trq_protocol.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/trq_protocol.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-data -msmall-scalar -mconst-in-code -menable-large-arrays -O1 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
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
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--stack=300,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="OUTPUT.MAP",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/PHOENIX_TRANSDUCER.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_Config_3=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=300,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="OUTPUT.MAP",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
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
