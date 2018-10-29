using System;
using System.IO;
using System.Linq;
using ICSharpCode.SharpZipLib.Zip.Compression.Streams;

namespace Arc.Ddsi.Xp3Pack
{
    internal class Xp3IndexExtractor
    {
        private static readonly byte[] ExpectedMagic = { 0x58, 0x50, 0x33, 0x0D, 0x0A, 0x20, 0x0A, 0x1A, 0x8B, 0x67, 0x01 };

        private const int TVP_XP3_INDEX_ENCODE_METHOD_MASK = 7;
        private const int TVP_XP3_INDEX_ENCODE_RAW = 0;
        private const int TVP_XP3_INDEX_ENCODE_ZLIB = 1;

        private const int TVP_XP3_INDEX_CONTINUE = 0x80;

        public static void Extract(string xp3FilePath)
        {
            using (Stream stream = File.OpenRead(xp3FilePath))
            {
                BinaryReader reader = new BinaryReader(stream);
                byte[] magic = reader.ReadBytes(ExpectedMagic.Length);
                if (!magic.SequenceEqual(ExpectedMagic))
                    throw new InvalidDataException("Invalid header magic");

                while (true)
                {
                    long indexOffset = reader.ReadInt64();
                    stream.Position = indexOffset;

                    byte flags = reader.ReadByte();
                    int indexSize = (int)reader.ReadInt64();

                    if ((flags & TVP_XP3_INDEX_CONTINUE) != 0)
                    {
                        if ((flags & TVP_XP3_INDEX_ENCODE_METHOD_MASK) != TVP_XP3_INDEX_ENCODE_RAW)
                            throw new NotSupportedException();

                        if (indexSize != 0)
                            throw new NotSupportedException();

                        continue;
                    }

                    byte[] index;
                    switch (flags & TVP_XP3_INDEX_ENCODE_METHOD_MASK)
                    {
                        case TVP_XP3_INDEX_ENCODE_ZLIB:
                            int compressedSize = indexSize;
                            indexSize = (int)reader.ReadInt64();

                            index = new byte[indexSize];
                            using (Stream inflateStream = new InflaterInputStream(stream))
                            {
                                inflateStream.Read(index, 0, indexSize);
                            }
                            break;

                        case TVP_XP3_INDEX_ENCODE_RAW:
                            index = new byte[indexSize];
                            stream.Read(index, 0, indexSize);
                            break;

                        default:
                            throw new NotSupportedException();
                    }

                    File.WriteAllBytes(Path.ChangeExtension(xp3FilePath, ".index"), index);
                    return;
                }
            }
        }
    }
}
