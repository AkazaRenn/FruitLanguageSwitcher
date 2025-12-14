using System;
using Interop;

namespace App
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string message = Interop.Core.GetMessage();
            Console.WriteLine(message);
        }
    }
}
