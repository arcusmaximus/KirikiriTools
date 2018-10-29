using System;
using System.IO;
using System.IO.Compression;
using System.Text;

namespace Arc.Ddsi.KirikiriDescrambler
{
    internal static class Descrambler
    {
        public static string Descramble(string filePath)
        {
            using (Stream stream = File.Open(filePath, FileMode.Open, FileAccess.Read))
            {
                return Descramble(stream);
            }
        }

        public static string Descramble(Stream stream)
        {
            BinaryReader reader = new BinaryReader(stream);
            byte[] magic = reader.ReadBytes(2);
            if (magic[0] != 0xFE || magic[1] != 0xFE)
                throw new InvalidDataException("Not a scrambled Kirikiri file.");

            byte mode = reader.ReadByte();
            byte[] bom = reader.ReadBytes(2);
            if (bom[0] != 0xFF || bom[1] != 0xFE)
                throw new InvalidDataException("Not a scrambled Kirikiri file.");

            byte[] utf16;
            switch (mode)
            {
                case 0:
                    utf16 = DescrambleMode0(reader);
                    break;

                case 1:
                    utf16 = DescrambleMode1(reader);
                    break;

                case 2:
                    utf16 = Decompress(reader);
                    break;

                default:
                    throw new NotSupportedException($"File uses unsupported scrambling mode {mode}");
            }
            return Encoding.Unicode.GetString(utf16);
        }

        private static byte[] DescrambleMode0(BinaryReader reader)
        {
            byte[] data = reader.ReadBytes((int)(reader.BaseStream.Length - reader.BaseStream.Position));
            for (int i = 0; i < data.Length; i += 2)
            {
                if (data[i + 1] == 0 && data[i] < 0x20)
                    continue;

                data[i + 1] ^= (byte)(data[i] & 0xFE);
                data[i] ^= 1;
            }
            return data;
        }

        private static byte[] DescrambleMode1(BinaryReader reader)
        {
            byte[] data = reader.ReadBytes((int)(reader.BaseStream.Length - reader.BaseStream.Position));
            for (int i = 0; i < data.Length; i += 2)
            {
                char c = (char)(data[i] | (data[i + 1] << 8));
                c = (char)(((c & 0xAAAA) >> 1) | ((c & 0x5555) << 1));
                data[i] = (byte)c;
                data[i + 1] = (byte)(c >> 8);
            }
            return data;
        }

        private static byte[] Decompress(BinaryReader reader)
        {
            int compressedLength = (int)reader.ReadInt64();
            int uncompressedLength = (int)reader.ReadInt64();
            short zlibHeader = reader.ReadInt16();

            byte[] uncompressedData = new byte[uncompressedLength];
            using (DeflateStream stream = new DeflateStream(reader.BaseStream, CompressionMode.Decompress, true))
            {
                stream.Read(uncompressedData, 0, uncompressedLength);
            }
            return uncompressedData;
        }
    }
}
