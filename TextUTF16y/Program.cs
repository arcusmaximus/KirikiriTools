using System;
using System.IO;
using System.Text;

namespace Arc.Ddsi.TextUTF16y
{
    public static class Program
    {
        /// <summary>
        /// Determines a text file's encoding by analyzing its byte order mark (BOM).
        /// Defaults to Shift_JIS when detection of the text file's endianness fails.
        /// </summary>
        /// <param name="filename">The text file to analyze.</param>
        /// <returns>The detected encoding.</returns>
        public static Encoding GetEncoding(string filename)
        {
            // Read the BOM
            var bom = new byte[4];
            using (var file = new FileStream(filename, FileMode.Open, FileAccess.Read))
            {
                file.Read(bom, 0, 4);
            }

            // Analyze the BOM
            if (bom[0] == 0x2b && bom[1] == 0x2f && bom[2] == 0x76) return Encoding.UTF7;
            if (bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf) return Encoding.UTF8;
            if (bom[0] == 0xff && bom[1] == 0xfe) return Encoding.Unicode; //UTF-16LE
            if (bom[0] == 0xfe && bom[1] == 0xff) return Encoding.BigEndianUnicode; //UTF-16BE
            if (bom[0] == 0 && bom[1] == 0 && bom[2] == 0xfe && bom[3] == 0xff) return Encoding.UTF32;
            // defaults to Shift_JIS for this project
            return System.Text.Encoding.GetEncoding(932);
        }

        public static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: TextUTF16y <dir> <*.ext>");
                return;
            }

            Console.WriteLine("Directory  : " + args[0]);
            Console.WriteLine("Files mask : " + args[1]);
            var ksFiles = Directory.EnumerateFiles(args[0], args[1], SearchOption.AllDirectories);
            foreach (string filePath in ksFiles)
            {
                //Directory.Move(filePath, Path.Combine(args[0] + "\\backup", filePath));
                try
                {
                    Encoding enc = GetEncoding(filePath);
                    Console.WriteLine("Parsing    : " + filePath + "(" +  enc.WebName + ")");
                    if (enc == null)
                        return;
                    string str = System.IO.File.ReadAllText(filePath, enc);
                    if (str == null || str == "")
                        return;
                    File.WriteAllText(filePath, str, Encoding.Unicode);// Encoding.UTF8);
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }


        }
    }
}
