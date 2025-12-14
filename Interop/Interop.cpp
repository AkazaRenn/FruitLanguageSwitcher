using namespace System;

namespace Interop {
    public ref class Core
    {
    public:
        static String^ GetMessage()
        {
            return "Hello from Interop Core!";
        }
    };
}
