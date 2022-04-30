using System;
using System.Collections.Generic;
using System.Text;

namespace ExtensionMethods
{
    public static class ConsoleEx
    {
        public static void WriteLine()
        {
            Console.WriteLine();
        }
        public static void WriteLine(string message)
        {
            Console.WriteLine(message);
        }

        public class ColorType
        {
            public string name { get; set; }
            public ConsoleColor color { get; set; }

            public ColorType(string _name, ConsoleColor _color)
            {
                name = _name;
                color = _color;
            }
        }

        public static ColorType Network()
        {
            return new ColorType("Network", ConsoleColor.Cyan);
        }
        public static ColorType Mining()
        {
            return new ColorType("Mining", ConsoleColor.Green);
        }
        public static ColorType Error()
        {
            return new ColorType("Error", ConsoleColor.Red);
        }
        public static ColorType Debug()
        {
            return new ColorType("Debug", ConsoleColor.DarkYellow);
        }

        public static void WriteLine(string message, ColorType type)
        {
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write("[");
            Console.ForegroundColor = type.color;
            Console.Write(type.name);
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write("]  - ");
            Console.ResetColor();

            Console.WriteLine(message);
        }
    }
}
