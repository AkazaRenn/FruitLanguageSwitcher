using System;
using Interop;

namespace App {
    internal class Program {
        static void Main(string[] args) {
            Interop.Core.Instance.OnEvent += Obj_OnSomethingHappened;
            Console.ReadLine();
        }
        private static void Obj_OnSomethingHappened() {
            Console.WriteLine("App: Event received");
        }

    }
}
