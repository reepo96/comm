RTS_HOME = C:/Program Files/Rational/Rose RealTime/C++/TargetRTS
include <$(RTS_HOME)/codegen/rtgen.mk>
MODEL = D:/work/��Ʒ����/SH2000D¼����/SHNeuron���ϼ�¼��������/������/Model/Communication/CommClassForRT.rtmdl
COMPONENT = "Component View::Communication"
RTGEN_FLAGS = -codesync -model $(MODEL) -component $(COMPONENT) -spacedeps dq
include <RTSystem/Communication.dep>
CODESYNC_TARGETS = $(Communication_SYSTARGET_CS)
RTCODESYNC_TARGET = $(CODESYNC_TARGETS)

RTcodesync : $(RTCODESYNC_TARGET)

.PRECIOUS : $(CODESYNC_TARGETS)

$(Communication_SYSTARGET_CS) : $(Communication_SYSDEPS_CS)
	$(RTGEN) $(RTGEN_FLAGS) -system

