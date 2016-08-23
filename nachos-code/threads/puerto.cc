#include "puerto.h"

Port::Port(const char *debugName)
{
	name = debugName;
	puerto_lock = new Lock(debugName);
	cond_send = new Condition(debugName , puerto_lock);
	cond_receive = new Condition(debugName , puerto_lock);
	leido = false;
	puedeLeer=false;
	puedeEscribir=true;
}

Port::~Port()
{
	delete puerto_lock;
	delete cond_send;
	delete cond_receive;
}
       
void Port::Send(int mensaje)
{
	puerto_lock->Acquire();
	while(!puedeEscribir)
		cond_receive->Wait();
	buffer = mensaje;
	puedeEscribir = false;
	puedeLeer = true;
	cond_send->Signal();
	while(!leido)
		cond_receive->Wait();
	leido = false;
    puerto_lock->Release();
}

void Port::Receive(int *mensaje)
{
	puerto_lock->Acquire();
	while(!puedeLeer)
			cond_send->Wait();
	*mensaje = buffer;
	puedeEscribir = true;
	leido = true;
	cond_receive->Broadcast();
	puedeLeer = false;
	puerto_lock->Release();
}
