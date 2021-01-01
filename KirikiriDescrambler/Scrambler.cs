using System;
using System.IO;
using System.Text;

namespace Arc.Ddsi.KirikiriDescrambler
{
    internal static class Scrambler
    {
        public static byte[] Scramble(string filePath, byte mode)
        {
            using (Stream stream = File.Open(filePath, FileMode.Open, FileAccess.Read))
            {
                return Scramble(stream, mode);
            }
        }

        public static byte[] Scramble(Stream stream, byte mode)
        {
            string text;
            using (StreamReader reader = new StreamReader(stream))
            {
                text = reader.ReadToEnd();
            }

            byte[] utf16 = Encoding.Unicode.GetBytes(text);
            MemoryStream resultStream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(resultStream);
            writer.Write((ushort)0xFEFE);
            writer.Write(mode);
            writer.Write((ushort)0xFEFF);
            switch (mode)
            {
                case 0:
                    ScrambleMode0(utf16, writer);
                    break;

                case 1:
                    ScrambleMode1(utf16, writer);
                    break;

                case 2:
                    Compress(utf16, writer);
                    break;

                default:
                    throw new ArgumentException("Invalid mode");
            }

            byte[] resultData = new byte[resultStream.Length];
            resultStream.Position = 0;
            resultStream.Read(resultData, 0, resultData.Length);
            return resultData;
        }

        private static void ScrambleMode0(byte[] data, BinaryWriter writer)
        {
            for (int i = 0; i < data.Length; i += 2)
            {
                if (data[i + 1] == 0 && data[i] < 0x20)
                    continue;

                data[i] ^= 1;
                data[i + 1] ^= (byte)(data[i] & 0xFE);
            }
            writer.Write(data);
        }

        private static void ScrambleMode1(byte[] data, BinaryWriter writer)
        {
            for (int i = 0; i < data.Length; i += 2)
            {
                char c = (char)(data[i] | (data[i + 1] << 8));
                c = (char)(((c & 0xAAAA) >> 1) | ((c & 0x5555) << 1));
                data[i] = (byte)c;
                data[i + 1] = (byte)(c >> 8);
            }
            writer.Write(data);
        }

        private static void Compress(byte[] data, BinaryWriter writer)
        {
            long startPos = writer.BaseStream.Position;

            writer.Write(0L);
            writer.Write((long)data.Length);
            using (ZlibStream stream = new ZlibStream(writer.BaseStream))
            {
                stream.Write(data, 0, data.Length);
            }

            long endPos = writer.BaseStream.Position;
            writer.BaseStream.Position = startPos;
            writer.Write(endPos - startPos - 16);
        }
    }
}
