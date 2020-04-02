using System;
using System.IO;
using System.Text;

namespace Arc.Ddsi.KirikiriDescrambler
{
    public static class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: TextDescramble <dir> <*.ext>");
                return;
            }

            Console.WriteLine("Directory  : " + args[0]);
            Console.WriteLine("Files mask : " + args[1]);
            var ksFiles = Directory.EnumerateFiles(args[0], args[1], SearchOption.AllDirectories);
            foreach (string filePath in ksFiles)
            {
                Console.WriteLine("Parsing    : " + filePath);
                //Directory.Move(filePath, Path.Combine(args[0] + "\\backup", filePath));
                try
                {
                    string content = Descrambler.Descramble(filePath);
                    if (content == null || content == "")
                        return;
                    File.WriteAllText(filePath, content, Encoding.Unicode);// Encoding.UTF8);
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }


        }
    }
}
