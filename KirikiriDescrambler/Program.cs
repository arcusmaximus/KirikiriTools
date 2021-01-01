using System;
using System.IO;
using System.Linq;
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
                Console.WriteLine($"Usage: {Assembly.GetEntryAssembly().GetName().Name} <file/folder>");
                return;
            }

            string path = args[0];
            if (Directory.Exists(path))
                DescrambleFolder(path);
            else if (File.Exists(path))
                DescrambleFile(path);
            else
                Console.WriteLine("Specified file or folder does not exist.");
        }

        private static void DescrambleFolder(string folderPath)
        {
            string[] extensions = { ".ks", ".tjs", ".txt", ".csv", ".ini" };
            foreach (string filePath in Directory.EnumerateFiles(folderPath, "*", SearchOption.AllDirectories))
            {
                if (!extensions.Contains(Path.GetExtension(filePath)?.ToLower()))
                    continue;

                try
                {
                    string content = Descrambler.Descramble(filePath);
                    if (content == null)
                        continue;

                    File.WriteAllText(filePath, content, Encoding.UTF8);
                    Console.WriteLine($"Descrambled {filePath}");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Failed to descramble {filePath}: {ex.Message}");
                }
            }
        }

        private static void DescrambleFile(string filePath)
        {
            try
            {
                string content = Descrambler.Descramble(filePath);
                if (content == null)
                {
                    Console.WriteLine("File is not scrambled.");
                    return;
                }

                File.WriteAllText(filePath, content, Encoding.UTF8);
                Console.WriteLine("File descrambled.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to descramble file: {ex.Message}");
            }
        }
    }
}
