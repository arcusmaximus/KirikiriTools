using System;
using System.IO;

namespace Arc.Ddsi.Xp3Pack
{
    public static class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: Xp3Pack folder");
                return;
            }

            string folderPath = args[0];
            if (!Directory.Exists(folderPath))
            {
                Console.WriteLine("Specified folder does not exist.");
                return;
            }

            string parentFolderPath = Path.GetDirectoryName(folderPath);
            if (parentFolderPath == null)
            {
                Console.WriteLine("Specified folder does not have a parent folder?!");
                return;
            }

            string fileName = Path.GetFileName(folderPath) + ".xp3";
            Console.WriteLine("Creating " + fileName);
            string xp3FilePath = Path.Combine(parentFolderPath, fileName);
            Xp3ArchiveWriter.Write(folderPath, xp3FilePath);
        }
    }
}
