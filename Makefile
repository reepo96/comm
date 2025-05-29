RTS_HOME = C:/Program Files/Rational/Rose RealTime/C++/TargetRTS
include <$(RTS_HOME)/codegen/rtgen.mk>
PERL = rtperl
IN = $(PERL) "$(ROSERT_HOME)/codegen/chdir_run.pl"
NOP = $(PERL) "$(ROSERT_HOME)/codegen/nop.pl"
MODEL = D:/work/产品开发/SH2000D录波器/SHNeuron故障记录分析中心/开发库/Model/Communication/CommClassForRT.rtmdl
COMPONENT = Component View::Communication
RTcompile : RTgenerate
	$(IN) build rtperl -S rtsetup.pl nmake -nologo -k -s RTcompile

RTpurify : RTgenerate
	$(IN) build rtperl -S rtsetup.pl nmake -nologo -k -s RTpurify

RTmycompile :
	$(IN) build rtperl -S rtsetup.pl nmake -nologo -k -s RTcompile

RTgenerate :
	$(IN) src nmake -nologo -s RTgenerate

RTmygenerate :
	$(IN) src nmake -nologo -s RTgenerate

RTcodesync :
	$(IN) src nmake -nologo -s -f Makefile.cs RTcodesync

RTmycodesync :
	$(IN) src nmake -nologo -s -f Makefile.cs RTcodesync

RTclean :
	$(PERL) "$(RTS_HOME)/codegen/rm.pl" -f -R src build Makefile Makefile.dep rtbuild.dat

RTmyclean :
	$(PERL) "$(RTS_HOME)/codegen/rm.pl" -f -R src build Makefile Makefile.dep rtbuild.dat

clean : RTclean

makefiles : RTmakefiles

regenerate : RTclean
	$(RTGEN) -model "$(MODEL)" -component "$(COMPONENT)" -makegen
	$(IN) src nmake -nologo -s RTgenerate

recompile : regenerate
	$(IN) build rtperl -S rtsetup.pl nmake -nologo -k -s RTcompile

RTmakefiles : Makefile

include <Makefile.dep>
Makefile : $(Makefile_DEPS) $(Makefile_TARGETS)
	$(RTGEN) -model "$(MODEL)" -component "$(COMPONENT)" -makegen

$(Makefile_TARGETS) :
	$(NOP)

