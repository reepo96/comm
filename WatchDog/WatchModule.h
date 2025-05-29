
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WATCHMODULE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WATCHMODULE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WATCHMODULE_EXPORTS
#define WATCHMODULE_API __declspec(dllexport)
#else
#define WATCHMODULE_API __declspec(dllimport)
#endif

class CWatchModuleImpl;
class CMonitorPoint;

// This class is exported from the WatchModule.dll
//##ModelId=47D631A40232
class WATCHMODULE_API CWatchModule {
public:
	//##ModelId=47D731A10000
	void quitWatchDog();

	//##ModelId=47D63C45004E
	int addMonitorPoint(unsigned int count);

	//##ModelId=47D63C65009C
	void delMonitorPoint(int pointID);

	//##ModelId=47D63C8F0128
	void kickMonitorPoint(int pointID);

	//##ModelId=47D6451000DA
	bool createWatchDog();

	//##ModelId=47D6327A00FA
	 ~CWatchModule();

	//##ModelId=47D6322702FD
	CWatchModule(void);
	// TODO: add your methods here.
private:
	//##ModelId=47D6370E031C
	CWatchModuleImpl* theImpl;

	//##ModelId=47D63D93008C
	CMonitorPoint* thePoints;

};


