#if !defined(__com_openmono_toucher_h)
#define __com_openmono_toucher_h
#include <mono.h>

class AppController;

class Toucher
:
	public mono::TouchResponder
{
	AppController * ctrl;
public:
	Toucher (AppController *);
	void RespondTouchEnd (mono::TouchEvent &);
};

#endif // __com_openmono_toucher_h
