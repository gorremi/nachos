#include <synch.h>


class Port
{
        public:
                Port(const char * debugName);
                const char* getName() { return (name); }
                ~Port();
                void Send(int mensaje);
                void Receive(int *mensaje);
        private:
				const char* name;
				Lock* puerto_lock;
                Condition* cond_receive;
                Condition* cond_send;
                bool leido;
                bool puedeLeer;
                bool puedeEscribir;
                int buffer;
};
