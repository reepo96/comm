RTS_HOME = C:/Program Files/Rational/Rose RealTime/C++/TargetRTS
include <$(RTS_HOME)/codegen/rtgen.mk>
MODEL = D:/work/产品开发/SH2000D录波器/SHNeuron故障记录分析中心/开发库/Model/Communication/CommClassForRT.rtmdl
COMPONENT = "Component View::Communication"
RTGEN_FLAGS = -codesync -model $(MODEL) -component $(COMPONENT) -spacedeps dq
include <RTSystem/Communication.dep>
CODESYNC_TARGETS = $(Communication_SYSTARGET_CS)
RTCODESYNC_TARGET = $(CODESYNC_TARGETS)

RTcodesync : $(RTCODESYNC_TARGET)

.PRECIOUS : $(CODESYNC_TARGETS)

$(Communication_SYSTARGET_CS) : $(Communication_SYSDEPS_CS)
	$(RTGEN) $(RTGEN_FLAGS) -system

