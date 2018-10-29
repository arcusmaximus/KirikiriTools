using System.IO;

namespace Arc.Ddsi.Xp3Pack
{
    internal class Xp3ArchiveWriter
    {
        private static readonly byte[] HeaderMagic = { 0x58, 0x50, 0x33, 0x0D, 0x0A, 0x20, 0x0A, 0x1A, 0x8B, 0x67, 0x01 };

        public static void Write(string folderPath, string xp3FilePath)
        {
            if (!folderPath.EndsWith("\\") && !folderPath.EndsWith("/"))
                folderPath += "\\";

            using (Stream xp3Stream = File.Open(xp3FilePath, FileMode.Create, FileAccess.Write))
            using (BinaryWriter xp3Writer = new BinaryWriter(xp3Stream))
            {
                xp3Writer.Write(HeaderMagic);
                xp3Writer.Write(0L);        // Index offset

                Xp3IndexBuilder index = new Xp3IndexBuilder();

                foreach (string filePath in Directory.EnumerateFiles(folderPath, "*", SearchOption.AllDirectories))
                {
                    long offset = xp3Stream.Position;
                    long originalSize;
                    long compressedSize;
                    AppendFile(xp3Stream, filePath, out originalSize, out compressedSize);

                    string relativeFilePath = filePath.Substring(folderPath.Length);
                    index.Add(relativeFilePath, offset, originalSize, compressedSize);
                }

                long indexOffset = xp3Stream.Length;
                xp3Writer.Write(index.Build());

                xp3Stream.Seek(HeaderMagic.Length, SeekOrigin.Begin);
                xp3Writer.Write(indexOffset);
            }
        }

        private static void AppendFile(Stream xp3Stream, string filePath, out long originalSize, out long compressedSize)
        {
            using (Stream fileStream = File.OpenRead(filePath))
            {
                originalSize = fileStream.Length;

                long startPos = xp3Stream.Position;
                using (ZlibStream compressionStream = new ZlibStream(xp3Stream))
                {
                    fileStream.CopyTo(compressionStream);
                }
                compressedSize = xp3Stream.Position - startPos;
            }
        }
    }
}
