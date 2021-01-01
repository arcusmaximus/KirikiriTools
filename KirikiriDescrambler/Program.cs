using System;
using System.IO;
using System.Reflection;
using System.Text;

namespace Arc.Ddsi.KirikiriDescrambler
{
    public static class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine($"Usage: {Assembly.GetEntryAssembly().GetName().Name} <file>");
                return;
            }

            string filePath = args[0];
            if (!File.Exists(filePath))
            {
                Console.WriteLine("Specified file does not exist.");
                return;
            }

            try
            {
                string content = Descrambler.Descramble(filePath);
                if (content == null)
                    return;

                File.WriteAllText(filePath, content, Encoding.UTF8);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
